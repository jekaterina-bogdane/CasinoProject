#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDialog>
#include "dialog2fa.h"
#include "clickableframe.h"
#include <QTimer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPropertyAnimation>
#include <QRandomGenerator>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    userManager.initializeStorage();

    ui->stackedWidget->setCurrentWidget(ui->WelcomePage);

    reelTimerL = new QTimer(this);
    reelTimerM = new QTimer(this);
    reelTimerR = new QTimer(this);

    connect(reelTimerL, &QTimer::timeout, this, &MainWindow::animateReelL);
    connect(reelTimerM, &QTimer::timeout, this, &MainWindow::animateReelM);
    connect(reelTimerR, &QTimer::timeout, this, &MainWindow::animateReelR);

    rouletteSpinTimer = new QTimer(this);
    connect(rouletteSpinTimer, &QTimer::timeout, this, &MainWindow::animateRouletteSpin);

    //Game Card Transitions
    connect(ui->frameBlackjack, &ClickableFrame::clicked, this, [this]() {
        playSlideTransition("left", [this]() {
            refreshUserInfo();
            ui->stackedWidget->setCurrentWidget(ui->Blackjack);
        });
        refreshUserInfo();
    });

    connect(ui->frameRoulette, &ClickableFrame::clicked, this, [this]() {
        playSlideTransition("left", [this]() {
            refreshUserInfo();
            ui->stackedWidget->setCurrentWidget(ui->Roulette);
        });
        refreshUserInfo();
    });

    connect(ui->frameSlots, &ClickableFrame::clicked, this, [this]() {
        playSlideTransition("left", [this]() {
            refreshUserInfo();
            ui->stackedWidget->setCurrentWidget(ui->SlotMachine);
        });
        refreshUserInfo();
    });
    //---------------------------------------------------------------------

    //Sub Menu Transitions
    connect(ui->frameStore, &ClickableFrame::clicked, this, [this]() {
        // run 2fa to open store
        Dialog2FA dlg(this);
        dlg.setAction(Dialog2FA::OpenStore);

        connect(&dlg, &Dialog2FA::twoFAConfirmed, this, [this, &dlg](Dialog2FA::ResultAction action) {
            if (action == Dialog2FA::OpenStore)
            {
                // fade out the dialog
                QPropertyAnimation* fade = new QPropertyAnimation(&dlg, "windowOpacity");
                fade->setDuration(250);
                fade->setStartValue(1.0);
                fade->setEndValue(0.0);

                connect(fade, &QPropertyAnimation::finished, this, [this, &dlg]() {
                    dlg.close();

                    // start transition after dialog fades out
                    playSlideTransition("up", [this]() {
                        ui->stackedWidget->setCurrentWidget(ui->Store);
                    });
                });

                fade->start(QAbstractAnimation::DeleteWhenStopped);
            }
        });


        dlg.exec();
        refreshUserInfo();
    });

    connect(ui->frameAccount, &ClickableFrame::clicked, this, [this]() {
        playSlideTransition("up", [this]() {
            ui->stackedWidget->setCurrentWidget(ui->Account);
        });
        refreshUserInfo();
        refreshAccountPage();
    });

    connect(ui->frameLeaveToWelcome, &ClickableFrame::clicked, this, [this]() {
        playSlideTransition("up", [this]() {
            ui->stackedWidget->setCurrentWidget(ui->WelcomePage);
        });
        refreshUserInfo();
    });

    //Frame Top Bar Cancellations
    ui->frameAccount->setDrawTopBar(false);
    ui->frameStore->setDrawTopBar(false);
    ui->frameLeaveToWelcome->setDrawTopBar(false);



}

MainWindow::~MainWindow()
{
    delete ui;
}

