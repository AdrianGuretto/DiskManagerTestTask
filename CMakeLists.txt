cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(RequestsStorageManager
        DESCRIPTION "A project designed to manage disk storage by processing user requests"
        LANGUAGES CXX
        VERSION 1.0
)

add_subdirectory(diskmanager)
