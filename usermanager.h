#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>
#include <QVector>
#include "user.h"

enum class RegisterResult {
    Success,
    UsernameExists,
    InvalidInput,
    AdminAlreadyExists
};

enum class LoginResult {
    Success,
    UserNotFound,
    WrongPassword,
    Blocked,
    TooManyAttempts
};


class UserManager
{
public:
    UserManager();

    void initializeStorage();
    bool loadUsers();
    bool saveUsers();
    int loginAttempts = 0;

    void addChips(int amount);

    // Admin creation
    void createDefaultAdmin();

    RegisterResult registerUser(const QString &username, const QString &password, bool admin = false);
    LoginResult login(const QString &username, const QString &password);

    //admin functions
    User* findUser(const QString& username);
    bool deleteUser(const QString& username);

    // Accessors
    const QVector<User>& getUsers() const { return users; }
    User* getCurrentUser() const { return currentUser; }
    void logout() { currentUser = nullptr; }

private:
    QVector<User> users;
    User* currentUser = nullptr;


    QString appDataPath;
    QString usersFilePath;


    QString generateSalt();
    QString hashPassword(const QString &password, const QString &salt);

    // Helpers
    void ensureAppDataFolder();
    void ensureUsersFile();

};

#endif // USERMANAGER_H
