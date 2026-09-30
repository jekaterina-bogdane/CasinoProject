#include "usermanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QCryptographicHash>
#include <QRandomGenerator>

UserManager::UserManager()
{
    // Resolve path
    appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    usersFilePath = appDataPath + "/users.json";
}

void UserManager::initializeStorage()
{
    ensureAppDataFolder();
    ensureUsersFile();

    if (!loadUsers()) {
        qDebug() << "Failed to load users.json, creating default admin.";
        createDefaultAdmin();
        saveUsers();
    }

    // If file exists but contains no users, create admin account
    if (users.isEmpty()) {
        qDebug() << "No users found. Creating default admin.";
        createDefaultAdmin();
        saveUsers();
    }
}

void UserManager::ensureAppDataFolder()
{
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

void UserManager::ensureUsersFile()
{
    QFile file(usersFilePath);

    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly)) {
            // Write empty JSON structure
            QJsonObject root;
            root["users"] = QJsonArray();

            QJsonDocument doc(root);
            file.write(doc.toJson());
            file.close();
        }
    }
}

bool UserManager::loadUsers()
{
    QFile file(usersFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return false;

    QJsonObject root = doc.object();
    QJsonArray arr = root["users"].toArray();

    users.clear();
    for (const QJsonValue &val : arr) {
        users.append(User::fromJson(val.toObject()));
    }

    return true;
}

bool UserManager::saveUsers()
{
    QJsonArray arr;
    for (const User &u : users) {
        arr.append(u.toJson());
    }

    QJsonObject root;
    root["users"] = arr;

    QFile file(usersFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();
    return true;
}

QString UserManager::generateSalt()
{
    const QString chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    QString salt;
    for (int i = 0; i < 12; i++) {
        int index = QRandomGenerator::global()->bounded(chars.length());
        salt.append(chars[index]);
    }
    return salt;
}

QString UserManager::hashPassword(const QString &password, const QString &salt)
{
    QByteArray data = (password + salt).toUtf8();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    return hash.toHex();
}

void UserManager::createDefaultAdmin()
{
    User admin;
    admin.username = "admin";
    admin.salt = generateSalt();
    admin.hashedPassword = hashPassword("admin123", admin.salt);
    admin.balance = 999999999;
    admin.totalSpent = 0;
    admin.isAdmin = true;
    admin.blocked = false;

    users.append(admin);
}
//----------REGISTERATION------------------------------------------------------------------------------
RegisterResult UserManager::registerUser(const QString &username, const QString &password, bool admin)
{
    // Basic validation
    if (username.trimmed().isEmpty() || password.isEmpty()) {
        return RegisterResult::InvalidInput;
    }

    // Check if username already exists
    for (const User &u : users) {
        if (u.username == username) {
            return RegisterResult::UsernameExists;
        }
    }

    // Only one admin allowed
    if (admin) {
        for (const User &u : users) {
            if (u.isAdmin) {
                return RegisterResult::AdminAlreadyExists;
            }
        }
    }

    // Create new user
    User newUser;
    newUser.username = username;

    newUser.salt = generateSalt();
    newUser.hashedPassword = hashPassword(password, newUser.salt);

    newUser.balance = 100.0;
    newUser.totalSpent = 0.0;
    newUser.isAdmin = admin;
    newUser.blocked = false;

    // Add to list and save
    users.append(newUser);
    saveUsers();

    return RegisterResult::Success;
}
//-----------------------------------------------------------------------------------------------------

//----------LOGIN--------------------------------------------------------------------------------------
LoginResult UserManager::login(const QString &username, const QString &password)
{
    // Find user
    User* user = nullptr;
    for (User &u : users) {
        if (u.username == username) {
            user = &u;
            break;
        }
    }

    if (!user) {
        return LoginResult::UserNotFound;
    }

    if (user->blocked) {
        return LoginResult::Blocked;
    }

    // Hash input password with stored salt
    QString hashedInput = hashPassword(password, user->salt);

    if (hashedInput != user->hashedPassword) {
        loginAttempts++;

        if (loginAttempts >= 3) {
            return LoginResult::TooManyAttempts;
        }

        return LoginResult::WrongPassword;
    }

    // Success
    loginAttempts = 0;
    currentUser = user;
    return LoginResult::Success;
}
//-----------------------------------------------------------------------------------------------------

void UserManager::addChips(int amount)
{
    if (!currentUser)
        return;

    currentUser->balance += amount;

    //only add amount added if it is positive(total spent)
    if(amount>0){
        currentUser->totalSpent += amount;
    }

    saveUsers();
}

// ADMIN FUNCTIONS
User* UserManager::findUser(const QString& username)
{
    for (User& u : users) {
        if (u.username == username)
            return &u;
    }
    return nullptr;
}

bool UserManager::deleteUser(const QString& username)
{
    for (int i = 0; i < users.size(); i++) {
        if (users[i].username == username) {
            users.removeAt(i);
            saveUsers();
            return true;
        }
    }
    return false;
}

