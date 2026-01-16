//
// Created by jusra on 15-1-2026.
//

#ifndef NEXUSCORE_WINDOWSMODULEOPERATIONS_HPP
#define NEXUSCORE_WINDOWSMODULEOPERATIONS_HPP
#include "IModuleOperations.hpp"
#include <windows.h>

class WindowsModuleOperations : public IModuleOperations {
public:
    void* loadLibrary(const std::string& libraryPath) override {
        HMODULE handle = LoadLibraryA(libraryPath.c_str());
        if (!handle) {
            lastError_ = "Failed to load library: " + libraryPath;
        }
        return handle;
    }

    bool unloadLibrary(void* handle) override {
        if (!handle) return false;

        BOOL result = FreeLibrary(static_cast<HMODULE>(handle));
        if (!result) {
            lastError_ = "Failed to unload library";
            return false;
        }
        return true;
    }

    void* getFunction(void* handle, const std::string& functionName) override {
        if (!handle) return nullptr;

        FARPROC func = GetProcAddress(static_cast<HMODULE>(handle), functionName.c_str());
        if (!func) {
            lastError_ = "Cannot find function: " + functionName;
        }
        return reinterpret_cast<void*>(func);
    }

    std::string getLastError() const override {
        return lastError_;
    }

    std::string getLibraryExtension() const override {
        return ".dll";
    }

private:
    std::string lastError_;
};

#endif //NEXUSCORE_WINDOWSMODULEOPERATIONS_HPP
