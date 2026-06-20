#include "rtos/TaskLoader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace rtos {
namespace {

std::string trim(const std::string& value) {
    const std::string whitespace = " \t\r\n";
    const std::size_t start = value.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    const std::size_t end = value.find_last_not_of(whitespace);
    return value.substr(start, end - start + 1);
}

std::vector<std::string> split_csv_line(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;

    while (std::getline(stream, field, ',')) {
        fields.push_back(trim(field));
    }

    return fields;
}

int parse_positive_int(const std::string& value, const std::string& field_name, int line_number) {
    try {
        const int parsed = std::stoi(value);
        if (parsed <= 0) {
            throw std::invalid_argument("not positive");
        }
        return parsed;
    } catch (const std::exception&) {
        throw std::runtime_error(
            "Invalid " + field_name + " at line " + std::to_string(line_number) + ": " + value
        );
    }
}

} // namespace

std::vector<Task> load_tasks_from_csv(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open task CSV: " + path);
    }

    std::vector<Task> tasks;
    std::string line;
    int line_number = 0;

    while (std::getline(file, line)) {
        line_number += 1;
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (line_number == 1 && line.find("name") != std::string::npos) {
            continue;
        }

        const std::vector<std::string> fields = split_csv_line(line);
        if (fields.size() != 4) {
            throw std::runtime_error(
                "Expected 4 CSV fields at line " + std::to_string(line_number)
            );
        }

        tasks.push_back(Task{
            fields[0],
            parse_positive_int(fields[1], "period_ms", line_number),
            parse_positive_int(fields[2], "execution_ms", line_number),
            parse_positive_int(fields[3], "deadline_ms", line_number)
        });
    }

    validate_tasks(tasks);
    return tasks;
}

void validate_tasks(const std::vector<Task>& tasks) {
    if (tasks.empty()) {
        throw std::runtime_error("Task set cannot be empty");
    }

    for (const Task& task : tasks) {
        if (task.name.empty()) {
            throw std::runtime_error("Task name cannot be empty");
        }
        if (task.period_ms <= 0 || task.execution_ms <= 0 || task.deadline_ms <= 0) {
            throw std::runtime_error("Task timing values must be positive");
        }
        if (task.execution_ms > task.deadline_ms) {
            throw std::runtime_error(
                "Task " + task.name + " has execution time greater than deadline"
            );
        }
    }
}

} // namespace rtos
