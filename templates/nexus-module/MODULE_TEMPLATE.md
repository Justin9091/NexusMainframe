# Module Template

This guide explains how to create a new module for NexusMainFrame.

---

## Quick start

1. Create a new shared library project (`.dll` on Windows, `.so` on Linux).
2. Include `IModule.hpp` and implement the interface (see below).
3. Export the factory function `createModule`.
4. Drop the compiled binary into the modules directory and load it via the HTTP API.

---

## Minimal implementation

```cpp
// MyModule.hpp
#pragma once
#include "Modules/IModule.hpp"

class MyModule : public IModule {
public:
    std::string getName() const override { return "MyModule"; }
    void initialize(EventBus& eventBus) override;
    void shutdown() override;
};
```

```cpp
// MyModule.cpp
#include "MyModule.hpp"

void MyModule::initialize(EventBus& eventBus) {
    auto logger = getLogger();
    logger.logInfo("MyModule initialized");

    // Subscribe to events
    eventBus.subscribe("some.event", [this](const Event& e) {
        // handle event
    });
}

void MyModule::shutdown() {
    auto logger = getLogger();
    logger.logInfo("MyModule shutting down");
    // Release resources, stop threads, etc.
}

// Required factory export — do NOT rename this function
extern "C" __declspec(dllexport) IModule* createModule() {
    return new MyModule();
}
```

> **Linux:** Replace `__declspec(dllexport)` with `__attribute__((visibility("default")))`.

---

## IModule interface

| Method | When called | What to do |
|--------|-------------|------------|
| `getName()` | Any time | Return a **stable, unique** name. Used in HTTP API and logs. |
| `initialize(EventBus&)` | Once, after load | Subscribe to events, start threads, open connections. |
| `shutdown()` | Once, before unload | Unsubscribe, stop threads, release all resources. |

### `getLogger()`

`IModule` provides a convenience helper:

```cpp
auto logger = getLogger(); // Logger named after getName()
logger.logInfo("...");
logger.logWarning("...");
logger.logError("...");
logger.logCritical("...");
```

Call it once at the start of a method and store the result — don't call it on every log line.

---

## EventBus

Subscribe to a named event channel in `initialize`:

```cpp
eventBus.subscribe("sensor.reading", [](const Event& e) {
    auto value = std::any_cast<double>(e.data);
});
```

Publish an event from anywhere:

```cpp
EventBus::getInstance().publish({"sensor.reading", 42.0});
```

**Important:**
- Do **not** publish events from `shutdown()` — the bus may already be shutting down.
- EventBus callbacks can fire from **any thread**. Protect shared state with a mutex.
- Unsubscribe in `shutdown()` using the `int` ID returned by `subscribe()`.

```cpp
// In your class
int _subscriptionId;

// initialize
_subscriptionId = eventBus.subscribe("some.event", [this](const Event& e) { ... });

// shutdown
EventBus::getInstance().unsubscribe("some.event", _subscriptionId);
```

---

## Module directory

| Platform | Path |
|----------|------|
| Windows  | `C:\Users\<user>\.nexus\modules\downloaded\<name>.dll` |
| Linux    | `~/.nexus/modules/downloaded/<name>.so` |

---

## HTTP API

Load and unload modules at runtime without restarting the application:

```http
POST /api/modules/{name}/enable
POST /api/modules/{name}/disable
GET  /api/modules
```

The `{name}` must match what `getName()` returns.

---

## Checklist before shipping a module

- [ ] `getName()` returns a unique, stable name (no version numbers).
- [ ] All resources started in `initialize()` are released in `shutdown()`.
- [ ] EventBus subscriptions are unsubscribed in `shutdown()`.
- [ ] No raw pointers to `EventBus` stored — use the reference or `getInstance()`.
- [ ] `createModule()` is exported with the correct visibility attribute for the platform.
- [ ] Compiled against the same C++ standard and runtime as `nexus_core` (C++20, MinGW on Windows).
