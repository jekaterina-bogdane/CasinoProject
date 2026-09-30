# Casino Project

A desktop casino game built with **C++17** and **Qt 6 (Widgets)**.

## Features

- **Games:** Blackjack, Roulette, Slot Machine
- **User accounts:** registration and login, passwords stored as salted SHA-256 hashes
- **Two-factor authentication** (2FA code dialog)
- **Balance and store:** in-game credits (C)
- **Admin panel:** manage users (view, block, adjust balances)
- User data is saved to `users.json` in the system AppData folder

## Build and run

1. Install **Qt 6** with Qt Creator.
2. Open `CasinoProject.pro` in Qt Creator.
3. Choose a kit (e.g. *Desktop Qt 6 MinGW 64-bit*) → **Build** → **Run**.

A ready-to-play Windows build is available on the **Releases** page.

## Default admin account

```
Username: admin
Password: admin123
```

Created automatically on first launch when no `users.json` exists.

## Project structure

| File | Purpose |
|---|---|
| `main.cpp`, `mainwindow.*` | Main window, login/register, navigation |
| `blackjackgame.*` | Blackjack logic |
| `roulettegame.*` | Roulette logic |
| `slotmachine.*` | Slot machine logic |
| `user.*`, `usermanager.*` | User model, JSON storage, hashing |
| `dialog2fa.*` | Two-factor authentication dialog |
| `adminwindow.*` | Admin panel |
| `clickableframe.*` | Custom clickable UI frame |
| `resources.qrc`, `*.png/jpg` | Images and icons |

## Author

**Jekaterina Bogdane** — Transport and Telecommunication Institute, 2026
