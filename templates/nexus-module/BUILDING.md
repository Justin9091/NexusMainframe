# Een Nexus-module maken

## 1. Kopieer dit template

```
cp -r templates/nexus-module/ mijn-module/
cd mijn-module/
```

Vervang overal `MyModule` door je module-naam (bestanden, class, `getName()`).

## 2. Bouw

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## 3. Installeer

```bash
cmake --install build
```

De `.dll` (Windows) of `.so` (Linux) wordt gekopieerd naar:
- Windows: `%USERPROFILE%\.nexus\modules\downloaded\`
- Linux: `~/.nexus/modules/downloaded/`

## 4. Activeer via de API

```http
POST http://localhost:8080/api/modules/MyModule/enable
```

Of herstart NexusMainFrame — modules in de state-store laden automatisch.

---

## Contracten

### Verplicht

| Wat | Vereiste |
|-----|----------|
| Export | `extern "C" IModule* createModule()` — exact deze signatuur |
| `getName()` | Uniek, komt overeen met de `.dll`-bestandsnaam (zonder extensie) |
| `shutdown()` | Alle subscriptions unsubscriben, threads stoppen |

### EventBus

```cpp
// Subscriben
int id = eventBus.subscribe("event.name", [](const Event& e) { ... });

// Publishen
eventBus.publish({ "event.name", std::string{"payload"} });

// Unsubscriben (doe dit altijd in shutdown())
eventBus.unsubscribe("event.name", id);
```

`Event::data` is `std::any` — cast met `std::any_cast<T>` en vang `std::bad_any_cast` op bij twijfel over het type.

### Logger

```cpp
auto log = getLogger();   // beschikbaar via IModule
log.logInfo("bericht");
log.logWarning("...");
log.logError("...");
```

---

## Veelgemaakte fouten

- **`createModule` ontbreekt of is niet `extern "C"`** → module laadt niet, geen foutmelding in de API.
- **Subscriptions niet unsubscriben in `shutdown()`** → dangling callback na unload, UB.
- **`.dll`-naam verschilt van `getName()`** → nexus kan de module niet terugvinden in de state.
