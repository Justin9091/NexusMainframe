#include "commands/ScanCommand.hpp"
#include "Modules/scanner/ModuleScannerFactory.hpp"
#include <sstream>

std::string ScanCommand::getName() {
    return "scan";
}

std::string ScanCommand::getDescription() {
    return "Scans a directory for modules";
}

std::string ScanCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        return "Usage: scan <directory>";
    }

    const std::string& directory = args[0];

    const auto start = std::chrono::steady_clock::now();

    auto scanner = ModuleScannerFactory::create();
    const auto modules = scanner->scanDirectory(directory);

    const auto end = std::chrono::steady_clock::now();
    const auto durationMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (modules.empty()) {
        return "No modules found in directory: " + directory +
               " (" + std::to_string(durationMs) + " ms)";
    }

    std::ostringstream output;
    output << "Found " << modules.size()
           << " module(s) in " << directory
           << " (" << durationMs << " ms)"
           << ":\r\n\r\n";

    for (const auto& mod : modules) {
        if (!mod.isValid) continue;

        output << " - " << mod.name << "\r\n"
               << "    File: " << mod.filename << "\r\n"
               << "    Path: " << mod.path << "\r\n\r\n";
    }

    return output.str();
}
