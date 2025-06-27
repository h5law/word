# word

> **NOTE** This project is still pre-alpha and very much a WIP

## Password Store

At its core `word` is a password store written using C and a simple ncurses based
UI if desired or can be interacted with through a simple CLI.

`word` stores passwords in a key-value-like database. Where each key in this
case is what the storage will be linked to. For example a specific website).
Then those keys store the relevant data assigned to them:

  - Usernames
  - Passwords
  - Emails
  - (2FA codes)[2FA]
