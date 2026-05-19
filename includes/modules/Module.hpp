#ifndef NEXUSCORE_MODULE_HPP
#define NEXUSCORE_MODULE_HPP

#include <functional>
#include <memory>
#include <string>

#include "modules/IModule.hpp"

/**
 * @brief RAII owner for a single loaded dynamic module.
 *
 * Holds a shared_ptr to the IModule instance and a cleanup callback that
 * unloads the shared library handle when the Module is destroyed.
 * Move-only; copying is disabled to ensure unique ownership of the handle.
 *
 * Dereference operators forward to the underlying IModule:
 * @code
 * Module m = loader.load("my-module").value();
 * m->initialize(bus);  // calls IModule::initialize
 * @endcode
 */
class Module {
public:
    /**
     * @param name        Module name as reported by IModule::getName().
     * @param path        Absolute path of the .dll / .so file.
     * @param instance    Shared pointer to the IModule implementation.
     * @param handleFree  Callback that releases the OS library handle.
     */
    Module(std::string name, std::string path,
           std::shared_ptr<IModule> instance,
           std::function<void()> handleFree);

    ~Module();

    Module(Module&&) noexcept;
    Module& operator=(Module&&) noexcept;
    Module(const Module&)            = delete;
    Module& operator=(const Module&) = delete;

    /** @brief Returns the module name (from IModule::getName()). */
    const std::string& getName() const { return name_; }

    /** @brief Returns the absolute path to the loaded library file. */
    const std::string& getPath() const { return path_; }

    /** @brief Provides arrow-access to the underlying IModule. */
    IModule* operator->() const { return instance_.get(); }

    /** @brief Provides reference-access to the underlying IModule. */
    IModule& operator*()  const { return *instance_; }

private:
    std::string              name_;
    std::string              path_;
    std::shared_ptr<IModule> instance_;
    std::function<void()>    handleFree_;
};

#endif // NEXUSCORE_MODULE_HPP
