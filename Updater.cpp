#include "Dota2Patcher.h"
#include <curl/curl.h>
#include <string>

std::wstring string_to_wstring(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    return wstr;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string Updater::get_remote_version() {
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

void Updater::check_update() {
    std::string remove_version = get_remote_version();
    if (remove_version.empty())
        return;

    remove_version.erase(remove_version.find_last_not_of(" \n\r\t") + 1);

    if (remove_version == local_version)
        return;

    printf("[!!!] Update Required!\n");
    printf("[!!!] Local Version: %s\n", local_version.c_str());
    printf("[!!!] Remote Version: %s\n", remove_version.c_str());

    printf("Press Enter to continue...\n");
    std::cin.get();
    std::wstring w_update_url = string_to_wstring(update_url);

    if (ShellExecuteW(0, L"open", w_update_url.c_str(), 0, 0, SW_SHOWNORMAL) <= (HINSTANCE)32) {
        printf("[!!!] Failed to open the browser. Please visit %s manually.\n", update_url.c_str());
    }
}
