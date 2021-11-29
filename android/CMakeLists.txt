cmake_minimum_required(VERSION 3.10)

project("CBL_C_FD_Max" LANGUAGES "CXX")

find_package(CouchbaseLite
        REQUIRED
        NO_CMAKE_FIND_ROOT_PATH
        NO_DEFAULT_PATH
        PATHS vendor/libcblite-3.0.0
)

find_library(log-lib log)

add_library(native-lib SHARED
    app/src/main/cpp/native-lib.cc
)

target_link_libraries(native-lib
    cblite ${log-lib}
)