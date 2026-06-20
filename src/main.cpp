#include "rtos/Scheduler.hpp"
#include "rtos/TaskLoader.hpp"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

struct CliOptions {
    std::string input_path = "examples/sample_tasks.csv";
    int duration_ms = 120;
    rtos::SchedulingPolicy policy = rtos::SchedulingPolicy::RateMonotonic;
    bool show_help = false;
};

void print_help() {
    std::cout
        << "RTOS Scheduler Simulator\n\n"
        << "Usage:\n"
        << "  rtos-sim --input examples/sample_tasks.csv --duration 120 --policy rm\n\n"
        << "Options:\n"
        << "  --input <path>       CSV file with task definitions\n"
        << "  --duration <ms>      Simulation length in milliseconds\n"
        << "  --policy <rm|edf>    Scheduling policy\n"
        << "  --help              Show this help text\n";
}

rtos::SchedulingPolicy parse_policy(const std::string& value) {
    if (value == "rm" || value == "rms" || value == "rate-monotonic") {
        return rtos::SchedulingPolicy::RateMonotonic;
    }
    if (value == "edf" || value == "earliest-deadline-first") {
        return rtos::SchedulingPolicy::EarliestDeadlineFirst;
    }
    throw std::runtime_error("Unknown policy: " + value);
}

CliOptions parse_args(int argc, char** argv) {
    CliOptions options;

    for (int index = 1; index < argc; ++index) {
        const std::string arg = argv[index];
        auto require_value = [&](const std::string& option_name) -> std::string {
            if (index + 1 >= argc) {
                throw std::runtime_error("Missing value for " + option_name);
            }
            return argv[++index];
        };

        if (arg == "--help" || arg == "-h") {
            options.show_help = true;
        } else if (arg == "--input") {
            options.input_path = require_value(arg);
        } else if (arg == "--duration") {
            options.duration_ms = std::stoi(require_value(arg));
        } else if (arg == "--policy") {
            options.policy = parse_policy(require_value(arg));
        } else {
            throw std::runtime_error("Unknown argument: " + arg);
        }
    }

    return options;
}

void print_timeline(const rtos::SimulationResult& result) {
    std::cout << "\nTimeline:\n";
    for (const rtos::TimelineSegment& segment : result.timeline) {
        std::cout << "[" << segment.start_ms << ", " << segment.end_ms << ") "
                  << segment.label << '\n';
    }
}

void print_deadline_misses(const rtos::SimulationResult& result) {
    if (result.deadline_misses() == 0) {
        return;
    }

    std::cout << "\nDeadline misses:\n";
    for (const rtos::JobResult& job : result.jobs) {
        if (!job.missed_deadline) {
            continue;
        }

        std::cout << "- " << job.task_name << "#" << job.job_id
                  << " released at " << job.release_time_ms << " ms"
                  << ", deadline " << job.deadline_time_ms << " ms";
        if (job.completion_time_ms >= 0) {
            std::cout << ", completed at " << job.completion_time_ms << " ms";
        } else {
            std::cout << ", unfinished with " << job.remaining_ms << " ms remaining";
        }
        std::cout << '\n';
    }
}

void print_summary(const rtos::SimulationResult& result, int task_count) {
    std::cout << "Policy: " << rtos::policy_name(result.policy) << '\n';
    std::cout << "Duration: " << result.duration_ms << " ms\n";
    std::cout << "Task count: " << task_count << '\n';
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "CPU utilization: " << (result.utilization * 100.0) << "%\n";
    std::cout << "Context switches: " << result.context_switches << '\n';
    std::cout << "Preemptions: " << result.preemptions << '\n';
    std::cout << "Idle time: " << result.idle_ms << " ms\n";
    std::cout << "Released jobs: " << result.released_jobs() << '\n';
    std::cout << "Completed jobs: " << result.completed_jobs() << '\n';
    std::cout << "Deadline misses: " << result.deadline_misses() << '\n';
    std::cout << "Unfinished jobs: " << result.unfinished_jobs() << '\n';
    std::cout << "Verdict: " << (result.passed() ? "PASS" : "FAIL") << '\n';
}

} // namespace

int main(int argc, char** argv) {
    try {
        const CliOptions options = parse_args(argc, argv);
        if (options.show_help) {
            print_help();
            return 0;
        }

        const std::vector<rtos::Task> tasks = rtos::load_tasks_from_csv(options.input_path);
        const rtos::SimulationResult result = rtos::Scheduler::simulate(
            tasks,
            options.duration_ms,
            options.policy
        );

        print_summary(result, static_cast<int>(tasks.size()));
        print_deadline_misses(result);
        print_timeline(result);
        return result.passed() ? 0 : 2;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
