#pragma once

#include <string>
#include <vector>

namespace rtos {

enum class SchedulingPolicy {
    RateMonotonic,
    EarliestDeadlineFirst
};

struct Task {
    std::string name;
    int period_ms = 0;
    int execution_ms = 0;
    int deadline_ms = 0;
};

struct JobResult {
    std::string task_name;
    int job_id = 0;
    int release_time_ms = 0;
    int deadline_time_ms = 0;
    int completion_time_ms = -1;
    int remaining_ms = 0;
    bool missed_deadline = false;
};

struct TimelineSegment {
    int start_ms = 0;
    int end_ms = 0;
    std::string label;
};

struct SimulationResult {
    SchedulingPolicy policy = SchedulingPolicy::RateMonotonic;
    int duration_ms = 0;
    int context_switches = 0;
    int preemptions = 0;
    int idle_ms = 0;
    double utilization = 0.0;
    std::vector<JobResult> jobs;
    std::vector<TimelineSegment> timeline;

    int released_jobs() const;
    int completed_jobs() const;
    int unfinished_jobs() const;
    int deadline_misses() const;
    bool passed() const;
};

class Scheduler {
public:
    static SimulationResult simulate(
        const std::vector<Task>& tasks,
        int duration_ms,
        SchedulingPolicy policy
    );
};

std::string policy_name(SchedulingPolicy policy);

} // namespace rtos
