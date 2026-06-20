# RTOS Concepts

## Periodic Task

A periodic task runs repeatedly at a fixed interval. For example, a sensor task with a 10 ms period releases one job at 0 ms, 10 ms, 20 ms, and so on.

## Execution Time

Execution time is how much CPU time one job needs. A task with 4 ms execution time must receive 4 ms of CPU time before it is complete.

## Deadline

The deadline is the latest time a job should finish. If a task has a 20 ms period and a 20 ms relative deadline, a job released at 40 ms must finish by 60 ms.

## Preemption

Preemption means the scheduler can pause one job and run another job. This is common in real-time systems because urgent tasks should not wait behind less urgent work.

## Rate Monotonic Scheduling

Rate Monotonic Scheduling is a fixed-priority policy. Tasks with shorter periods receive higher priority. It is simple and common in embedded systems.

## Earliest Deadline First

Earliest Deadline First is a dynamic-priority policy. At each scheduling decision, the job with the closest absolute deadline runs first.

## CPU Utilization

CPU utilization is the fraction of time the CPU is busy. A task set with too much utilization may miss deadlines, especially under fixed-priority scheduling.
