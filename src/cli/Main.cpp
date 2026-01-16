//
// Created by kikker234 on 16-01-2026.
//

#include "../../includes/cli/NexusCLI.h"

int main(int argc, char* argv[]) {
    NexusCLI cli("127.0.0.1", 9999);
    return cli.run(argc, argv);
}
