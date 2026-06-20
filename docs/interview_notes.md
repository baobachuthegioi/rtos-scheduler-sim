# Interview Notes

## Thirty-Second Pitch

I built a C++ RTOS scheduler simulator for periodic embedded tasks. It supports Rate Monotonic and Earliest Deadline First scheduling, simulates preemption at 1 ms resolution, and reports CPU utilization, context switches, preemptions, deadline misses, and unfinished jobs. I added a CSV task loader, CLI, unit tests, and GitHub Actions.

## Skills Demonstrated

- Modern C++17
- Real-time scheduling concepts
- Algorithms and priority selection
- Data modeling with structs/classes
- File parsing and command-line interfaces
- Unit testing without external dependencies
- CMake and CI

## Resume Bullet

Built a C++ RTOS scheduler simulator that models periodic embedded tasks, preemption, context switches, CPU utilization, and deadline misses using Rate Monotonic and Earliest Deadline First scheduling with unit tests and CI.

## Interview Talking Points

- Rate Monotonic uses fixed priorities where shorter periods get higher priority.
- EDF uses dynamic priorities based on the closest absolute deadline.
- Preemptive scheduling can interrupt a lower-priority job when a more urgent job is released.
- Deadline misses can happen even when code compiles correctly, so simulation helps validate timing before hardware testing.
- CMake and CI make the project easy for another engineer to build and verify.

## Follow-Up Features

- ASCII Gantt chart output.
- Response-time analysis for fixed-priority scheduling.
- Interrupt service routine simulation.
- Mutex and priority inversion simulation.
- Export timeline data as JSON.
