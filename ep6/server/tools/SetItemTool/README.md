# SetItemTool

## Environment

Windows 10

Visual Studio 2022

C++ 20

.NET Framework 4.8

## Description

This application will extract `SetItem.SData` to `JSON` format. The output file will be easy to read and edit.

```json
"synergies": [
  {
    "strength": 0,
    "dexterity": 0,
    "intelligence": 0,
    "wisdom": 0,
    "reaction": 0,
    "luck": 0,
    "health": 0,
    "mana": 0,
    "stamina": 0,
    "meleeAttackPower": 0,
    "rangeAttackPower": 0,
    "magicAttackPower": 0
  },
```

When you are finished, compile the file using the **decrypted** option. Then, copy `SetItem.SData` to `SERVER/PSM_Client/Bin/Data`. Do not use the `Parsec` build included with this application to compile the client-side file.
