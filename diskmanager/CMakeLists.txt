add_library(RequestsStorageManager_core block_manager.cpp buffer_manager.cpp)

option(BUILD_TESTS OFF CACHE)

# Install DuckDB dependency
include(FetchContent)

# Determine the link for the duckDB downloading

set(BINARY_DOWNLOAD_LINK "")

if (MSVC)
    set(BINARY_DOWNLOAD_LINK "https://github.com/duckdb/duckdb/releases/download/v0.9.2/libduckdb-windows-amd64.zip")
elseif(APPLE)
    set(BINARY_DOWNLOAD_LINK "https://github.com/duckdb/duckdb/releases/download/v0.9.2/libduckdb-osx-universal.zip")
elseif(UNIX AND NOT APPLE)
    set(BINARY_DOWNLOAD_LINK "https://github.com/duckdb/duckdb/releases/download/v0.9.2/libduckdb-linux-aarch64.zip")
else()
    message(FATAL_ERROR "You are using an unsupported platform.")
endif()

FetchContent_Declare(
    duckdb
    URL ${BINARY_DOWNLOAD_LINK}
)
FetchContent_MakeAvailable(duckdb)
link_directories(${duckdb_SOURCE_DIR})

add_executable(StorageManager main.cpp)
target_link_libraries(StorageManager PRIVATE RequestsStorageManager_core duckdb)

# Install GoogleTest

if (BUILD_TESTS)
    FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(StorageManagerTests tests_runner.cpp buffer_manager.test.cpp block_manager.test.cpp)
    target_link_libraries(StorageManagerTests GTest::gtest_main GTest::gmock_main RequestsStorageManager_core duckdb)

    include(GoogleTest)
    gtest_discover_tests(StorageManagerTests)
endif()

# Create executable and link the installed modules
