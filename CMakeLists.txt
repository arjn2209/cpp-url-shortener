cmake_minimum_required(VERSION 3.15)
project(URLShortener)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Crow
add_subdirectory(Crow)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/Crow/include
    /opt/homebrew/include
)

# SQLite
find_package(SQLite3 REQUIRED)

add_executable(server
    src/main.cpp
    src/UrlService.cpp
)

target_link_libraries(server
    SQLite::SQLite3
)