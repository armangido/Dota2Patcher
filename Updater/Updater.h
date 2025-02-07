#include <iostream>
#include <vector>
#include <optional>
#include <compare>
#include <format>

struct WebVer {
    std::string tag_name;
    bool prerelease;
    std::string html_url;
};

struct SemVer {
    int major, minor, patch;
    std::optional<int> rc_ver;
    std::optional<std::string> update_url;

    static SemVer from_string(const std::string& version_str) {
        SemVer ver{ 0, 0, 0, std::nullopt };

        // Parse major.minor.patch
        int matched = sscanf(version_str.c_str(), "%d.%d.%d", &ver.major, &ver.minor, &ver.patch);
        if (matched < 3)
            throw std::invalid_argument("[-] (Updater) Invalid version format: " + version_str);

        // Search for -rc and parse it
        size_t rc_pos = version_str.find("-rc");
        if (rc_pos != std::string::npos) {
            if (rc_pos + 3 == version_str.size()) // -rc
                ver.rc_ver = 0;
            else if (version_str[rc_pos + 3] == '.') { // -rc.X
                int rc_num;
                if (sscanf(version_str.c_str() + rc_pos, "-rc.%d", &rc_num) == 1)
                    ver.rc_ver = rc_num;
                else
                    throw std::invalid_argument("[-] (Updater) Invalid RC format: " + version_str);
            }
            else
                throw std::invalid_argument("[-] (Updater) Invalid RC format: " + version_str);
        }
        return ver;
    }

    std::string to_string() const {
        if (rc_ver.has_value()) {
            if (rc_ver.value() == 0)
                return std::format("{}.{}.{}-rc", major, minor, patch);
            return std::format("{}.{}.{}-rc.{}", major, minor, patch, rc_ver.value());
        }
        return std::format("{}.{}.{}", major, minor, patch);
    }

    auto operator<=>(const SemVer& other) const = default;
};

class Updater {
public:
    void check_update();

private:
    const SemVer local_version = { 2, 6, 0 };

    std::optional<std::string> web_request();
    SemVer get_latest_version(const std::vector<WebVer>& versions) const;
    const std::string update_url = "https://api.github.com/repos/Wolf49406/Dota2Patcher/releases";
    const std::string download_url = "https://github.com/Wolf49406/Dota2Patcher/releases/latest";
};
