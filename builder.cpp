#include <iostream>
#include <fstream>
#include <string>

const std::string CONFIG_MARKER = "---CONFIG_START---";

int main() {
    std::string token, chat_id, output_name;

    std::cout << "========================================" << std::endl;
    std::cout << "  Telegram Bot Dynamic EXE Builder      " << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "[?] Enter Bot Token: ";
    std::getline(std::cin, token);

    std::cout << "[?] Enter Chat ID: ";
    std::getline(std::cin, chat_id);

    std::cout << "[?] Enter Output EXE Name (e.g., bot1): ";
    std::getline(std::cin, output_name);

    if (output_name.find(".exe") == std::string::npos) {
        output_name += ".exe";
    }

    // 1. Read stub.exe
    std::ifstream stubFile("stub.exe", std::ios::binary);
    if (!stubFile) {
        std::cout << "\n[-] Error: 'stub.exe' not found in the current folder!" << std::endl;
        system("pause");
        return 1;
    }

    // 2. Create the output EXE copy
    std::ofstream outFile(output_name, std::ios::binary);
    outFile << stubFile.rdbuf(); 
    stubFile.close();

    // 3. Append the Token and Chat ID to the EOF
    std::string configPayload = CONFIG_MARKER + token + "|" + chat_id;
    outFile.write(configPayload.c_str(), configPayload.length());
    outFile.close();

    std::cout << "\n[+] Success! Generated: " << output_name << std::endl;
    system("pause");
    return 0;
}