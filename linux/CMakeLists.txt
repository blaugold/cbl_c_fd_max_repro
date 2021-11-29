cmake_minimum_required(VERSION 3.16)

project("CBL_C_FD_Max" LANGUAGES "CXX")

find_package("CouchbaseLite" REQUIRED PATHS "vendor/libcblite-3.0.0")

add_executable(main
    src/main.cc
)

target_link_libraries(main
    cblite
)