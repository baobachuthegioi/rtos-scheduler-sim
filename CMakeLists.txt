cmake_minimum_required(VERSION 3.16)

project(rtos_scheduler_sim VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(rtos_scheduler
    src/Scheduler.cpp
    src/TaskLoader.cpp
)

target_include_directories(rtos_scheduler
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(rtos_scheduler PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

add_executable(rtos-sim src/main.cpp)
target_link_libraries(rtos-sim PRIVATE rtos_scheduler)

enable_testing()

add_executable(rtos-scheduler-tests tests/test_scheduler.cpp)
target_link_libraries(rtos-scheduler-tests PRIVATE rtos_scheduler)

add_test(NAME rtos_scheduler_tests COMMAND rtos-scheduler-tests)
