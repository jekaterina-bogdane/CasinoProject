#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qboxlayout.h"
#include "usermanager.h"
#include "slotmachine.h"
#include "roulettegame.h"
#include "blackjackgame.h"
#include "adminwindow.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnLogin_clicked();

    void on_btnRegister_clicked();

    void on_btnCancel_clicked();

    void on_btnLeave_clicked();

    void on_btnCancelReg_clicked();

    void on_lblToLogin_linkActivated(const QString &link);

    void on_lblToRegister_linkActivated(const QString &link);

    void on_btnLoginConfirm_clicked();

    void on_btnRegisterConfirm_clicked();

    void on_btnLeaveStore_clicked();

    void on_btnPurchase10_clicked();

    void on_btnPurchase20_clicked();

    void on_btnPurchase50_clicked();

    void on_btnPurchase100_clicked();

    void on_btnPurchase250_clicked();

    void on_btnPurchase500_clicked();

    void on_btnSlotGameSpin_clicked();

    void on_SGQB10_clicked();

    void on_SGQB50_clicked();

    void on_SGQB100_clicked();

    void on_SGQB500_clicked();

    void on_btnLeaveSlotMachine_clicked();

    void on_btnLeaveRoulette_clicked();

    void on_btnLeaveAccount_clicked();

    void on_RGQB10_clicked();

    void on_RGQB50_clicked();

    void on_RGQB100_clicked();

    void on_RGQB500_clicked();

    void on_btnRouletteRed_clicked();

    void on_btnRouletteBlack_clicked();

    void on_btnRouletteEven_clicked();

    void on_btnRouletteOdd_clicked();

    void on_btnRouletteLow_clicked();

    void on_btnRouletteHigh_clicked();

    void on_btnRoulette1_12_clicked();

    void on_btnRoulette13_24_clicked();

    void on_btnRoulette25_36_clicked();

    void on_btnRouletteColumn1_clicked();

    void on_btnRouletteColumn2_clicked();

    void on_btnRouletteColumn3_clicked();

    void on_btnRouletteSingleNumber_clicked();

    void on_btnRouletteClearBet_clicked();

    void on_btnRouletteGameSpin_clicked();

    void on_btnLeaveBlackjack_clicked();

    void on_btnBlackjackGameDeal_clicked();

    void on_btnBlackjackGameHit_clicked();

    void on_btnBlackjackGameStand_clicked();

    void on_BJQB10_clicked();

    void on_BJQB50_clicked();

    void on_BJQB100_clicked();

    void on_BJQB500_clicked();

private:
    void playSlideTransition(const QString &direction, std::function<void()> onMidpoint);
    void refreshUserInfo();

    UserManager userManager;

    //Slot Machine
    SlotMachine slotMachine;

    //timers for slot machine reel
    QTimer* reelTimerL;
    QTimer* reelTimerM;
    QTimer* reelTimerR;

    void animateReelL();
    void animateReelM();
    void animateReelR();

    void stopReelL();
    void stopReelM();
    void stopReelR();

    void checkAllReelsStopped();

    int reelStopCounter = 0; // tracks how many reels have stopped

    QStringList finalReels;  // stores the final spin result
    QString getWinColorSlotGame(const QStringList& reels, int payout, int bet); //gets win color for slot machine

    int slotCurrentBet = 10; // default bet for slots

    //Roulette
    RouletteGame rouletteGame;
    void setRouletteBet(BetType type, const QString& label);

    bool rouletteBetSelected = false;
    BetType rouletteCurrentBetType = BetType::Red;
    int rouletteCurrentBetAmount = 10;
    int rouletteCurrentNumber = 0;

    void animateRouletteSpin();

    QTimer* rouletteSpinTimer;
    int rouletteSpinIndex = 0;
    int rouletteSpinSpeed = 50; // ms
    int rouletteFinalNumber = 0;

    QVector<int> rouletteWheelOrder = {
        0, 32, 15, 19, 4, 21, 2, 25, 17, 34, 6,
        27, 13, 36, 11, 30, 8, 23, 10, 5, 24,
        16, 33, 1, 20, 14, 31, 9, 22, 18, 29,
        7, 28, 12, 35, 3, 26
    };
    void finishRouletteSpin();

    SpinResult lastSpinResult;
    Bet lastBet;

    //Blackjack

    BlackjackGame blackjack;
    int blackjackCurrentBet = 10;

    void clearLayout(QLayout* layout);
    void addCard(QHBoxLayout* layout, int value);
    Ui::MainWindow *ui;

    //Account
    void refreshAccountPage();

};
#endif // MAINWINDOW_H
