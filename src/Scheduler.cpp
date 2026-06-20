#include "rtos/Scheduler.hpp"

#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace rtos {
namespace {

struct ActiveJob {
    int task_index = 0;
    int job_id = 0;
    int release_time_ms = 0;
    int deadline_time_ms = 0;
    int remaining_ms = 0;
    int completion_time_ms = -1;
    bool missed_deadline = false;
};

std::string job_label(const Task& task, const ActiveJob& job) {
    return task.name + "#" + std::to_string(job.job_id);
}

bool is_ready(const ActiveJob& job) {
    return job.remaining_ms > 0;
}

int select_job(
    const std::vector<Task>& tasks,
    const std::vector<ActiveJob>& jobs,
    SchedulingPolicy policy
) {
    int selected = -1;

    for (int index = 0; index < static_cast<int>(jobs.size()); ++index) {
        if (!is_ready(jobs[index])) {
            continue;
        }

        if (selected == -1) {
            selected = index;
            continue;
        }

        const ActiveJob& candidate = jobs[index];
        const ActiveJob& current = jobs[selected];
        const Task& candidate_task = tasks[candidate.task_index];
        const Task& current_task = tasks[current.task_index];

        bool candidate_wins = false;
        if (policy == SchedulingPolicy::EarliestDeadlineFirst) {
            candidate_wins = candidate.deadline_time_ms < current.deadline_time_ms;
            if (candidate.deadline_time_ms == current.deadline_time_ms) {
                candidate_wins = candidate.release_time_ms < current.release_time_ms;
            }
        } else {
            candidate_wins = candidate_task.period_ms < current_task.period_ms;
            if (candidate_task.period_ms == current_task.period_ms) {
                candidate_wins = candidate.task_index < current.task_index;
            }
        }

        if (candidate_wins) {
            selected = index;
        }
    }

    return selected;
}

void append_segment(std::vector<TimelineSegment>& timeline, int time_ms, const std::string& label) {
    if (!timeline.empty() && timeline.back().label == label && timeline.back().end_ms == time_ms) {
        timeline.back().end_ms += 1;
        return;
    }

    timeline.push_back(TimelineSegment{time_ms, time_ms + 1, label});
}

} // namespace

int SimulationResult::released_jobs() const {
    return static_cast<int>(jobs.size());
}

int SimulationResult::completed_jobs() const {
    return static_cast<int>(std::count_if(jobs.begin(), jobs.end(), [](const JobResult& job) {
        return job.completion_time_ms >= 0;
    }));
}

int SimulationResult::unfinished_jobs() const {
    return static_cast<int>(std::count_if(jobs.begin(), jobs.end(), [](const JobResult& job) {
        return job.completion_time_ms < 0;
    }));
}

int SimulationResult::deadline_misses() const {
    return static_cast<int>(std::count_if(jobs.begin(), jobs.end(), [](const JobResult& job) {
        return job.missed_deadline;
    }));
}

bool SimulationResult::passed() const {
    return deadline_misses() == 0 && unfinished_jobs() == 0;
}

SimulationResult Scheduler::simulate(
    const std::vector<Task>& tasks,
    int duration_ms,
    SchedulingPolicy policy
) {
    if (duration_ms <= 0) {
        throw std::invalid_argument("duration_ms must be positive");
    }
    if (tasks.empty()) {
        throw std::invalid_argument("at least one task is required");
    }

    std::vector<ActiveJob> active_jobs;
    std::vector<int> next_job_id(tasks.size(), 0);
    SimulationResult result;
    result.policy = policy;
    result.duration_ms = duration_ms;

    int previous_job_index = -1;
    int busy_ms = 0;

    for (int time_ms = 0; time_ms < duration_ms; ++time_ms) {
        for (int task_index = 0; task_index < static_cast<int>(tasks.size()); ++task_index) {
            const Task& task = tasks[task_index];
            if (time_ms % task.period_ms == 0) {
                active_jobs.push_back(ActiveJob{
                    task_index,
                    next_job_id[task_index]++,
                    time_ms,
                    time_ms + task.deadline_ms,
                    task.execution_ms,
                    -1,
                    false
                });
            }
        }

        for (ActiveJob& job : active_jobs) {
            if (job.remaining_ms > 0 && time_ms >= job.deadline_time_ms) {
                job.missed_deadline = true;
            }
        }

        const int selected_job_index = select_job(tasks, active_jobs, policy);
        if (selected_job_index == -1) {
            append_segment(result.timeline, time_ms, "IDLE");
            result.idle_ms += 1;
            previous_job_index = -1;
            continue;
        }

        if (previous_job_index != -1 && previous_job_index != selected_job_index) {
            result.context_switches += 1;
            if (active_jobs[previous_job_index].remaining_ms > 0) {
                result.preemptions += 1;
            }
        }

        ActiveJob& running_job = active_jobs[selected_job_index];
        append_segment(
            result.timeline,
            time_ms,
            job_label(tasks[running_job.task_index], running_job)
        );

        running_job.remaining_ms -= 1;
        busy_ms += 1;

        if (running_job.remaining_ms == 0) {
            running_job.completion_time_ms = time_ms + 1;
            if (running_job.completion_time_ms > running_job.deadline_time_ms) {
                running_job.missed_deadline = true;
            }
        }

        previous_job_index = selected_job_index;
    }

    result.utilization = static_cast<double>(busy_ms) / static_cast<double>(duration_ms);

    result.jobs.reserve(active_jobs.size());
    for (const ActiveJob& job : active_jobs) {
        result.jobs.push_back(JobResult{
            tasks[job.task_index].name,
            job.job_id,
            job.release_time_ms,
            job.deadline_time_ms,
            job.completion_time_ms,
            job.remaining_ms,
            job.missed_deadline || (job.remaining_ms > 0 && job.deadline_time_ms <= duration_ms)
        });
    }

    return result;
}

std::string policy_name(SchedulingPolicy policy) {
    if (policy == SchedulingPolicy::EarliestDeadlineFirst) {
        return "Earliest Deadline First";
    }
    return "Rate Monotonic";
}

} // namespace rtos
