//
// Created by jusra on 15-1-2026.
//

#ifndef NEXUSCORE_LINUXMODULEOPERATIONS_HPP
#define NEXUSCORE_LINUXMODULEOPERATIONS_HPP

#include "IModuleOperations.hpp"

#include <dlfcn.h>

class LinuxModuleOperations : public IModuleOperations {
public:
    void* loadLibrary(const std::string& libraryPath) override {
        void* handle = dlopen(libraryPath.c_str(), RTLD_LAZY);
        if (!handle) {
            lastError_ = std::string("Failed to load library: ") + dlerror();
        }
        return handle;
    }

    bool unloadLibrary(void* handle) override {
        if (!handle) return false;

        int result = dlclose(handle);
        if (result != 0) {
            lastError_ = std::string("Failed to unload library: ") + dlerror();
            return false;
        }
        return true;
    }

    void* getFunction(void* handle, const std::string& functionName) override {
        if (!handle) return nullptr;

        // Reset errors
        dlerror();

        void* func = dlsym(handle, functionName.c_str());
        const char* error = dlerror();
        if (error) {
            lastError_ = std::string("Cannot load symbol: ") + error;
            return nullptr;
        }
        return func;
    }

    std::string getLastError() const override {
        return lastError_;
    }

    std::string getLibraryExtension() const override {
        return ".o";
    }

private:
    std::string lastError_;
};

#endif //NEXUSCORE_LINUXMODULEOPERATIONS_HPP