//Slide Transition Animation
void MainWindow::playSlideTransition(const QString &direction, std::function<void()> onMidpoint)
{
    //Create overlay
    QGraphicsView *view = new QGraphicsView(this);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameShape(QFrame::NoFrame);
    view->setStyleSheet("background: transparent;");
    view->setAttribute(Qt::WA_TransparentForMouseEvents);
    view->setGeometry(0, 0, width(), height());
    view->raise();
    view->show();


    //Set new scene
    QGraphicsScene *scene = new QGraphicsScene(view);
    view->setScene(scene);
    scene->setSceneRect(0, 0, width(), height());

    // Black rectangle to cover screen
    scene->addRect(0, 0, width(), height(), Qt::NoPen, QBrush(Qt::black));

    //Positions
    QPoint startPos;
    QPoint midPos(0, 0);
    QPoint endPos;

    int w = width();
    int h = height();

    if (direction == "left") {
        startPos = QPoint(w, 0);
        endPos   = QPoint(-w, 0);
    }
    else if (direction == "right") {
        startPos = QPoint(-w, 0);
        endPos   = QPoint(w, 0);
    }
    else if (direction == "up") {
        startPos = QPoint(0, h);
        endPos   = QPoint(0, -h);
    }
    else if (direction == "down") {
        startPos = QPoint(0, -h);
        endPos   = QPoint(0, h);
    }

    view->move(startPos);

    // Slide in
    auto animIn = new QPropertyAnimation(view, "pos");
    animIn->setDuration(450);
    animIn->setStartValue(startPos);
    animIn->setEndValue(midPos);
    animIn->setEasingCurve(QEasingCurve::InOutQuart);

    // Slide out
    auto animOut = new QPropertyAnimation(view, "pos");
    animOut->setDuration(450);
    animOut->setStartValue(midPos);
    animOut->setEndValue(endPos);
    animOut->setEasingCurve(QEasingCurve::InOutQuart);

    connect(animIn, &QPropertyAnimation::finished, this, [=]() {
        setUpdatesEnabled(false);
        onMidpoint();
        setUpdatesEnabled(true);

        QTimer::singleShot(10, [=]() {
            animOut->start();
        });
    });

    connect(animOut, &QPropertyAnimation::finished, view, &QGraphicsView::deleteLater);
    animIn->start();
}

void MainWindow::refreshUserInfo()
{
    User* u = userManager.getCurrentUser();
    if (!u) return;

    //refresh store balance
    ui->lblBalanceActualStore->setText(QString::number(u->balance) + "C");

    //refresh slot machine balance
    ui->lblSlotGameBalance->setText("Balance: " + QString::number(u->balance) + "C");

    //refresh roulette balance
    ui->lblRouletteGameBalance->setText("Balance: " + QString::number(u->balance) + "C");

    //refresh blackjack balance
    ui->lblBlackjackGameBalance->setText("Balance: " + QString::number(u->balance) + "C");

}

void MainWindow::on_btnLogin_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->LoginPage);
}


void MainWindow::on_btnRegister_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->RegisterPage);
}


void MainWindow::on_btnCancel_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->WelcomePage);
}


void MainWindow::on_btnLeave_clicked()
{
    QApplication::quit();
}


void MainWindow::on_btnCancelReg_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->WelcomePage);
}


void MainWindow::on_lblToLogin_linkActivated(const QString &link)
{
    playSlideTransition("left", [this]() {
        ui->stackedWidget->setCurrentWidget(ui->LoginPage);
    });
    ui->stackedWidget->setCurrentWidget(ui->LoginPage);
}


void MainWindow::on_lblToRegister_linkActivated(const QString &link)
{
    playSlideTransition("right", [this]() {
        ui->stackedWidget->setCurrentWidget(ui->RegisterPage);
    });
    ui->stackedWidget->setCurrentWidget(ui->RegisterPage);
}


