#include "user.h"

QJsonObject User::toJson() const
{
    QJsonObject obj;
    obj["username"] = username;
    obj["hashedPassword"] = hashedPassword;
    obj["salt"] = salt;
    obj["balance"] = balance;
    obj["totalSpent"] = totalSpent;
    obj["isAdmin"] = isAdmin;
    obj["blocked"] = blocked;
    obj["blackjackGamesPlayed"] = blackjackGamesPlayed;
    obj["rouletteGamesPlayed"] = rouletteGamesPlayed;
    obj["slotGamesPlayed"] = slotGamesPlayed;
    return obj;
}

User User::fromJson(const QJsonObject &obj)
{
    User u;
    u.username = obj["username"].toString();
    u.hashedPassword = obj["hashedPassword"].toString();
    u.salt = obj["salt"].toString();
    u.balance = obj["balance"].toDouble();
    u.totalSpent = obj["totalSpent"].toDouble();
    u.isAdmin = obj["isAdmin"].toBool();
    u.blocked = obj["blocked"].toBool();
    u.blackjackGamesPlayed = obj["blackjackGamesPlayed"].toInt();
    u.rouletteGamesPlayed = obj["rouletteGamesPlayed"].toInt();
    u.slotGamesPlayed = obj["slotGamesPlayed"].toInt();
    return u;
}
