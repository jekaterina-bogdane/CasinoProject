#include "roulettegame.h"
#include <cstdlib>

SpinResult RouletteGame::spinWheel() const
{
    SpinResult spin;
    spin.number = rand() % 37;
    spin.isRed = isRedNumber(spin.number);
    spin.isBlack = isBlackNumber(spin.number);
    return spin;
}

bool RouletteGame::isRedNumber(int n) const
{
    switch (n) {
    case 1: case 3: case 5: case 7: case 9:
    case 12: case 14: case 16: case 18:
    case 19: case 21: case 23: case 25: case 27:
    case 30: case 32: case 34: case 36:
        return true;
    default:
        return false;
    }
}

bool RouletteGame::isBlackNumber(int n) const
{
    return n != 0 && !isRedNumber(n);
}

int RouletteGame::getColumn(int n) const
{
    if (n == 0) return 0;
    if (n % 3 == 1) return 1;
    if (n % 3 == 2) return 2;
    return 3;
}

bool RouletteGame::checkWin(const Bet& bet, const SpinResult& spin) const
{
    switch (bet.type) {
    case BetType::Number:
        return bet.number == spin.number;

    case BetType::Red:
        return spin.isRed;

    case BetType::Black:
        return spin.isBlack;

    case BetType::Even:
        return spin.number != 0 && spin.number % 2 == 0;

    case BetType::Odd:
        return spin.number != 0 && spin.number % 2 == 1;

    case BetType::Low:
        return spin.number >= 1 && spin.number <= 18;

    case BetType::High:
        return spin.number >= 19 && spin.number <= 36;

    case BetType::Dozen1:
        return spin.number >= 1 && spin.number <= 12;

    case BetType::Dozen2:
        return spin.number >= 13 && spin.number <= 24;

    case BetType::Dozen3:
        return spin.number >= 25 && spin.number <= 36;

    case BetType::Column1:
        return getColumn(spin.number) == 1;

    case BetType::Column2:
        return getColumn(spin.number) == 2;

    case BetType::Column3:
        return getColumn(spin.number) == 3;
    }
    return false;
}

int RouletteGame::getMultiplier(BetType type) const
{
    switch (type) {
    case BetType::Number:
        return 35;

    case BetType::Red:
    case BetType::Black:
    case BetType::Even:
    case BetType::Odd:
    case BetType::Low:
    case BetType::High:
        return 1;

    case BetType::Dozen1:
    case BetType::Dozen2:
    case BetType::Dozen3:
    case BetType::Column1:
    case BetType::Column2:
    case BetType::Column3:
        return 2;
    }
    return 0;
}
