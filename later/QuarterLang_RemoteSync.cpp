#include <iostream>
#include <fstream>
#include <sstream>
#include <curl/curl.h> // Requires libcurl

class CloudSync {
public:
    static std::string fetchFile(const std::string& url) {
        CURL* curl = curl_easy_init();
        std::stringstream out;

        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
            CURLcode res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);

            if (res != CURLE_OK) {
                std::cerr << "❌ Failed to fetch: " << url << "\n";
                return "";
            }
        }

        return out.str();
    }

    static bool syncProject(const std::string& qtrprojUrl, const std::string& localDir) {
        std::string proj = fetchFile(qtrprojUrl);
        if (proj.empty()) return false;

        std::filesystem::create_directory(localDir);
        std::ofstream(localDir + "/project.qtrproj") << proj;

        std::istringstream iss(proj);
        std::string line;
        while (std::getline(iss, line)) {
            std::string fileUrl = qtrprojUrl.substr(0, qtrprojUrl.find_last_of("/") + 1) + line;
            std::string content = fetchFile(fileUrl);
            if (content.empty()) continue;
            std::ofstream(localDir + "/" + line) << content;
        }

        std::cout << "📡 Synced project to: " << localDir << "\n";
        return true;
    }

private:
    static size_t writeCallback(void* ptr, size_t size, size_t nmemb, void* stream) {
        ((std::stringstream*)stream)->write((char*)ptr, size * nmemb);
        return size * nmemb;
    }
};
