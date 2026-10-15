# OpenVikings: Object Limits and Capacity Management

Review date: October 14, 2026.

This document summarizes the default object capacities identified in the current OpenVikings C++ implementation and the original Windows version of **Cultures: 8th Wonder of the World**. The limits apply to the entire map and are shared by all players; they are not separate allowances for each player.

## Default capacities

| Object or storage pool | Default capacity | Notes |
| --- | ---: | --- |
| Humans | 1,000 | Includes children and humans belonging to different players. |
| Buildings | 400 | Allocated building records, including buildings under construction. |
| Animals | 500 | Shared animal pool. |
| Vehicles and ships | 200 | Both use the same vehicle array. |
| Guides / signposts | 400 | Separate guide array. |
| Markers | 1,000 | Also require entries in the shared map-moveable pool. |
| Map moveables | 2,000 | Shared by humans, animals, vehicles and markers. |
| Pathfinder records | 2,000 | Each initialized map moveable reserves a record. This is not the number of searches computed simultaneously. |
| Human merchant records | 1,000 | Supporting pool that must also be considered when increasing human-related capacities. |

The capacities for humans, buildings, animals, vehicles, guides, map moveables and pathfinder records were checked against both the Windows assembly export and the corresponding instructions in the original `Game.exe`. Marker and merchant capacities are also present in the current C++ constructors; they were not independently byte-verified during this review.

## How the limits are defined

Buildings, humans, vehicles, animals and guides declare an `InitialCapacity` constant in `C2LogicObjectArrays.h`. For example:

```cpp
static constexpr std::int32_t InitialCapacity = 1000;
```

The constructor allocates a contiguous block whose size is:

```text
allocation size = capacity × record size
```

A human record occupies 3,160 bytes in the current Windows layout. The default human array therefore reserves 3,160,000 bytes for its records, excluding supporting data.

`CArrayHandler` stores the capacity, allocated count and free-slot search position. `Element_Allocate()` returns `-1` when the allocated count equals the capacity. The allocation path does not automatically grow the array. The remaining pools are initialized with explicit capacities in `C2LogicLifetimeAccess.cpp`.

## Shared limits and dependencies

An available human, animal or vehicle slot is not sufficient by itself. Creation also needs a slot in the shared map-moveable array, and map-moveable initialization allocates a pathfinder record. Creation can fail when either supporting pool is exhausted.

With 1,000 humans, 500 animals and 200 vehicles allocated, 1,700 of the 2,000 map-moveable slots are already used. Only 300 shared slots remain for markers. The marker array's own capacity of 1,000 does not guarantee that all 1,000 markers can coexist with those populations.

## Savegame behavior

The numbers above are default capacities for newly constructed arrays. Savegames store array capacities, and the load constructors allocate storage using the serialized values rather than always substituting `InitialCapacity`.

This means the default constants are not universal upper bounds on every loaded array. It does not establish that arbitrary larger capacities are safe: dependent pools, record sizes, identifiers and consumers must remain consistent. Arrays still do not grow automatically during normal object allocation.

## What must change to increase the limits

- Update each relevant capacity together with its allocation size, initialization loops and dependent pools.
- Review merchant records and other supporting structures rather than changing only the human or building constant.
- Audit savegame loading/saving, object references, map structures, callbacks and network consumers for assumptions about the original capacities.
- Review identifier widths. A map cell's moveable-list head is read as a signed 16-bit value; IDs from 32,768 upward would be interpreted as negative by that existing path. This is an additional representation constraint, not the current 2,000-object limit.
- Measure simulation, pathfinding and rendering performance at the intended population sizes. Available memory alone does not establish acceptable performance.

These capacities are software-defined. Moving the application to 64 bit does not automatically increase them. Higher limits require coordinated changes to the storage and its consumers.

## Source references

- [C2LogicObjectArrays.h](../C2LogicObjectArrays.h): default capacities and array-handler fields.
- [C2LogicObjectArrays.cpp](../C2LogicObjectArrays.cpp): allocation, full-array handling and generic savegame capacity loading.
- [C2LogicLifetimeAccess.cpp](../C2LogicLifetimeAccess.cpp): marker, map-moveable, merchant and pathfinder constructors.
- [C2MapMoveableAccess.cpp](../C2MapMoveableAccess.cpp): dependent pathfinder allocation and signed 16-bit map-cell list heads.
- [C2SavegameStateAccess.cpp](../C2SavegameStateAccess.cpp): map-moveable and pathfinder load constructors.

The Windows disassembly and original executable are authoritative for behavior. This was a source and binary inspection; no limits were changed, and no build or gameplay stress test was performed.
