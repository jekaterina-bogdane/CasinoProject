#ifndef DIALOG2FA_H
#define DIALOG2FA_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class Dialog2FA;
}

class Dialog2FA : public QDialog
{
    Q_OBJECT

public:
    enum ResultAction {
        None,
        OpenMainMenu,
        OpenStore,
        OpenAdminPanel
    };
    void setAction(ResultAction action);

    explicit Dialog2FA(QWidget *parent = nullptr);
    ~Dialog2FA();

private slots:
    void on_btnCancel_clicked();

    void on_btnConfirm_clicked();

signals:
    void twoFAConfirmed(Dialog2FA::ResultAction action);

private:
    ResultAction action = None;
    QString codeFilePath;
    QString generatedCode;

    void generateCode();

    QTimer countdownTimer;
    int timeLeft = 60;

    Ui::Dialog2FA *ui;
};



#endif // DIALOG2FA_H
