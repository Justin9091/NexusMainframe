//
// Created by jusra on 17-1-2026.
//

#ifndef NEXUSCORE_DOWNLOADCOMMAND_HPP
#define NEXUSCORE_DOWNLOADCOMMAND_HPP
#include <filesystem>

#include "Command.hpp"

class DownloadCommand : public Command {
public:
    std::string getName() override;
    std::string getDescription() override;
    std::string execute(const std::vector<std::string> &args) override;

private:
    bool downloadFile(const std::string& url, const std::filesystem::path& destination);
    std::string extractFilenameFromUrl(const std::string& url);
    bool isValidUrl(const std::string& url);
};

#endif //NEXUSCORE_DOWNLOADCOMMAND_HPP