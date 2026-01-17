//
// Created by jusra on 17-1-2026.
//

#ifndef NEXUSCORE_PATHMANAGER_HPP
#define NEXUSCORE_PATHMANAGER_HPP

#include <filesystem>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include "resolver/IPathResolver.hpp"

class PathManager {
public:
    static PathManager& getInstance();

    std::filesystem::path get(const std::string& pathKey) const;

    void registerPath(const std::string& key,
                     const std::filesystem::path& relativePath);

    void registerPath(const std::string& key,
                     std::function<std::filesystem::path()> resolver);

    void ensureExists(const std::string& pathKey) const;
    bool exists(const std::string& pathKey) const;

    std::filesystem::path getBase() const { return _basePath; }
    std::filesystem::path getCustom(const std::string& relativePath) const;

    PathManager(const PathManager&) = delete;
    PathManager& operator=(const PathManager&) = delete;

private:
    PathManager();

    std::unique_ptr<IPathResolver> _resolver;
    std::filesystem::path _basePath;
    std::unordered_map<std::string, std::function<std::filesystem::path()>> _pathResolvers;

    void initialize();
    void registerCorePaths();
};

#endif //NEXUSCORE_PATHMANAGER_HPP