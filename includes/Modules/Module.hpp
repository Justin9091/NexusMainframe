#ifndef NEXUSCORE_MODULE_HPP
#define NEXUSCORE_MODULE_HPP

#include <functional>
#include <memory>
#include <string>

#include "Modules/IModule.hpp"

class Module {
public:
    Module(std::string name, std::string path,
           std::shared_ptr<IModule> instance,
           std::function<void()> handleFree);

    ~Module();

    Module(Module&&) noexcept;
    Module& operator=(Module&&) noexcept;
    Module(const Module&) = delete;
    Module& operator=(const Module&) = delete;

    const std::string& getName() const { return name_; }
    const std::string& getPath() const { return path_; }

    IModule* operator->() const { return instance_.get(); }
    IModule& operator*() const { return *instance_; }

private:
    std::string name_;
    std::string path_;
    std::shared_ptr<IModule> instance_;
    std::function<void()> handleFree_;
};

#endif // NEXUSCORE_MODULE_HPP
