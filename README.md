# RTOS Scheduler Simulator

A C++ command-line simulator for real-time task scheduling. It models periodic embedded tasks, preemption, context switches, deadline misses, idle time, and CPU utilization.

This project is built for Computer Engineering internship applications. It shows modern C++, algorithms, real-time systems concepts, test-driven validation, and clean technical documentation.

## What It Does

- Loads periodic task sets from CSV.
- Simulates a preemptive single-core scheduler at 1 ms resolution.
- Supports Rate Monotonic Scheduling (RMS) and Earliest Deadline First (EDF).
- Reports CPU utilization, context switches, preemptions, completed jobs, unfinished jobs, and deadline misses.
- Emits a readable timeline of task execution.
- Includes unit tests and GitHub Actions CI.

## Why This Project Is Useful

Embedded systems often run multiple periodic tasks:

- read a sensor every 10 ms
- update a control loop every 20 ms
- log telemetry every 50 ms
- send a heartbeat every 100 ms

An RTOS scheduler decides which task runs when. This simulator helps explain whether a task set can meet deadlines before running it on hardware.

## Quick Start

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/rtos-sim --input examples/sample_tasks.csv --duration 120 --policy rm
./build/rtos-sim --input examples/sample_tasks.csv --duration 120 --policy edf
```

On Windows with MinGW:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
ctest --test-dir build --output-on-failure
.\build\rtos-sim.exe --input examples\sample_tasks.csv --duration 120 --policy rm
```

## Input Format

CSV columns:

```text
name,period_ms,execution_ms,deadline_ms
sensor_read,10,1,10
control_loop,20,4,20
telemetry,50,8,50
heartbeat,100,2,100
```

Each task releases a job every `period_ms`. Each job needs `execution_ms` of CPU time and must finish before `release_time + deadline_ms`.

## Example Output

```text
Policy: Rate Monotonic
Duration: 120 ms
Task count: 4
CPU utilization: 53.33%
Context switches: 15
Preemptions: 2
Released jobs: 23
Completed jobs: 23
Deadline misses: 0
Unfinished jobs: 0
Verdict: PASS
```

Timeline excerpt:

```text
[0, 1) sensor_read#0
[1, 5) control_loop#0
[5, 10) telemetry#0
[10, 11) sensor_read#1
[11, 14) telemetry#0
[14, 16) heartbeat#0
```

## Scheduling Policies

Rate Monotonic Scheduling (RMS):

- fixed-priority scheduling
- shorter period means higher priority
- common in embedded/RTOS systems

Earliest Deadline First (EDF):

- dynamic-priority scheduling
- job with closest absolute deadline runs first
- often achieves higher CPU utilization in theory

## Resume Bullet

Built a C++ RTOS scheduler simulator that models periodic embedded tasks, preemption, context switches, CPU utilization, and deadline misses using Rate Monotonic and Earliest Deadline First scheduling with unit tests and CI.

## Interview Pitch

I built a C++ simulator for real-time operating system scheduling. It loads periodic task sets from CSV, simulates preemptive scheduling at millisecond resolution, and reports whether each job meets its deadline. I implemented both Rate Monotonic and Earliest Deadline First policies, added unit tests, and documented how the simulator relates to embedded systems and firmware validation.

## Roadmap

- Add JSON export for timelines.
- Add ASCII Gantt chart output.
- Add non-periodic interrupt service routines.
- Add priority inversion and mutex simulation.
- Add response-time analysis for RMS.
