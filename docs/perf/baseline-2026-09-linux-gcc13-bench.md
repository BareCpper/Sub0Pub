# Sub0Pub benchmark report

```
== System Info ==
OS: Linux
Compiler: GCC 13.3.0
CPU: Intel(R) Xeon(R) Processor @ 2.80GHz
Threads: 4
RAM: 15 GB
Build: Release
==
```

instr/op: callgrind instruction count / 10000 iterations (includes ~3 instructions of loop overhead).

## Core publish/subscribe by policy

### instr/op

| Scenario | Snapshot (default) | Direct unchecked | Direct + check | ThreadSafe |
|---|---:|---:|---:|---:|
| **Publish** | | | | |
| 0 subscribers | 38.0 | 39.0 | 38.0 | 123.0 |
| 1 subscriber | 72.0 | 60.0 | 59.0 | 157.0 |
| 2 subscribers | 108.0 | 81.0 | 80.0 | 194.0 |
| 4 subscribers | 155.0 | 123.0 | 122.0 | 241.0 |
| 8 subscribers (max default) | 247.0 | 207.0 | 206.0 | 333.0 |
| **Filter and cancel** | | | | |
| 1 filtered subscriber (pass) | 74.0 | 62.0 | 61.0 | 159.0 |
| 1 filtered subscriber (reject) | 68.0 | 56.0 | 55.0 | 153.0 |
| 8 subscribers, first cancels | 87.0 | 59.0 | 58.0 | 173.0 |
| **Multi-type dispatch** | | | | |
| int publish (2-type publisher) | 72.0 | 60.0 | 59.0 | 157.0 |
| float publish (2-type publisher) | 59.0 | 57.0 | 56.0 | 138.0 |
| **Re-entrant publish** | | | | |
| 1 subscriber, nested depth 1 | 147.0 | - | - | 312.0 |
| **Subscription lifetime** | | | | |
| create + destroy subscriber (empty table) | 61.0 | 61.0 | 66.0 | 218.0 |
| create + destroy subscriber (7 others) | 77.0 | 77.0 | 82.0 | 233.0 |
| unsubscribe first of 8 + resubscribe | 88.0 | 88.0 | 92.0 | 237.0 |
| create + destroy subscriber (table full, rejected) | 71.0 | 71.0 | 74.0 | 228.0 |
| trySubscribe() (table full, rejected) | 12.0 | 12.0 | 14.0 | 90.0 |
| trySubscribe() (already subscribed) | 9.0 | 9.0 | 9.0 | 87.0 |
| **Floor (no Sub0Pub)** | | | | |
| direct call (collapse target), 1 receiver | 6.0 | 6.0 | 6.0 | 6.0 |
| direct call (collapse target), 8 receivers | 13.0 | 13.0 | 13.0 | 13.0 |
| virtual receive loop, 1 receiver | 11.0 | 11.0 | 11.0 | 11.0 |
| virtual receive loop, 8 receivers | 89.0 | 89.0 | 89.0 | 89.0 |
| virtual filter+receive loop, 1 receiver | 25.0 | 25.0 | 25.0 | 25.0 |
| virtual filter+receive loop, 8 receivers | 118.0 | 118.0 | 118.0 | 118.0 |
| std::function loop, 1 receiver | 30.0 | 30.0 | 30.0 | 30.0 |
| std::function loop, 8 receivers | 100.0 | 100.0 | 100.0 | 100.0 |

### ns/op

| Scenario | Snapshot (default) | Direct unchecked | Direct + check | ThreadSafe |
|---|---:|---:|---:|---:|
| **Publish** | | | | |
| 0 subscribers | 3.10 | 3.24 | 2.74 | 9.07 |
| 1 subscriber | 5.01 | 4.58 | 4.59 | 11.01 |
| 2 subscribers | 10.59 | 7.49 | 8.20 | 15.36 |
| 4 subscribers | 14.25 | 12.16 | 13.70 | 19.58 |
| 8 subscribers (max default) | 24.98 | 22.15 | 28.96 | 29.22 |
| **Filter and cancel** | | | | |
| 1 filtered subscriber (pass) | 5.12 | 4.59 | 4.76 | 10.95 |
| 1 filtered subscriber (reject) | 4.33 | 3.65 | 3.95 | 10.48 |
| 8 subscribers, first cancels | 6.11 | 4.28 | 4.28 | 11.61 |
| **Multi-type dispatch** | | | | |
| int publish (2-type publisher) | 4.96 | 4.60 | 4.65 | 10.94 |
| float publish (2-type publisher) | 4.61 | 3.99 | 4.08 | 9.46 |
| **Re-entrant publish** | | | | |
| 1 subscriber, nested depth 1 | 10.04 | - | - | 24.04 |
| **Subscription lifetime** | | | | |
| create + destroy subscriber (empty table) | 4.66 | 4.58 | 4.79 | 14.68 |
| create + destroy subscriber (7 others) | 5.67 | 5.65 | 5.86 | 16.14 |
| unsubscribe first of 8 + resubscribe | 8.04 | 7.75 | 7.43 | 16.05 |
| create + destroy subscriber (table full, rejected) | 5.20 | 5.18 | 4.93 | 15.31 |
| trySubscribe() (table full, rejected) | 1.24 | 1.55 | 1.53 | 6.62 |
| trySubscribe() (already subscribed) | 1.57 | 1.22 | 1.23 | 6.09 |
| **Floor (no Sub0Pub)** | | | | |
| direct call (collapse target), 1 receiver | 1.98 | 2.01 | 2.07 | 1.96 |
| direct call (collapse target), 8 receivers | 2.76 | 2.76 | 2.76 | 2.75 |
| virtual receive loop, 1 receiver | 1.87 | 1.86 | 1.53 | 1.83 |
| virtual receive loop, 8 receivers | 11.35 | 11.34 | 10.99 | 11.29 |
| virtual filter+receive loop, 1 receiver | 2.12 | 1.87 | 1.84 | 2.13 |
| virtual filter+receive loop, 8 receivers | 14.67 | 12.45 | 11.92 | 12.18 |
| std::function loop, 1 receiver | 2.51 | 2.46 | 2.25 | 4.30 |
| std::function loop, 8 receivers | 19.81 | 11.77 | 11.30 | 18.62 |

## IPC end-to-end

### instr/op

| Scenario | Default |
|---|---:|
| **IPC 4B payload (17B framed)** | |
| serialize: publish -> stream | 133.0 |
| deserialize: stream -> subscriber | 472.0 |
| floor: memcpy framed message | 23.0 |
| **IPC 64B payload (77B framed)** | |
| serialize: publish -> stream | 139.0 |
| deserialize: stream -> subscriber | 469.0 |
| floor: memcpy framed message | 34.0 |
| **IPC 256B payload (269B framed)** | |
| serialize: publish -> stream | 155.0 |
| deserialize: stream -> subscriber | 487.0 |
| floor: memcpy framed message | 65.0 |

### ns/op

| Scenario | Default |
|---|---:|
| **IPC 4B payload (17B framed)** | |
| serialize: publish -> stream | 12.86 |
| deserialize: stream -> subscriber | 40.30 |
| floor: memcpy framed message | 3.04 |
| **IPC 64B payload (77B framed)** | |
| serialize: publish -> stream | 13.19 |
| deserialize: stream -> subscriber | 37.19 |
| floor: memcpy framed message | 3.03 |
| **IPC 256B payload (269B framed)** | |
| serialize: publish -> stream | 17.53 |
| deserialize: stream -> subscriber | 39.74 |
| floor: memcpy framed message | 4.70 |

