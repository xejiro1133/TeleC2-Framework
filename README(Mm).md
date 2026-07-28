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

 Pre-compiled Binaries (Code တွေ Compile မလုပ်ချင်သူများအတွက်)
C++ Compiler တပ်ဆင်ပြီး Code တွေ Compile တိုက်ရိုက်ထုတ်ဖို့ အခက်အခဲရှိသူများအတွက် အသင့်သုံးနိုင်သော Pre-compiled binaries များကို သုံးလိုပါက -
    GitHub Repository ရှိ Releases section မှတစ်ဆင့် Latest Release ZIP ဖိုင်ကို Download ဆွဲပါ။
    ဖိုင်ကို ဖြည်လိုက်ပါ (Extract)။
    ပါရှိသော builder.exe ကို ဖွင့်ပြီး သင်၏ Telegram Bot Token နှင့် Chat ID ကို ထည့်သွင်းကာ အသင့်သုံး Executable ကို ထုတ်ယူပါ။
 Features & Commands
    /screenshot - Capture and send screen
    /systeminfo - Get system specifications
    /wifipassword - Extract saved Wi-Fi profiles & keys
    /shutdown - Remotely shutdown the target system
    Cmd/<command> - Run remote terminal commands
    fupload/<path> / pupload/<path> - Upload files or images to Telegram

##  Prerequisites & Dependencies (ဘာတွေလိုလဲ၊ ဘယ်လို Down ရမလဲ)
ဒီ Project ကို Compile လုပ်ဖို့အတွက် အောက်ပါ Tools တွေနဲ့ Libraries တွေကို Windows ပေါ်မှာ အရင် Download ဆွဲထားရပါမည်။
1. **C++ Compiler (MinGW-w64):**
   * Windows ပေါ်မှာ C++ code တွေ Compile လုပ်ဖို့ MinGW-w64 ကို ထည့်သွင်းထားပါ။
2. **libcurl Development Library:**
   * Telegram API နဲ့ HTTP Requests တွေ ချိတ်ဆက်ဖို့ libcurl လိုအပ်ပါတယ်။ (WinGet ဖြင့် တပ်ဆင်နိုင်သည်)
3. **nlohmann/json Header (`json.hpp`):**
   * JSON Parse လုပ်ဖို့အတွက် [nlohmann/json GitHub Repository](https://github.com/nlohmann/json) ကနေ `json.hpp` ဖိုင်ကို Download ဆွဲပြီး သင့် Source Code Folder ထဲမှာ ထည့်ထားပါ။
---
## Step 1: Telegram Bot & Chat ID Setup (ဘိုတီ တည်ဆောက်ခြင်း)
1. **Telegram Bot ဖန်တီးပုံ (Bot Token ယူရန်):**
   * Telegram မှာ **`@BotFather`** ကို ရှာပြီး စတင်ပါ။
   * `/newbot` Command ပေး၍ Bot ရဲ့ နာမည်နဲ့ Username ကို ဖန်တီးပါ။
   * BotFather ပေးလိုက်တဲ့ **API Token** ကို Copy ကူးထားပါ။
2. **Telegram Chat ID ယူပုံ (Authorized User အတွက်):**
   * Telegram မှာ **`@userinfobot`** (သို့မဟုတ် `@RawDataBot`) ဆီကို Message တစ်စောင် ပို့ပါ။
   * ပြန်လာတဲ့ အချက်အလက်ထဲက ကိုယ့်ရဲ့ **`id`** နံပါတ်ကို Copy ကူးထားပါ။
---

## Step 2: Custom Icon ထည့်သွင်းခြင်း (Optional)
Stub Executable မှာ ကိုယ်ပိုင် Custom Icon ထည့်ချင်ပါက -
1. ကြိုက်နှစ်သက်ရာ ပုံဖိုင် (`.png`) တစ်ခုကို [convertico.com](https://convertico.com/) ကဲ့သို့ Online Converter ဖြင့် `.ico` format သို့ ပြောင်းပါ။
2. Project Folder ထဲတွင် **`resource.rc`** ဆိုတဲ့ ဖိုင်လေးဆောက်ပြီး အောက်ပါအတိုင်း ရေးပါ -
   MAINICON ICON "app_icon.ico"
MinGW ဖြင့် Compile လုပ်သည့်အခါတွင် windres ကို အသုံးပြုပါ -
	windres resource.rc -O coff -o resource.o

 Step 3: Compilation & Building
1. Compiling the Stub (Agent) with Static Linking & No Console
အကယ်၍ သင်သည် cURL ကို WinGet ဖြင့် တပ်ဆင်ထားပြီး MinGW ဖြင့် Static Compile ထုတ်လိုပါက အောက်ပါ Command ကို အသုံးပြုနိုင်ပါသည်။ (သင်၏ Windows Username နှင့် cURL Version အပေါ်မူတည်၍ Path များကို ကိုယ်တိုင် ပြင်ဆင်ပေးရပါမည်):
g++ stub1.cpp resource.o -o stub.exe \-I"C:/Users/YOUR_USERNAME/AppData/Local/Microsoft/WinGet/Packages/cURL.cURL_Microsoft.Winget.Source_8wekyb3d8bbwe/curl-YOUR_VERSION-win64-mingw/include" \ -L"C:/Users/YOUR_USERNAME/AppData/Local/Microsoft/WinGet/Packages/cURL.cURL_Microsoft.Winget.Source_8wekyb3d8bbwe/curl-YOUR_VERSION-win64-mingw/lib" \ -lcurl -lwininet -lgdi32 -lgdiplus -lole32 -static-libgcc -static-libstdc++ -lshfolder -mwindows
 အချက်အလက် ရှင်းလင်းချက်:
    YOUR_USERNAME နေရာတွင် သင့် Windows Account နာမည်ကို ထည့်ပါ။
    YOUR_VERSION နေရာတွင် သင့်စက်ထဲရှိ cURL Folder နာမည် (ဥပမာ- curl-8.21.0_4-win64-mingw) ကို ထည့်ပါ။
    -mwindows Flag ပါဝင်ခြင်းကြောင့် နောက်ခံတွင် CMD/Console ဝင်းဒိုး ပေါ်လာမည် မဟုတ်ပါ။
    -static-libgcc -static-libstdc++ ပါဝင်သဖြင့် C++ DLL Errors မရှိဘဲ မည်သည့် Windows စက်ပေါ်တွင်မဆို သီးသန့် Run နိုင်မည် ဖြစ်သည်။

2. Compiling and Running the Builder
Builder (builder.cpp) ကို Compile လုပ်ပြီး Token နဲ့ Chat ID ထည့်သွင်းကာ Final Executable ထုတ်ပါ -
	g++ builder.cpp -o builder.exe
./builder.exe

