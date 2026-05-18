//
// Created by jusra on 17-1-2026.
//
// PathManager.cpp
#include "pathing/PathManager.hpp"
#include "pathing/resolver/PathResolverFactory.hpp"
#include "exceptions/PathResolutionException.hpp"
#include "pathing/PathValidator.hpp"

PathManager& PathManager::getInstance() {
    static PathManager instance;
    return instance;
}

PathManager::PathManager()
    : _resolver(PathResolverFactory::create()) {
    initialize();
    registerCorePaths();
}

void PathManager::initialize() {
    try {
        // Gebruik de resolver om base path te bepalen
        _basePath = _resolver->getNexusPath();

        // Optioneel: valideer parent directory bestaat
        // (niet .nexus zelf, die maken we later)
        auto parentPath = _basePath.parent_path();
        if (!std::filesystem::exists(parentPath)) {
            throw PathResolutionException(
                "Parent directory does not exist: " + parentPath.string()
            );
        }

    } catch (const PathResolutionException& e) {
        // Re-throw met meer context
        throw PathResolutionException(
            std::string("Failed to initialize PathManager: ") + e.what()
        );
    }
}

void PathManager::registerCorePaths() {
    registerPath("base", [this]() { return _basePath; });

    registerPath("modules", [this]() {
        return _basePath / "modules";
    });

    registerPath("modules.builtin", [this]() {
        return _basePath / "modules" / "builtin";
    });

    registerPath("modules.downloaded", [this]() {
        return _basePath / "modules" / "downloaded";
    });

    registerPath("modules.custom", [this]() {
        return _basePath / "modules" / "custom";
    });

    registerPath("config", [this]() {
        return _basePath / "config";
    });

    registerPath("cache", [this]() {
        return _basePath / "cache";
    });

    registerPath("cache.downloads", [this]() {
        return _basePath / "cache" / "downloads";
    });

    registerPath("logs", [this]() {
        return _basePath / "logs";
    });

    registerPath("data", [this]() {
        return _basePath / "data";
    });

    registerPath("data.scans", [this]() {
        return _basePath / "data" / "scan-results";
    });
}

std::filesystem::path PathManager::get(const std::string& pathKey) const {
    auto it = _pathResolvers.find(pathKey);
    if (it == _pathResolvers.end()) {
        throw PathResolutionException("Unknown path key: " + pathKey);
    }
    return it->second();
}

void PathManager::registerPath(const std::string& key,
                               const std::filesystem::path& relativePath) {
    if (key.empty()) {
        throw PathResolutionException("Path key cannot be empty");
    }

    registerPath(key, [this, relativePath]() {
        return _basePath / relativePath;
    });
}

void PathManager::registerPath(const std::string& key,
                               std::function<std::filesystem::path()> resolver) {
    if (key.empty()) {
        throw PathResolutionException("Path key cannot be empty");
    }

    if (!resolver) {
        throw PathResolutionException("Path resolver cannot be null for key: " + key);
    }

    _pathResolvers[key] = resolver;
}

void PathManager::ensureExists(const std::string& pathKey) const {
    try {
        auto path = get(pathKey);

        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directories(path);
        }

        // Valideer dat het nu een directory is
        PathValidator::validateIsDirectory(path, "Path '" + pathKey + "'");

    } catch (const std::filesystem::filesystem_error& e) {
        throw PathResolutionException(
            "Failed to create directory for '" + pathKey + "': " + e.what()
        );
    }
}

bool PathManager::exists(const std::string& pathKey) const {
    try {
        return std::filesystem::exists(get(pathKey));
    } catch (const PathResolutionException&) {
        return false;
    }
}

std::filesystem::path PathManager::getCustom(const std::string& relativePath) const {
    if (relativePath.empty()) {
        throw PathResolutionException("Relative path cannot be empty");
    }

    return _basePath / relativePath;
}