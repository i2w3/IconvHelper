cmake_minimum_required(VERSION 3.15)
project(iconvhelper LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate the version header from the template
configure_file(include/version.h.in version.h)

# Compiler-specific settings for MSVC
if(MSVC)
    # Use static runtime library (/MT or /MTd), requires CMake 3.15+
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # Set source file encoding to UTF-8 for the compiler
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

message(STATUS ">> CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# Find the required Iconv library package
find_package(Iconv REQUIRED)
message(STATUS ">> Iconv_INCLUDE_DIRS: ${Iconv_INCLUDE_DIRS}")
message(STATUS ">> Iconv_LIBRARIES: ${Iconv_LIBRARIES}")
message(STATUS ">> Iconv_VERSION: ${Iconv_VERSION}")

add_library(${PROJECT_NAME} src/IconvHelper.cpp)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        # For public headers like IconvHelper.hpp
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # For the generated version.h
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
        # For the libiconv headers, which are included by our public header
        ${Iconv_INCLUDE_DIRS}
    PRIVATE
        # No private include directories needed for now
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${Iconv_LIBRARIES}
)


add_executable(demo src/main.cpp src/gbk_str.cpp)
target_link_libraries(demo PRIVATE ${PROJECT_NAME})

add_executable(test EXCLUDE_FROM_ALL src/test.cpp src/gbk_str.cpp)
target_link_libraries(test PRIVATE ${PROJECT_NAME})