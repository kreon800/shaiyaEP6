# Shaiya Episode 6 - Server

A library that modifies ps_game and ps_dbAgent to make them compatible with episode 6 clients.

## Prerequisites

[Microsoft Visual C++ Redistributable](https://aka.ms/vs/17/release/vc_redist.x86.exe)

## Getting Started

1. Navigate to `sdev-db/bin` and read the documentation. Copy the binaries in the `bin` directory of each project to `SERVER/PSM_Client/Bin`. You can use your own binaries, but since I cannot account for what has been done to the files, support will be limited.

2. Open the project in Visual Studio, target the x86 platform, and build the solution. Copy the libraries to `SERVER\PSM_Client\Bin` and start the services. Use the provided `.ct` files to inject the libraries.

3. Pay attention to what you inject, as it may overwrite addresses targeted by the library. Compare every address in the cheat table to the addresses in the library before injecting the script.

Note: injecting `sdev.dll` before ps_session packet `0x105` arrives will result in `g_nPayLetterEnable` being set to `false`, which will disable presents. Please follow the instructions or set `g_nPayLetterEnable` to `true` with a Cheat Engine script.

```
[ENABLE]
0058799C:
dd 01

[DISABLE]
0058799C:
dd 00
```

To use this library with episode 5 clients, comment the episode 6 macros. `sdev-db` is not required for `client/0x12092000` or episode 5 clients.

```cpp
// sdev/include/shaiya/common.h
//#define SHAIYA_EP6
//#define SHAIYA_EP6_3
//#define SHAIYA_EP6_3_PT
//#define SHAIYA_EP6_4_PT
```

For episode 6.3 clients, expose the `SHAIYA_EP6_3` macro or the `SHAIYA_EP6_3_PT` macro. See `client/0x13120600` for more information.

```cpp
// sdev/include/shaiya/common.h
//#define SHAIYA_EP6
//#define SHAIYA_EP6_3
#define SHAIYA_EP6_3_PT
//#define SHAIYA_EP6_4_PT
```

For episode 6.4 clients, expose the `SHAIYA_EP6_4_PT` macro.

```cpp
// sdev/include/shaiya/common.h
//#define SHAIYA_EP6
//#define SHAIYA_EP6_3
//#define SHAIYA_EP6_3_PT
#define SHAIYA_EP6_4_PT
```

## Contributors

The project goal has been reached, so help is not wanted anymore. Pull requests are still welcome.
