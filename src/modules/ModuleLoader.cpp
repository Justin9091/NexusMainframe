//
// Created by jusra on 29-12-2025.
//

#include "../../includes/Modules/ModuleLoader.hpp"

#include <algorithm>
#include <iostream>
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

namespace fs = std::filesystem;

ModuleLoader::~ModuleLoader() {
    // Cleanup: sluit alle module handles
    for (auto& [name, handle] : moduleHandles_) {
        if (handle) {
#ifdef _WIN32
            FreeLibrary((HMODULE)handle);
#else
            dlclose(handle);
#endif
        }
    }
}

std::shared_ptr<IModule> ModuleLoader::loadModule(const std::string& libraryPath) {
    void* handle = nullptr;

#ifdef _WIN32
    handle = LoadLibraryA(libraryPath.c_str());
    if (!handle) {
        std::cerr << "Failed to load module: " << libraryPath << std::endl;
        return nullptr;
    }

    auto createFunc = (CreateModuleFn)GetProcAddress((HMODULE)handle, CREATE_MODULE_FUNC);
#else
    handle = dlopen(libraryPath.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load module: " << dlerror() << std::endl;
        return nullptr;
    }

    // Reset errors
    dlerror();

    auto createFunc = (CreateModuleFn)dlsym(handle, CREATE_MODULE_FUNC);
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Cannot load symbol: " << dlsym_error << std::endl;
        dlclose(handle);
        return nullptr;
    }
#endif

    if (!createFunc) {
        std::cerr << "Cannot find " << CREATE_MODULE_FUNC << " function" << std::endl;
#ifdef _WIN32
        FreeLibrary((HMODULE)handle);
#else
        dlclose(handle);
#endif
        return nullptr;
    }

    // Creëer module instance
    IModule* modulePtr = createFunc();
    if (!modulePtr) {
        std::cerr << "Module creation failed" << std::endl;
        return nullptr;
    }

    auto module = std::shared_ptr<IModule>(modulePtr);

    // Sla op
    modules_.push_back(module);
    moduleHandles_[module->getName()] = handle;

    std::cout << "Loaded module: " << module->getName() << std::endl;

    return module;
}

std::vector<std::shared_ptr<IModule>> ModuleLoader::loadModulesFromDirectory(const std::string& directory) {
    std::vector<std::shared_ptr<IModule>> loadedModules;

    if (!fs::exists(directory)) {
        std::cerr << "Module directory does not exist: " << directory << std::endl;
        return loadedModules;
    }

    // Zoek naar .so (Linux) of .dll (Windows) files
    std::string extension;
#ifdef _WIN32
    extension = ".dll";
#else
    extension = ".so";
#endif

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == extension) {
            auto module = loadModule(entry.path().string());
            if (module) {
                loadedModules.push_back(module);
            }
        }
    }

    return loadedModules;
}

bool ModuleLoader::unloadModule(const std::string& moduleName) {
    auto it = moduleHandles_.find(moduleName);
    if (it == moduleHandles_.end()) {
        return false;
    }

    // Remove from modules vector
    modules_.erase(
        std::remove_if(modules_.begin(), modules_.end(),
            [&moduleName](const auto& m) { return m->getName() == moduleName; }),
        modules_.end()
    );


    // Close handle
#ifdef _WIN32
    FreeLibrary((HMODULE)it->second);
#else
    dlclose(it->second);
#endif

    moduleHandles_.erase(it);


    return true;
}

const std::vector<std::shared_ptr<IModule>>& ModuleLoader::getLoadedModules() const {
    return modules_;
}