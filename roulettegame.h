#ifndef ROULETTEGAME_H
#define ROULETTEGAME_H

enum class BetType {
    Number,
    Red,
    Black,
    Even,
    Odd,
    Low,
    High,
    Dozen1,
    Dozen2,
    Dozen3,
    Column1,
    Column2,
    Column3
};

struct Bet {
    BetType type;
    int amount;
    int number; // Single Number bet
};

struct SpinResult {
    int number;
    bool isRed;
    bool isBlack;
};

class RouletteGame
{
public:
    RouletteGame() = default;

    SpinResult spinWheel() const;

    bool checkWin(const Bet& bet, const SpinResult& spin) const;
    int getMultiplier(BetType type) const;

    bool isRedNumber(int n) const;
    bool isBlackNumber(int n) const;

private:

    int getColumn(int n) const;
};

#endif // ROULETTEGAME_H
