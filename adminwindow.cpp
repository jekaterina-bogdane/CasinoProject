#include "adminwindow.h"
#include "ui_adminwindow.h"

AdminWindow::AdminWindow(UserManager* manager, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminWindow)
    , userManager(manager)
{
    ui->setupUi(this);

    //allow selection of rows, but disable editing
    ui->tableUsers->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableUsers->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableUsers->setSelectionBehavior(QAbstractItemView::SelectRows);

    refreshUserTable();
}


AdminWindow::~AdminWindow()
{
    delete ui;
}

void AdminWindow::refreshUserTable()
{
    const auto& users = userManager->getUsers();

    ui->tableUsers->setRowCount(users.size());
    ui->tableUsers->setColumnCount(3);

    QStringList headers;
    headers << "Username" << "Balance" << "Blocked";
    ui->tableUsers->setHorizontalHeaderLabels(headers);

    int row = 0;
    for (const User& u : users)
    {
        ui->tableUsers->setItem(row, 0, new QTableWidgetItem(u.username));
        ui->tableUsers->setItem(row, 1, new QTableWidgetItem(QString::number(u.balance)));
        ui->tableUsers->setItem(row, 2, new QTableWidgetItem(u.blocked ? "Yes" : "No"));

        // Optional: color blocked users red
        if (u.blocked) {
            for (int col = 0; col < 3; col++) {
                ui->tableUsers->item(row, col)->setForeground(Qt::red);
            }
        }

        row++;
    }

    ui->tableUsers->resizeColumnsToContents();
}

void AdminWindow::on_btnBanUser_clicked()
{
    QString name = ui->txtBanUser->text().trimmed();
    if (name.isEmpty()) {
        ui->lblBanStatus->setText("Enter a username.");
        return;
    }

    User* u = userManager->findUser(name);
    if (!u) {
        ui->lblBanStatus->setText("User not found.");
        return;
    }

    if (u->isAdmin) {
        ui->lblBanStatus->setText("Cannot ban admin.");
        return;
    }

    u->blocked = true;
    userManager->saveUsers();

    ui->lblBanStatus->setText("User banned.");
    refreshUserTable();
}


void AdminWindow::on_btnUnbanUser_clicked()
{
    QString name = ui->txtBanUser->text().trimmed();
    if (name.isEmpty()) {
        ui->lblBanStatus->setText("Enter a username.");
        return;
    }

    User* u = userManager->findUser(name);
    if (!u) {
        ui->lblBanStatus->setText("User not found.");
        return;
    }

    u->blocked = false;
    userManager->saveUsers();

    ui->lblBanStatus->setText("User unbanned.");
    refreshUserTable();
}


void AdminWindow::on_btnDeleteUser_clicked()
{
    QString name = ui->txtDeleteUser->text().trimmed();
    if (name.isEmpty()) {
        ui->lblDeleteStatus->setText("Enter a username.");
        return;
    }

    User* u = userManager->findUser(name);
    if (!u) {
        ui->lblDeleteStatus->setText("User not found.");
        return;
    }

    if (u->isAdmin) {
        ui->lblDeleteStatus->setText("Cannot delete admin.");
        return;
    }

    // Remove user from storage
    bool success = userManager->deleteUser(name);

    if (success) {
        ui->lblDeleteStatus->setText("User deleted.");
        refreshUserTable();
    } else {
        ui->lblDeleteStatus->setText("Failed to delete user.");
    }
}


void AdminWindow::on_btnLeaveAdmin_clicked()
{
    close();
}