void MainWindow::on_btnLoginConfirm_clicked()
{
    QString username = ui->txtUsername->text().trimmed();
    QString password = ui->txtPassword->text();

    LoginResult result = userManager.login(username, password);

    switch (result)
    {
    case LoginResult::UserNotFound:
        ui->lblLoginError->setText("User does not exist.");
        return;

    case LoginResult::WrongPassword:
        ui->lblLoginError->setText("Incorrect password.");
        return;

    case LoginResult::Blocked:
        ui->lblLoginError->setText("This account is blocked.");
        return;

    case LoginResult::TooManyAttempts:
        ui->lblLoginError->setText("Too many failed attempts.");
        ui->txtPassword->clear();
        ui->stackedWidget->setCurrentWidget(ui->WelcomePage);
        return;

    case LoginResult::Success:
        ui->txtPassword->clear();
        break; // continue to 2FA
    }



    // on login success, run 2fa
    Dialog2FA dlg(this);

    User* u = userManager.getCurrentUser();
    if (u && u->isAdmin) {
        dlg.setAction(Dialog2FA::OpenAdminPanel);
    } else {
        dlg.setAction(Dialog2FA::OpenMainMenu);
    }

    connect(&dlg, &Dialog2FA::twoFAConfirmed, this, [this, &dlg](Dialog2FA::ResultAction action) {
        // ADMIN BRANCH
        if (action == Dialog2FA::OpenAdminPanel)
        {
            QPropertyAnimation* fade = new QPropertyAnimation(&dlg, "windowOpacity");
            fade->setDuration(250);
            fade->setStartValue(1.0);
            fade->setEndValue(0.0);

            connect(fade, &QPropertyAnimation::finished, this, [this, &dlg]() {
                dlg.close();

                this->hide();

                AdminWindow* admin = new AdminWindow(&userManager);
                admin->setAttribute(Qt::WA_DeleteOnClose);

                // When admin closes, show main window again
                connect(admin, &QWidget::destroyed, this, [this]() {
                    this->show();
                });

                admin->show();
            });

            fade->start(QAbstractAnimation::DeleteWhenStopped);
            return;
        }

        // NORMAL USER BRANCH
        if (action == Dialog2FA::OpenMainMenu)
        {
            QPropertyAnimation* fade = new QPropertyAnimation(&dlg, "windowOpacity");
            fade->setDuration(250);
            fade->setStartValue(1.0);
            fade->setEndValue(0.0);

            connect(fade, &QPropertyAnimation::finished, this, [this, &dlg]() {
                dlg.close();

                playSlideTransition("left", [this]() {
                    ui->stackedWidget->setCurrentWidget(ui->MainMenu);
                });
            });

            fade->start(QAbstractAnimation::DeleteWhenStopped);
            return;
        }
    });


    dlg.exec();

}




void MainWindow::on_btnRegisterConfirm_clicked()
{
    QString username = ui->txtUsername_Reg->text().trimmed();
    QString password = ui->txtPassword_Reg->text();

    RegisterResult result = userManager.registerUser(username, password);

    switch (result)
    {
    case RegisterResult::InvalidInput:
        ui->lblRegisterError->setText("Please enter a valid username and password.");
        return;

    case RegisterResult::UsernameExists:
        ui->lblRegisterError->setText("This username is already taken.");
        return;

    case RegisterResult::AdminAlreadyExists:
        ui->lblRegisterError->setText("Admin account already exists.");
        return;

    case RegisterResult::Success:
        break; // continue
    }

    // on success move to login page
    ui->lblRegisterError->clear();
    ui->txtPassword_Reg->clear();

    playSlideTransition("right", [this]() {
        ui->stackedWidget->setCurrentWidget(ui->LoginPage);
    });

    // pre-fill username in login page
    ui->txtUsername->setText(username);
}




void MainWindow::on_btnLeaveStore_clicked()
{
    playSlideTransition("down", [this]() {
        ui->stackedWidget->setCurrentWidget(ui->MainMenu);
    });
    refreshUserInfo();
}


void MainWindow::on_btnPurchase10_clicked()
{
    userManager.addChips(10);
    refreshUserInfo();
}



void MainWindow::on_btnPurchase20_clicked()
{
    userManager.addChips(20);
    refreshUserInfo();
}


