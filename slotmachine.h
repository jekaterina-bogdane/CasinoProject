#ifndef SLOTMACHINE_H
#define SLOTMACHINE_H

#include <QStringList>

class SlotMachine
{
public:
    SlotMachine();

    QStringList spin();
    const QStringList& getSymbols() const;

    int calculatePayout(const QStringList& reels, int bet);



private:

    QStringList symbols;


};

#endif
