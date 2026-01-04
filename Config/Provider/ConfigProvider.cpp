//
// Created by jusra on 4-1-2026.
//
#include "ConfigProvider.hpp"

ConfigProvider::ConfigProvider(std::unique_ptr<IConfigSource> source)
    : source_(std::move(source)) {}

void ConfigProvider::load(const std::string& filepath) {
    config_ = source_->load(filepath);
}

bool ConfigProvider::has(const std::string& key) const {
    return config_.find(key) != config_.end();
}