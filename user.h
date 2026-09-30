#ifndef USER_H
#define USER_H

#include <QString>
#include <QJsonObject>

class User
{
public:
    User() = default;

    QString username;
    QString hashedPassword;
    QString salt;
    double balance = 0.0;
    double totalSpent = 0.0;
    bool isAdmin = false;
    bool blocked = false;
    int blackjackGamesPlayed = 0;
    int rouletteGamesPlayed = 0;
    int slotGamesPlayed = 0;

    // JSON conversion
    QJsonObject toJson() const;
    static User fromJson(const QJsonObject &obj);
};

#endif // USER_H
