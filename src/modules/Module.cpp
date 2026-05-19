#include "modules/Module.hpp"

Module::Module(std::string name, std::string path,
               std::shared_ptr<IModule> instance,
               std::function<void()> handleFree)
    : name_(std::move(name)),
      path_(std::move(path)),
      instance_(std::move(instance)),
      handleFree_(std::move(handleFree)) {}

Module::~Module() {
    instance_.reset(); // destroy IModule before freeing the library
    if (handleFree_) handleFree_();
}

Module::Module(Module&& other) noexcept
    : name_(std::move(other.name_)),
      path_(std::move(other.path_)),
      instance_(std::move(other.instance_)),
      handleFree_(std::move(other.handleFree_)) {}

Module& Module::operator=(Module&& other) noexcept {
    if (this != &other) {
        instance_.reset();
        if (handleFree_) handleFree_();

        name_       = std::move(other.name_);
        path_       = std::move(other.path_);
        instance_   = std::move(other.instance_);
        handleFree_ = std::move(other.handleFree_);
    }
    return *this;
}
