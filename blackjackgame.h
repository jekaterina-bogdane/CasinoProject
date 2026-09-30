#ifndef BLACKJACKGAME_H
#define BLACKJACKGAME_H

#include <QVector>

enum class BlackjackState {
    NotStarted,
    PlayerTurn,
    DealerTurn,
    RoundOver
};

class BlackjackGame
{
public:
    BlackjackGame();

    void startRound();  // new deck, deal 2+2
    void playerHit();   // player draws one
    void playerStand(); // switch to dealer turn
    void dealerPlay();  // dealer draws to 17+

    int getPlayerTotal() const;
    int getDealerTotal() const;

    const QVector<int>& getPlayerHand() const;
    const QVector<int>& getDealerHand() const;

    bool isPlayerBust() const;
    bool isDealerBust() const;
    bool isPlayerBlackjack() const;

    BlackjackState getState() const;

private:
    int deck[52];
    int deckIndex;

    QVector<int> playerHand;
    QVector<int> dealerHand;

    BlackjackState state;

    void fillDeck();
    void shuffleDeck();
    int drawCard();
    int calculateTotal(const QVector<int>& hand) const;
};

#endif // BLACKJACKGAME_H
