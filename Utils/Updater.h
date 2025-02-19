#include <compare>
#include <format>

struct WebVer {
    string tag_name;
    bool prerelease;
    string html_url;
};

struct SemVer {
    int major, minor, patch;
    optional<int> rc_ver;
    optional<string> update_url;

    static SemVer from_string(const string& version_str) {
        SemVer ver{ 0, 0, 0, nullopt };

        int matched = sscanf(version_str.c_str(), "%d.%d.%d", &ver.major, &ver.minor, &ver.patch);
        if (matched < 3)
            throw std::invalid_argument("[-] (Updater) Invalid version format: " + version_str);

        size_t rc_pos = version_str.find("-rc");
        if (rc_pos != string::npos) {
            if (rc_pos + 3 == version_str.size())
                ver.rc_ver = 0;
            else if (version_str[rc_pos + 3] == '.') {
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

    string to_string() const {
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
    static bool update_required();

private:
    static constexpr SemVer local_version = { 2, 8, 5 };

    static optional<string> web_request();
    static SemVer get_latest_version(const std::vector<WebVer>& versions);
    static constexpr std::string_view update_url = "https://api.github.com/repos/Wolf49406/Dota2Patcher/releases";
    static constexpr std::string_view download_url = "https://github.com/Wolf49406/Dota2Patcher/releases/latest";
};
