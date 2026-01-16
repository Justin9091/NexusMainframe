//
// Created by jusra on 15-1-2026.
//

#ifndef NEXUSCORE_IMODULEOPERATIONS_HPP
#define NEXUSCORE_IMODULEOPERATIONS_HPP

#include <string>

class IModuleOperations {
public:
    virtual ~IModuleOperations() = default;

    virtual void* loadLibrary(const std::string& libraryPath) = 0;
    virtual bool unloadLibrary(void* handle) = 0;
    virtual void* getFunction(void* handle, const std::string& functionName) = 0;
    virtual std::string getLastError() const = 0;
    virtual std::string getLibraryExtension() const = 0;
};
#endif //NEXUSCORE_IMODULEOPERATIONS_HPP