void MainWindow::on_btnPurchase50_clicked()
{
    userManager.addChips(50);
    refreshUserInfo();
}


void MainWindow::on_btnPurchase100_clicked()
{
    userManager.addChips(100);
    refreshUserInfo();
}


void MainWindow::on_btnPurchase250_clicked()
{
    userManager.addChips(250);
    refreshUserInfo();
}


void MainWindow::on_btnPurchase500_clicked()
{
    userManager.addChips(500);
    refreshUserInfo();
}


void MainWindow::on_btnSlotGameSpin_clicked()
{

    int bet = slotCurrentBet;

    if (bet <= 0) {
        ui->lblSlotGameResult->setText("Invalid bet.");
        return;
    }

    User* u = userManager.getCurrentUser();

    if (u->balance < bet) {
        ui->lblSlotGameResult->setText("Not enough chips.");
        return;
    }

    // Deduct bet immediately
    userManager.addChips(-bet);
    refreshUserInfo();

    //add played stat
    u->slotGamesPlayed++;
    userManager.saveUsers();

    // Disable spin button during animation
    ui->btnSlotGameSpin->setEnabled(false);

    // Get final result now (but don't show it yet)
    finalReels = slotMachine.spin();

    reelStopCounter = 0;

    // Start animation
    reelTimerL->start(50);
    reelTimerM->start(50);
    reelTimerR->start(50);

    // Stop each reel at different times
    QTimer::singleShot(300, this, &MainWindow::stopReelL);
    QTimer::singleShot(600, this, &MainWindow::stopReelM);
    QTimer::singleShot(900, this, &MainWindow::stopReelR);
}


void MainWindow::on_SGQB10_clicked()
{
    slotCurrentBet = 10;
    ui->sbSlotGameBet->setValue(10);
}


void MainWindow::on_SGQB50_clicked()
{
    slotCurrentBet = 50;
    ui->sbSlotGameBet->setValue(50);
}


void MainWindow::on_SGQB100_clicked()
{
    slotCurrentBet = 100;
    ui->sbSlotGameBet->setValue(100);
}


void MainWindow::on_SGQB500_clicked()
{
    slotCurrentBet = 500;
    ui->sbSlotGameBet->setValue(500);
}

void MainWindow::animateReelL()
{
    int index = QRandomGenerator::global()->bounded(slotMachine.getSymbols().size());
    ui->lblReelDisplay_L->setText(slotMachine.getSymbols()[index]);
}

void MainWindow::animateReelM()
{
    int index = QRandomGenerator::global()->bounded(slotMachine.getSymbols().size());
    ui->lblReelDisplay_M->setText(slotMachine.getSymbols()[index]);
}

void MainWindow::animateReelR()
{
    int index = QRandomGenerator::global()->bounded(slotMachine.getSymbols().size());
    ui->lblReelDisplay_R->setText(slotMachine.getSymbols()[index]);
}

void MainWindow::stopReelL()
{
    reelTimerL->stop();
    ui->lblReelDisplay_L->setText(finalReels[0]);
    checkAllReelsStopped();
}

void MainWindow::stopReelM()
{
    reelTimerM->stop();
    ui->lblReelDisplay_M->setText(finalReels[1]);
    checkAllReelsStopped();
}

void MainWindow::stopReelR()
{
    reelTimerR->stop();
    ui->lblReelDisplay_R->setText(finalReels[2]);
    checkAllReelsStopped();
}

void MainWindow::checkAllReelsStopped()
{
    reelStopCounter++;

    if (reelStopCounter < 3)
        return;

    // All reels have stopped so calculate  the payout
    int bet = slotCurrentBet;
    int payout = slotMachine.calculatePayout(finalReels, bet);

    QString color = getWinColorSlotGame(finalReels, payout, bet);

    ui->lblSlotGameResult->setStyleSheet("color: " + color + ";");

    if (payout > 0) {
        userManager.addChips(payout);
        ui->lblSlotGameResult->setText("You won: " + QString::number(payout) + "C");
    } else {
        ui->lblSlotGameResult->setText("You lost!");
    }

    refreshUserInfo();

    // Re-enable spin button
    ui->btnSlotGameSpin->setEnabled(true);
}

