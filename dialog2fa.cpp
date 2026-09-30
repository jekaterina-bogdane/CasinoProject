#include "dialog2fa.h"
#include "ui_dialog2fa.h"
#include <QMainWindow>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QDir>

Dialog2FA::Dialog2FA(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog2FA)
{
    ui->setupUi(this);

    // Build AppData path
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appData);
    codeFilePath = appData + "/2fa.txt";
    generateCode();

    timeLeft = 60;
    ui->lbl2FACountdown->setText("Time left: 60s");

    connect(&countdownTimer, &QTimer::timeout, this, [this]() {
        timeLeft--;
        ui->lbl2FACountdown->setText("Time left: " + QString::number(timeLeft) + "s");

        if (timeLeft <= 0) {
            countdownTimer.stop();
            this->close();   // auto-cancel 2FA
        }
    });

    countdownTimer.start(1000); // 1 second interval
}

Dialog2FA::~Dialog2FA()
{
    delete ui;
}

void Dialog2FA::on_btnCancel_clicked()
{
    countdownTimer.stop();
    this->close();
}

void Dialog2FA::setAction(ResultAction action)
{
    this->action = action;
}

void Dialog2FA::generateCode()
{
    generatedCode.clear();

    for (int i = 0; i < 6; i++) {
        int digit = QRandomGenerator::global()->bounded(10);
        generatedCode.append(QString::number(digit));
    }

    // Write code to file
    QFile file(codeFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << generatedCode;
        qDebug() << "2FA code written:" << generatedCode;
        file.close();
    }
    else {
        qDebug() << "Failed to write to 2FA file";
    }
}

void Dialog2FA::on_btnConfirm_clicked()
{
    QString entered = ui->txt2FACode->text().trimmed();

    // Read code from file
    QFile file(codeFilePath);
    QString storedCode;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        storedCode = in.readAll().trimmed();
        file.close();
    }

    if (entered == storedCode) {
        countdownTimer.stop();   // stop countdown
        emit twoFAConfirmed(action);
        QFile::remove(codeFilePath);

    } else {
        ui->lblError->setText("Incorrect code.");
    }
}





