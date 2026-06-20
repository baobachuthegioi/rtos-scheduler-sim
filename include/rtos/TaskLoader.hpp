#pragma once

#include "rtos/Scheduler.hpp"

#include <string>
#include <vector>

namespace rtos {

std::vector<Task> load_tasks_from_csv(const std::string& path);
void validate_tasks(const std::vector<Task>& tasks);

} // namespace rtos