QString MainWindow::getWinColorSlotGame(const QStringList& reels, int payout, int bet)
{
    if (payout == 0)
        return "red";

    if (reels[0] == "BAR" && reels[1] == "BAR" && reels[2] == "BAR")
        return "gold";

    if (payout == bet * 10)
        return "orange";

    if (payout == bet * 2)
        return "green";

    return "white"; // no game played
}


void MainWindow::on_btnLeaveSlotMachine_clicked()
{
    playSlideTransition("right", [this]() {
        ui->stackedWidget->setCurrentWidget(ui->MainMenu);
    });
    refreshUserInfo();
}

void MainWindow::on_btnLeaveRoulette_clicked()
{
    playSlideTransition("right", [this]() {
        ui->stackedWidget->setCurrentWidget(ui->MainMenu);
    });
    refreshUserInfo();
}


void MainWindow::on_btnLeaveAccount_clicked()
{
    playSlideTransition("down", [this]() {
        ui->stackedWidget->setCurrentWidget(ui->MainMenu);
    });
    refreshUserInfo();
}


void MainWindow::on_RGQB10_clicked()
{
    rouletteCurrentBetAmount = 10;
    ui->sbRouletteGameBet->setValue(10);
}


void MainWindow::on_RGQB50_clicked()
{
    rouletteCurrentBetAmount = 50;
    ui->sbRouletteGameBet->setValue(50);
}


void MainWindow::on_RGQB100_clicked()
{
    rouletteCurrentBetAmount = 100;
    ui->sbRouletteGameBet->setValue(100);
}


void MainWindow::on_RGQB500_clicked()
{
    rouletteCurrentBetAmount = 500;
    ui->sbRouletteGameBet->setValue(500);
}

void MainWindow::setRouletteBet(BetType type, const QString& label)
{
    rouletteCurrentBetType = type;
    rouletteBetSelected = true;
    ui->lblRouletteBetType->setText(label);
}

void MainWindow::on_btnRouletteRed_clicked()
{
    setRouletteBet(BetType::Red, "Red (1:1)");
}


void MainWindow::on_btnRouletteBlack_clicked()
{
    setRouletteBet(BetType::Black, "Black (1:1)");
}


void MainWindow::on_btnRouletteEven_clicked()
{
    setRouletteBet(BetType::Even, "Even (1:1)");
}


void MainWindow::on_btnRouletteOdd_clicked()
{
    setRouletteBet(BetType::Odd, "Odd (1:1)");
}


void MainWindow::on_btnRouletteLow_clicked()
{
    setRouletteBet(BetType::Low, "Low (1:1)");
}


void MainWindow::on_btnRouletteHigh_clicked()
{
    setRouletteBet(BetType::High, "High (1:1)");
}


void MainWindow::on_btnRoulette1_12_clicked()
{
    setRouletteBet(BetType::Dozen1, "1-12 (2:1)");
}


void MainWindow::on_btnRoulette13_24_clicked()
{
    setRouletteBet(BetType::Dozen2, "13-24 (2:1)");
}


void MainWindow::on_btnRoulette25_36_clicked()
{
    setRouletteBet(BetType::Dozen3, "25-36 (2:1)");
}


void MainWindow::on_btnRouletteColumn1_clicked()
{
    setRouletteBet(BetType::Column1, "Column 1 (2:1)");
}


void MainWindow::on_btnRouletteColumn2_clicked()
{
    setRouletteBet(BetType::Column2, "Column 2 (2:1)");
}


void MainWindow::on_btnRouletteColumn3_clicked()
{
    setRouletteBet(BetType::Column3, "Column 3 (2:1)");
}


