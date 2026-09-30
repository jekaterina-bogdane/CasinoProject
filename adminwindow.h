#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include "usermanager.h"
#include <QWidget>

namespace Ui {
class AdminWindow;
}

class AdminWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AdminWindow(UserManager* manager, QWidget *parent = nullptr);
    ~AdminWindow();

private slots:
    void on_btnBanUser_clicked();

    void on_btnUnbanUser_clicked();

    void on_btnDeleteUser_clicked();

    void on_btnLeaveAdmin_clicked();

private:
    Ui::AdminWindow *ui;

    UserManager* userManager;

    void refreshUserTable();
};

#endif // ADMINWINDOW_H
