cmake_minimum_required(VERSION 3.10)
project(HospitalManagementSystem VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_executable(HospitalApp ${SRC_FILES})

# Professional fix for paths with spaces and '&' symbols
# Instead of forcing the directory (which breaks shell commands), 
# we use a POST_BUILD copy command which is much more robust.
add_custom_command(TARGET HospitalApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:HospitalApp> "${CMAKE_SOURCE_DIR}/HospitalApp.exe"
    COMMENT "Copying executable to project root..."
)
