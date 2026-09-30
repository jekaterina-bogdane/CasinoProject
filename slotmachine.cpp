#include "slotmachine.h"
#include <QRandomGenerator>

SlotMachine::SlotMachine()
{
    symbols = {"A", "K", "Q", "J", "7", "BAR"};
}

const QStringList& SlotMachine::getSymbols() const
{
    return symbols;
}

QStringList SlotMachine::spin()
{
    QStringList reels;

    for (int i = 0; i < 3; i++) {
        int index = QRandomGenerator::global()->bounded(symbols.size());
        reels.append(symbols[index]);
    }

    return reels;
}

int SlotMachine::calculatePayout(const QStringList& reels, int bet)
{
    // JACKPOT: BAR BAR BAR
    if (reels[0] == "BAR" && reels[1] == "BAR" && reels[2] == "BAR") {
        return bet * 20;
    }

    // 3 of a kind
    if (reels[0] == reels[1] && reels[1] == reels[2]) {
        return bet * 10;
    }

    // 2 of a kind
    if (reels[0] == reels[1] ||
        reels[1] == reels[2] ||
        reels[0] == reels[2]) {
        return bet * 2;
    }

    // if there is no match
    return 0;
}
