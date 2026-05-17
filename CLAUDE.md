# NexusMainFrame

C++20 modulair framework voor embedded/desktop systemen. Draait op Windows (primair, MinGW) en Linux.

## Projectstructuur

```
NexusMainFrame/
├── CMakeLists.txt          # Root build — targets: nexus_core (shared lib), NexusMainFrame (exe)
├── cmake/
│   ├── Dependencies.cmake  # FetchContent: spdlog, httplib, croncpp, nlohmann_json, curl, Catch2
│   └── CompilerOptions.cmake # Warnings, optimalisaties, unity builds, coverage
├── NexusMainFrame.cpp/.hpp # Applicatie-entrypoint: start/run/stop lifecycle
├── main.cpp                # main() — roept NexusMainFrame::run() aan
├── includes/               # Publieke headers (geëxporteerd door nexus_core)
│   ├── Modules/            # IModule, ModuleLoader/Unloader/Manager, operations
│   ├── server/             # HttpServer, Endpoint, HttpMethod
│   ├── IPC/                # CommandServer (TCP socket op poort 9999)
│   ├── Event/              # EventBus
│   ├── Scheduler/          # Scheduler (cron-gebaseerd via croncpp)
│   ├── mqtt/               # MQTTClient
│   ├── commands/           # Command interface + registry + concrete commands
│   ├── config/             # ConfigProvider, JsonConfigSource, adapters
│   └── pathing/            # PathManager, PathResolver (cross-platform)
└── src/                    # Implementaties
└── tests/                  # Catch2 unit tests
    ├── config/source/
    ├── Event/
    └── command/
```

## Bouwen

```bash
# Debug (standaard)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Met tests
cmake -B build -DNEXUS_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build

# Release
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

**Opties:**
- `NEXUS_BUILD_TESTS=ON` — bouwt `nexus_tests` met Catch2
- `NEXUS_FAST_BUILD=ON` — unity builds (standaard aan, batch size 4)
- `NEXUS_ENABLE_COV=ON` — gcov coverage (niet MSVC)
- `NEXUS_ENABLE_ASAN=OFF` — ASan uitgeschakeld (MinGW incompatibel)

## Architectuur

**nexus_core** is een shared library (.dll/.so). De executable (`NexusMainFrame`) linkt er tegenaan.

### Kernonderdelen

| Component | Locatie | Rol |
|-----------|---------|-----|
| `NexusMainFrame` | `NexusMainFrame.cpp` | Lifecycle: start/run/stop |
| `ModuleManager` | `includes/Modules/ModuleManager.hpp` | Laadt/unlaadt .dll modules dynamisch |
| `HttpServer` | `includes/server/HttpServer.hpp` | REST API op poort 8080 (cpp-httplib) |
| `CommandServer` | `includes/IPC/CommandServer.hpp` | TCP IPC op poort 9999 |
| `EventBus` | `includes/Event/EventBus.hpp` | Publish/subscribe, singleton |
| `Scheduler` | `includes/Scheduler/Scheduler.hpp` | Cron-gebaseerde taakplanning |
| `MQTTClient` | `includes/mqtt/MQTTClient.hpp` | MQTT verbinding (hardcoded: 192.168.2.161:1883) |
| `PathManager` | `includes/pathing/PathManager.hpp` | Platform-onafhankelijke padresolutie |

### Module systeem

Modules zijn externe .dll/.so bestanden. Interface:
```cpp
class IModule {
    virtual std::string getName() const = 0;
    virtual void initialize(EventBus& eventBus) = 0;
    virtual void shutdown() = 0;
};
```

Module zoekpad (Windows): `C:\Users\jusra\.nexus\modules\downloaded\<naam>.dll`
Module zoekpad (Linux): vergelijkbaar via `UnixPathResolver`

### HTTP API endpoints

| Methode | Pad | Beschrijving |
|---------|-----|-------------|
| GET | `/api/status` | Simpele OK check |
| GET | `/api/health` | Uptime, MQTT status, geladen modules |
| GET | `/api/metrics` | Uitgebreide metrics incl. geheugen |
| GET | `/api/modules` | Lijst van geladen modules |
| POST | `/api/modules/:name/enable` | Laad module |
| POST | `/api/modules/:name/disable` | Unlaad module |
| POST | `/api/command` | Voer commando uit (gedeeltelijk geïmplementeerd) |

## Afhankelijkheden

| Library | Versie | Doel |
|---------|--------|------|
| spdlog | 1.12 | Logging |
| cpp-httplib | 0.18.1 | HTTP server/client |
| croncpp | master | Cron expressie parsing |
| nlohmann/json | 3.11.3 | JSON serialisatie |
| libcurl | 8.5.0 (fallback) | HTTP downloads |
| Catch2 | 3.5.1 | Unit tests |

## Tests

Tests staan in `tests/`. Framework: Catch2 v3. Testprefixen: `nexus::`.

```bash
ctest --test-dir build --output-on-failure
```

Bestaande testbestanden:
- `tests/config/source/JsonConfigSourceTests.cpp`
- `tests/Event/EventBusTests.cpp`
- `tests/command/CommandRegistryTests.cpp`

## Bekende aandachtspunten

- `ModuleLoader` heeft een hardcoded Windows-pad — moet configureerbaar worden
- `/api/command` endpoint is gedeeltelijk geïmplementeerd (commentaar in code)
- `/api/modules` response bevat een trailing comma bug in JSON
- ASan werkt niet met MinGW; gebruik WSL2/Linux voor memory debugging
- MQTT broker IP (192.168.2.161) is hardcoded in `NexusMainFrame.cpp`
