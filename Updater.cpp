#include "Dota2Patcher.h"
#include <curl/curl.h>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "md5.h"

static std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return std::tolower(c);
        });
    return s;
}

static std::wstring string_to_wstring(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    return wstr;
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string Updater::get_remote_hash() {
    CURL* curl;
    CURLcode res;
    std::string read_buffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, remote_version_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (res != CURLE_OK) {
            printf("[!] Can't check update. Error: %s", curl_easy_strerror(res));
            return "";
        }
    }

    return read_buffer;
}

std::string Updater::get_local_hash() {
    if (!std::filesystem::exists("Dota2Patcher.exe"))
        return "";
    std::ifstream inBigArrayfile;
    inBigArrayfile.open("Dota2Patcher.exe", std::ios::binary | std::ios::in);
    inBigArrayfile.seekg(0, std::ios::end);
    long Length = (long)inBigArrayfile.tellg();
    inBigArrayfile.seekg(0, std::ios::beg);
    char* InFileData = new char[Length];
    inBigArrayfile.read(InFileData, Length);
    std::string HASH = md5(InFileData, Length);
    delete[] InFileData;
    return HASH;
}


void Updater::check_update() {
    std::string remote_hash = get_remote_hash();
    std::string local_hash = get_local_hash();

    if (remote_hash.empty() || local_hash.empty())
        return;

    remote_hash.erase(remote_hash.find_last_not_of(" \n\r\t") + 1);

    if (ToLower(remote_hash) == ToLower(local_hash))
        return;

    printf("[!] Update Required!\n");
    printf("[~] Press Enter to continue...\n");
    std::cin.get();

    std::wstring w_update_url = string_to_wstring(update_url);

    if (ShellExecuteW(0, L"open", w_update_url.c_str(), 0, 0, SW_SHOWNORMAL) <= (HINSTANCE)32) {
        printf("[-] Failed to open the browser. Please visit %s.\n", update_url.c_str());
    }
}