void MainWindow::on_btnRouletteSingleNumber_clicked()
{
    rouletteCurrentNumber = ui->sbRouletteGameNumberSelection->value();
    setRouletteBet(BetType::Number,
                   "Number " + QString::number(rouletteCurrentNumber) + " (35:1)");
}


void MainWindow::on_btnRouletteClearBet_clicked()
{
    rouletteBetSelected = false;
    rouletteCurrentNumber = 0;
    ui->sbRouletteGameBet->setValue(0);
    ui->lblRouletteBetType->setText("No bet selected");
}


void MainWindow::on_btnRouletteGameSpin_clicked()
{
    if (!rouletteBetSelected) {
        ui->lblRouletteGameResult->setText("No bet selected.");
        return;
    }

    int betAmount = rouletteCurrentBetAmount;
    if (betAmount <= 0) {
        ui->lblRouletteGameResult->setText("Invalid bet amount.");
        return;
    }

    User* u = userManager.getCurrentUser();
    if (!u || u->balance < betAmount) {
        ui->lblRouletteGameResult->setText("Not enough chips.");
        return;
    }

    // Deduct bet immediately
    userManager.addChips(-betAmount);
    refreshUserInfo();
    u->rouletteGamesPlayed++;
    userManager.saveUsers();
    ui->lblRouletteGameBalance->setText(QString::number(u->balance) + "C");

    // Build bet
    Bet bet;
    bet.type = rouletteCurrentBetType;
    bet.amount = betAmount;
    bet.number = (rouletteCurrentBetType == BetType::Number)
                     ? rouletteCurrentNumber
                     : 0;

    // Spin the wheel logically
    SpinResult spin = rouletteGame.spinWheel();
    rouletteFinalNumber = spin.number;

    // Choose a starting index for the animation
    rouletteSpinIndex = 0; // or qrand() % rouletteWheelOrder.size();

    // Store spin + bet for when animation finishes
    lastSpinResult = spin;
    lastBet = bet;

    // Start animation
    rouletteSpinSpeed = 30;
    rouletteSpinTimer->start(rouletteSpinSpeed);

    // Disable spin button during animation
    ui->btnRouletteGameSpin->setEnabled(false);
}


void MainWindow::animateRouletteSpin()
{
    // Move to next number
    rouletteSpinIndex = (rouletteSpinIndex + 1) % rouletteWheelOrder.size();
    int n = rouletteWheelOrder[rouletteSpinIndex];

    // Update number label
    ui->lblRouletteNumber->setText(QString::number(n));

    // Update color label
    if (n == 0) {
        ui->lblRouletteColor->setText("GREEN");
        ui->lblRouletteColor->setStyleSheet("color: green;");
    } else if (rouletteGame.isRedNumber(n)) {
        ui->lblRouletteColor->setText("RED");
        ui->lblRouletteColor->setStyleSheet("color: #C62828;"); // deep red
    } else {
        ui->lblRouletteColor->setText("BLACK");
        ui->lblRouletteColor->setStyleSheet("color: #333333;"); // charcoal
    }

    // Slow down animation
    rouletteSpinSpeed += 3;
    rouletteSpinTimer->setInterval(rouletteSpinSpeed);

    // Stop when slow enough AND at final number
    if (rouletteSpinSpeed > 260 && n == rouletteFinalNumber) {
        rouletteSpinTimer->stop();
        rouletteSpinSpeed = 20; // reset for next spin
        finishRouletteSpin();
    }
}
void MainWindow::finishRouletteSpin()
{
    bool won = rouletteGame.checkWin(lastBet, lastSpinResult);
    int payout = 0;

    if (won) {
        int mult = rouletteGame.getMultiplier(lastBet.type);
        payout = lastBet.amount * mult;
        userManager.addChips(payout);
        ui->lblRouletteGameResult->setText("You won: " + QString::number(payout) + "C");
        ui->lblRouletteGameResult->setStyleSheet("color: green;");
    } else {
        ui->lblRouletteGameResult->setText("You lost!");
        ui->lblRouletteGameResult->setStyleSheet("color: red;");
    }

    refreshUserInfo();
    ui->btnRouletteGameSpin->setEnabled(true);
}



