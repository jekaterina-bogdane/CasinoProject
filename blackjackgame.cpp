#include "blackjackgame.h"
#include <cstdlib>
#include <ctime>

BlackjackGame::BlackjackGame()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    state = BlackjackState::NotStarted;
}

void BlackjackGame::fillDeck()
{
    int index = 0;

    for (int value = 2; value <= 9; ++value) {
        for (int i = 0; i < 4; ++i) {
            deck[index++] = value;
        }
    }

    for (int i = 0; i < 16; ++i) {
        deck[index++] = 10;
    }

    for (int i = 0; i < 4; ++i) {
        deck[index++] = 11;
    }

    deckIndex = 0;
}

void BlackjackGame::shuffleDeck()
{
    for (int i = 0; i < 52; ++i) {
        int r = std::rand() % 52;
        int tmp = deck[i];
        deck[i] = deck[r];
        deck[r] = tmp;
    }
}

int BlackjackGame::drawCard()
{
    if (deckIndex >= 52) {
        fillDeck();
        shuffleDeck();
        deckIndex = 0;
    }
    return deck[deckIndex++];
}

int BlackjackGame::calculateTotal(const QVector<int>& hand) const
{
    int total = 0;
    int aceCount = 0;

    for (int v : hand) {
        total += v;
        if (v == 11) aceCount++;
    }

    while (total > 21 && aceCount > 0) {
        total -= 10;
        aceCount--;
    }

    return total;
}

void BlackjackGame::startRound()
{
    fillDeck();
    shuffleDeck();

    playerHand.clear();
    dealerHand.clear();

    playerHand.append(drawCard());
    dealerHand.append(drawCard());
    playerHand.append(drawCard());
    dealerHand.append(drawCard());

    state = BlackjackState::PlayerTurn;
}

void BlackjackGame::playerHit()
{
    if (state != BlackjackState::PlayerTurn) return;

    playerHand.append(drawCard());

    if (isPlayerBust()) {
        state = BlackjackState::RoundOver;
    }
}

void BlackjackGame::playerStand()
{
    if (state != BlackjackState::PlayerTurn) return;

    state = BlackjackState::DealerTurn;
    dealerPlay();
}

void BlackjackGame::dealerPlay()
{
    if (state != BlackjackState::DealerTurn) return;

    while (getDealerTotal() < 17) {
        dealerHand.append(drawCard());
    }

    state = BlackjackState::RoundOver;
}

int BlackjackGame::getPlayerTotal() const
{
    return calculateTotal(playerHand);
}

int BlackjackGame::getDealerTotal() const
{
    return calculateTotal(dealerHand);
}

const QVector<int>& BlackjackGame::getPlayerHand() const
{
    return playerHand;
}

const QVector<int>& BlackjackGame::getDealerHand() const
{
    return dealerHand;
}

bool BlackjackGame::isPlayerBust() const
{
    return getPlayerTotal() > 21;
}

bool BlackjackGame::isDealerBust() const
{
    return getDealerTotal() > 21;
}

bool BlackjackGame::isPlayerBlackjack() const
{
    return (playerHand.size() == 2 && getPlayerTotal() == 21);
}

BlackjackState BlackjackGame::getState() const
{
    return state;
}
