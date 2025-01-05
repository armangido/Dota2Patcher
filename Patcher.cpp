#include "Dota2Patcher.h"
#include <fstream>
#include <sstream>

std::vector<BYTE> parse_pattern(const std::string& pattern) {
    std::vector<BYTE> parsed_pattern;
    std::stringstream ss(pattern);
    std::string byte_str;
    while (ss >> byte_str) {
        if (byte_str == "?") {
            parsed_pattern.push_back(0xFF);
        }
        else {
            parsed_pattern.push_back(static_cast<BYTE>(std::stoi(byte_str, nullptr, 16)));
        }
    }
    return parsed_pattern;
}

int find_offset_internal(char* array, int array_length, const std::string& pattern) {
    std::vector<BYTE> parsed_pattern = parse_pattern(pattern);

    for (int haystack_index = 0; haystack_index < array_length; haystack_index++) {
        bool needle_found = true;
        for (size_t needle_index = 0; needle_index < parsed_pattern.size(); needle_index++) {
            unsigned char haystack_character = static_cast<unsigned char>(array[haystack_index + needle_index]);
            BYTE needle_character = parsed_pattern[needle_index];

            if (needle_character == 0xFF)
                continue;

            if (haystack_character != needle_character) {
                needle_found = false;
                break;
            }
        }

        if (needle_found) {
            return haystack_index;
        }
    }

    return -1;
}

bool get_byte_array(const std::string& file_path, char** ret_array, int* file_size) {
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    if (!file)
        return false;

    int file_length = static_cast<int>(file.tellg());
    if (file_length <= 0)
        return false;

    char* array = new char[file_length];
    file.rdbuf()->pubseekoff(0, std::ios_base::beg);
    file.read(array, file_length);
    file.close();

    *file_size = file_length;
    *ret_array = array;

    return true;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

int Patcher::find_offset(const std::string& file_path, const std::string& pattern) {
    char* array = nullptr;
    int file_size = 0;

    if (!get_byte_array(file_path, &array, &file_size))
        return 0;

    int patch_offset = find_offset_internal(array, file_size, pattern);

    delete[] array;

    return patch_offset;
}

void Patcher::apply_patch(const std::string& file_path, int patch_offset, const std::string& replace_str) {
    std::vector<BYTE> replace_bytes = parse_pattern(replace_str);

    FILE* pFile;
    fopen_s(&pFile, file_path.c_str(), "r+b");

    if (pFile == nullptr) {
        printf("[-] Failed to open \"%s\" for patching", file_path.c_str());
        return;
    }

    for (size_t i = 0; i < replace_bytes.size(); i++) {
        fseek(pFile, patch_offset + static_cast<int>(i), SEEK_SET);
        fputc(replace_bytes[i], pFile);
    }

    fclose(pFile);
}