void MainWindow::on_btnLeaveBlackjack_clicked()
{
    playSlideTransition("right", [this]() {
        ui->stackedWidget->setCurrentWidget(ui->MainMenu);
    });
    refreshUserInfo();
}


void MainWindow::on_btnBlackjackGameDeal_clicked()
{
    blackjackCurrentBet = ui->sbBlackjackGameBet->value();

    User* u = userManager.getCurrentUser();
    u->blackjackGamesPlayed++;
    userManager.saveUsers();

    if (!u) return;

    int bet = blackjackCurrentBet;

    // Validate bet
    if (bet <= 0 || u->balance < bet) {
        ui->lblBlackjackGameResult->setText("Not enough chips.");
        return;
    }

    // Deduct bet
    userManager.addChips(-bet);
    refreshUserInfo();

    // Start backend round
    blackjack.startRound();

    // Clear old cards
    clearLayout(ui->dealerCardLayout);
    clearLayout(ui->playerCardLayout);

    // Show player cards
    for (int v : blackjack.getPlayerHand())
        addCard(ui->playerCardLayout, v);

    // Show dealer first card + facedown card
    addCard(ui->dealerCardLayout, blackjack.getDealerHand()[0]);
    addCard(ui->dealerCardLayout, -1); // placeholder for hidden card

    // Update totals
    ui->lblBlackjackPlayerTotal->setText(QString::number(blackjack.getPlayerTotal()));
    ui->lblBlackjackDealerTotal->setText("??");

    // Reset statuses
    ui->lblBlackjackPlayerStatus->setText("");
    ui->lblBlackjackDealerStatus->setText("");
    ui->lblBlackjackGameResult->setText("");

    // Enable/disable buttons
    ui->btnBlackjackGameHit->setEnabled(true);
    ui->btnBlackjackGameStand->setEnabled(true);
    ui->btnBlackjackGameDeal->setEnabled(false);

    // Check for instant blackjack
    if (blackjack.isPlayerBlackjack()) {

        ui->lblBlackjackPlayerStatus->setText("Blackjack!");

        // Reveal dealer cards
        clearLayout(ui->dealerCardLayout);
        for (int v : blackjack.getDealerHand())
            addCard(ui->dealerCardLayout, v);

        ui->lblBlackjackDealerTotal->setText(QString::number(blackjack.getDealerTotal()));

        // Payout 1.5x
        int payout = bet * 1.5;
        userManager.addChips(payout);
        refreshUserInfo();

        ui->lblBlackjackGameResult->setText("Blackjack! You win " + QString::number(payout) + "C");

        // End round
        ui->btnBlackjackGameHit->setEnabled(false);
        ui->btnBlackjackGameStand->setEnabled(false);
        ui->btnBlackjackGameDeal->setEnabled(true);
    }
}


void MainWindow::on_btnBlackjackGameHit_clicked()
{
    // Player draws a card in backend
    blackjack.playerHit();

    // Get the new card (last card in the hand)
    int newCard = blackjack.getPlayerHand().last();

    // Add card to UI
    addCard(ui->playerCardLayout, newCard);

    // Update total
    ui->lblBlackjackPlayerTotal->setText(QString::number(blackjack.getPlayerTotal()));

    // Check for bust
    if (blackjack.isPlayerBust()) {

        ui->lblBlackjackPlayerStatus->setText("Bust!");
        ui->lblBlackjackGameResult->setText("Dealer wins.");
        ui->lblBlackjackGameResult->setStyleSheet("color: red;");

        // Reveal dealer cards
        clearLayout(ui->dealerCardLayout);
        for (int v : blackjack.getDealerHand())
            addCard(ui->dealerCardLayout, v);

        ui->lblBlackjackDealerTotal->setText(QString::number(blackjack.getDealerTotal()));

        // Disable buttons
        ui->btnBlackjackGameHit->setEnabled(false);
        ui->btnBlackjackGameStand->setEnabled(false);
        ui->btnBlackjackGameDeal->setEnabled(true);
    }

}


