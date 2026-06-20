#include "rtos/Scheduler.hpp"
#include "rtos/TaskLoader.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_schedulable_task_set_passes_rm() {
    const std::vector<rtos::Task> tasks{
        {"sensor_read", 10, 1, 10},
        {"control_loop", 20, 4, 20},
        {"telemetry", 50, 8, 50},
        {"heartbeat", 100, 2, 100}
    };

    const rtos::SimulationResult result = rtos::Scheduler::simulate(
        tasks,
        100,
        rtos::SchedulingPolicy::RateMonotonic
    );

    require(result.passed(), "schedulable RM task set should pass");
    require(result.deadline_misses() == 0, "schedulable RM task set should not miss deadlines");
    require(result.completed_jobs() == result.released_jobs(), "all released jobs should complete");
}

void test_overloaded_task_set_fails() {
    const std::vector<rtos::Task> tasks{
        {"fast_control", 10, 8, 10},
        {"telemetry", 20, 8, 20}
    };

    const rtos::SimulationResult result = rtos::Scheduler::simulate(
        tasks,
        60,
        rtos::SchedulingPolicy::RateMonotonic
    );

    require(!result.passed(), "overloaded task set should fail");
    require(result.deadline_misses() > 0, "overloaded task set should miss deadlines");
}

void test_edf_completes_less_strict_task_set() {
    const std::vector<rtos::Task> tasks{
        {"task_a", 20, 7, 20},
        {"task_b", 50, 15, 50},
        {"task_c", 100, 20, 100}
    };

    const rtos::SimulationResult result = rtos::Scheduler::simulate(
        tasks,
        100,
        rtos::SchedulingPolicy::EarliestDeadlineFirst
    );

    require(result.deadline_misses() == 0, "EDF should meet this task set's deadlines");
    require(result.utilization > 0.0, "utilization should be positive");
}

void test_policy_names_are_readable() {
    require(
        rtos::policy_name(rtos::SchedulingPolicy::RateMonotonic) == "Rate Monotonic",
        "RM policy name should be readable"
    );
    require(
        rtos::policy_name(rtos::SchedulingPolicy::EarliestDeadlineFirst) == "Earliest Deadline First",
        "EDF policy name should be readable"
    );
}

} // namespace

int main() {
    try {
        test_schedulable_task_set_passes_rm();
        test_overloaded_task_set_fails();
        test_edf_completes_less_strict_task_set();
        test_policy_names_are_readable();
    } catch (const std::exception& error) {
        std::cerr << "Test failed: " << error.what() << '\n';
        return 1;
    }

    std::cout << "All scheduler tests passed.\n";
    return 0;
}
