//
// Created by jusra on 15-1-2026.
//

#ifndef NEXUSCORE_WINDOWSMODULESCANNER_HPP
#define NEXUSCORE_WINDOWSMODULESCANNER_HPP

#ifdef _WIN32

#include <windows.h>
#include <string>
#include <vector>
#include <unordered_set>

#include "IModuleScanner.hpp"
#include "Modules/IModule.hpp"
#include "Modules/operations/ModuleOperationsFactory.hpp"

#include <filesystem>
#include <unordered_set>
#include <algorithm>


namespace fs = std::filesystem;

class WindowsModuleScanner final : public IModuleScanner {
public:
    WindowsModuleScanner()
        : osOps_(ModuleOperationsFactory::create()) {}

    std::vector<ModuleInfo> scanDirectory(const std::string& directory) override {
        std::vector<ModuleInfo> modules;
        scanIterative(widen(directory), modules);
        return modules;
    }

    bool moduleExists(const std::string& modulePath) override {
        DWORD attrs = GetFileAttributesW(widen(modulePath).c_str());
        return attrs != INVALID_FILE_ATTRIBUTES;
    }

private:
    std::unique_ptr<IModuleOperations> osOps_;

    // =========================
    // Helpers
    // =========================

    static std::wstring widen(const std::string& str) {
        return std::wstring(str.begin(), str.end());
    }

    static std::string narrow(const std::wstring& ws) {
        return std::string(ws.begin(), ws.end());
    }

    static bool isDll(const wchar_t* name) {
        const wchar_t* ext = wcsrchr(name, L'.');
        return ext && _wcsicmp(ext, L".dll") == 0;
    }

    static bool shouldSkipDir(const wchar_t* name) {
        static const std::unordered_set<std::wstring> skip = {
            L".git", L".vs", L".idea",
            L"node_modules", L"build",
            L"dist", L"cmake-build-debug",
            L"cmake-build-release",
            L"System Volume Information",
            L"$RECYCLE.BIN"
        };
        return skip.contains(name);
    }

    void scanIterative(const std::wstring& root, std::vector<ModuleInfo>& out) {
        std::vector<std::wstring> stack;
        stack.reserve(1024);
        stack.push_back(root);

        while (!stack.empty()) {
            std::wstring dir = std::move(stack.back());
            stack.pop_back();

            std::wstring pattern = dir + L"\\*";

            WIN32_FIND_DATAW data;
            HANDLE hFind = FindFirstFileExW(
                pattern.c_str(),
                FindExInfoBasic,
                &data,
                FindExSearchNameMatch,
                nullptr,
                FIND_FIRST_EX_LARGE_FETCH
            );

            if (hFind == INVALID_HANDLE_VALUE)
                continue;

            do {
                const wchar_t* name = data.cFileName;

                if (wcscmp(name, L".") == 0 || wcscmp(name, L"..") == 0)
                    continue;

                const bool isDir =
                    data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

                const bool isReparse =
                    data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT;

                if (isDir) {
                    // ❗ Cruciaal: geen symlinks / junctions volgen
                    if (isReparse)
                        continue;

                    if (shouldSkipDir(name))
                        continue;

                    stack.push_back(dir + L"\\" + name);
                    continue;
                }

                if (!isDll(name))
                    continue;

                std::wstring fullPath = dir + L"\\" + name;
                auto info = inspectModule(narrow(fullPath));

                if (info.isValid)
                    out.push_back(std::move(info));

            } while (FindNextFileW(hFind, &data));

            FindClose(hFind);
        }
    }

    // =========================
    // Module inspect
    // =========================

    ModuleInfo inspectModule(const std::string& modulePath) {
        ModuleInfo info;
        info.path = modulePath;
        info.filename = modulePath.substr(modulePath.find_last_of("\\/") + 1);
        info.name = info.filename;
        info.isValid = false;

        void* handle = osOps_->loadLibrary(modulePath);
        if (!handle)
            return info;

        using CreateModuleFn = IModule* (*)();
        auto createFunc = reinterpret_cast<CreateModuleFn>(
            osOps_->getFunction(handle, "createModule")
        );

        if (createFunc) {
            if (IModule* module = createFunc()) {
                info.name = module->getName();
                info.isValid = true;
                delete module;
            }
        }

        osOps_->unloadLibrary(handle);
        return info;
    }
};


#endif // _WIN32
#endif // NEXUSCORE_WINDOWSMODULESCANNER_HPP
