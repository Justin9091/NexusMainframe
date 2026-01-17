//
// Created by jusra on 17-1-2026.
//
#include "commands/DownloadCommand.hpp"

#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

#include "commands/ScanCommand.hpp"
#include "exceptions/PathResolutionException.hpp"
#include "pathing/PathManager.hpp"

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    auto* file = static_cast<std::ofstream*>(userp);
    file->write(static_cast<char*>(contents), totalSize);
    return totalSize;
}

static int ProgressCallback(void*, curl_off_t dltotal, curl_off_t dlnow,
                            curl_off_t, curl_off_t) {
    if (dltotal > 0) {
        int percentage = static_cast<int>((dlnow * 100) / dltotal);
        std::cout << "\rProgress: " << percentage << "% ("
                  << dlnow << "/" << dltotal << " bytes)" << std::flush;
    }
    return 0;
}

std::string DownloadCommand::getName() {
    return "download";
}

std::string DownloadCommand::getDescription() {
    return "Download a module from URL and scan it automatically";
}

std::string DownloadCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        return "Error: No URL provided\n" + getDescription();
    }

    const std::string& url = args[0];

    if (!isValidUrl(url)) {
        return "Error: Invalid URL format";
    }

    try {
        auto& paths = PathManager::getInstance();
        paths.ensureExists("modules.downloaded");

        std::string filename = extractFilenameFromUrl(url);
        if (filename.empty()) {
            filename = "downloaded_module.dll";
        }

        auto basePath = paths.get("modules.downloaded");
        auto downloadPath = basePath / filename;

        std::cout << "Downloading from: " << url << "\n";
        std::cout << "Saving to: " << downloadPath << "\n";

        if (!downloadFile(url, downloadPath)) {
            return "Error: Failed to download file from " + url;
        }

        std::cout << "\n✓ Download completed\n\nStarting automatic scan...\n";

        ScanCommand scan;
        std::vector<std::string> scanArgs{ basePath.string() };

        std::stringstream result;
        result << "✓ File downloaded successfully to: " << downloadPath << "\n\n";
        result << "=== Scan Results ===\n";
        result << scan.execute(scanArgs);

        return result.str();

    } catch (const PathResolutionException& e) {
        return std::string("Path error: ") + e.what();
    } catch (const std::exception& e) {
        return std::string("Error: ") + e.what();
    }
}

bool DownloadCommand::downloadFile(const std::string& url,
                                   const std::filesystem::path& destination) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Error: Failed to initialize CURL\n";
        return false;
    }

    std::ofstream outFile(destination, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Error: Cannot create file: " << destination << "\n";
        curl_easy_cleanup(curl);
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300L);

    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Nexus/1.0");

    CURLcode res = curl_easy_perform(curl);
    outFile.close();

    if (res != CURLE_OK) {
        std::cerr << "\nError: Download failed: "
                  << curl_easy_strerror(res) << "\n";
        curl_easy_cleanup(curl);
        std::filesystem::remove(destination);
        return false;
    }

    long responseCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

    const char* contentType = nullptr;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &contentType);

    curl_easy_cleanup(curl);

    if (responseCode != 200) {
        std::cerr << "\nError: HTTP " << responseCode << "\n";
        std::filesystem::remove(destination);
        return false;
    }

    if (contentType && std::string(contentType).find("text/html") != std::string::npos) {
        std::cerr << "\nError: URL returned HTML, not a binary file\n";
        std::filesystem::remove(destination);
        return false;
    }

    std::cout << "\n";
    return true;
}

std::string DownloadCommand::extractFilenameFromUrl(const std::string& url) {
    size_t slash = url.find_last_of('/');
    if (slash == std::string::npos) return "";

    std::string filename = url.substr(slash + 1);

    if (auto q = filename.find('?'); q != std::string::npos)
        filename = filename.substr(0, q);
    if (auto f = filename.find('#'); f != std::string::npos)
        filename = filename.substr(0, f);

    return filename;
}

bool DownloadCommand::isValidUrl(const std::string& url) {
    return url.rfind("http://", 0) == 0 || url.rfind("https://", 0) == 0;
}