void MainWindow::on_btnBlackjackGameStand_clicked()
{

    // Tell backend the player is done
    blackjack.playerStand();   // this automatically calls dealerPlay()

    // Reveal dealer cards
    clearLayout(ui->dealerCardLayout);
    for (int v : blackjack.getDealerHand())
        addCard(ui->dealerCardLayout, v);

    // Update dealer total
    ui->lblBlackjackDealerTotal->setText(QString::number(blackjack.getDealerTotal()));

    int playerTotal = blackjack.getPlayerTotal();
    int dealerTotal = blackjack.getDealerTotal();

    QString result;

    // Determine outcome
    if (dealerTotal > 21) {
        result = "Dealer busts! You win.";
        ui->lblBlackjackGameResult->setStyleSheet("color: green;");
        userManager.addChips(blackjackCurrentBet * 2);
    }
    else if (playerTotal > dealerTotal) {
        result = "You win!";
        ui->lblBlackjackGameResult->setStyleSheet("color: green;");
        userManager.addChips(blackjackCurrentBet * 2);
    }
    else if (playerTotal < dealerTotal) {
        result = "Dealer wins.";
        ui->lblBlackjackGameResult->setStyleSheet("color: red;");
    }
    else {
        result = "Push (tie).";
        ui->lblBlackjackGameResult->setStyleSheet("color: orange;");
        userManager.addChips(blackjackCurrentBet); // refund
    }

    refreshUserInfo();
    ui->lblBlackjackGameResult->setText(result);

    // Disable Hit/Stand, enable Deal for next round
    ui->btnBlackjackGameHit->setEnabled(false);
    ui->btnBlackjackGameStand->setEnabled(false);
    ui->btnBlackjackGameDeal->setEnabled(true);
}


void MainWindow::on_BJQB10_clicked()
{
    blackjackCurrentBet = 10;
    ui->sbBlackjackGameBet->setValue(10);
}


void MainWindow::on_BJQB50_clicked()
{
    blackjackCurrentBet = 50;
    ui->sbBlackjackGameBet->setValue(50);
}


void MainWindow::on_BJQB100_clicked()
{
    blackjackCurrentBet = 100;
    ui->sbBlackjackGameBet->setValue(100);
}


void MainWindow::on_BJQB500_clicked()
{
    blackjackCurrentBet = 500;
    ui->sbBlackjackGameBet->setValue(500);
}

void MainWindow::clearLayout(QLayout* layout)
{
    while (QLayoutItem* item = layout->takeAt(0)) {
        if (item->widget())
            delete item->widget();
        delete item;
    }
}

void MainWindow::addCard(QHBoxLayout* layout, int value)
{
    QLabel* card = new QLabel(QString::number(value));

    if (value == -1) {
        card = new QLabel("?");
    } else {
        card = new QLabel(QString::number(value));
    }


    card->setStyleSheet(
        "font-size: 24px;"
        "padding: 10px;"
        "border: 2px solid white;"
        "border-radius: 6px;"
        "min-width: 40px;"
        "text-align: center;"
        );
    card->setAlignment(Qt::AlignCenter);
    layout->addWidget(card);
}

void MainWindow::refreshAccountPage()
{
    User* u = userManager.getCurrentUser();
    if (!u) return;

    ui->lblAccountUsernameText->setText("Username: " + u->username);
    ui->lblAccountBalanceText->setText("Balance: " + QString::number(u->balance) + "C");

    ui->lblAccountBJGamesPlayedText->setText("Blackjack Games Played: " + QString::number(u->blackjackGamesPlayed));
    ui->lblAccountRGamesPlayedText->setText("Roulette Games Played: " + QString::number(u->rouletteGamesPlayed));
    ui->lblAccountSMGamesPlayedText->setText("Slot Games Played: " + QString::number(u->slotGamesPlayed));
}

