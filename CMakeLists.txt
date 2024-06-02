cmake_minimum_required(VERSION 3.16...3.24)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()


PROJECT(hi-happy-garden-parser VERSION "1.0.0" LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
add_compile_options("$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:-fno-rtti>")
add_compile_options("$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:-fexceptions>")

#add_definitions(-DINCLUDE_HHG_PARSER_CONFIG)

if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
  set(CMAKE_CXX17_STANDARD_COMPILE_OPTION "-std=c++17")
  set(CMAKE_C17_STANDARD_COMPILE_OPTION "-std=c17")
  set(CMAKE_C17_EXTENSION_COMPILE_OPTION "-std=gnu17")
elseif (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.1)
    message(error "GCC too old")
endif()


set(FORMAT_CHAR %c)
set(FORMAT_SHORT %d)
set(FORMAT_INT %d)
set(FORMAT_LONG %ls)
set(FORMAT_USHORT %u)
set(FORMAT_UINT %u)
set(FORMAT_ULONG %lu)
set(ARGS_MAX 3)
configure_file(hhg-parser-config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config/hhg-parser-config.h)

file(GLOB_RECURSE HI_HAPPY_GARDEN_PARSER_SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HI_HAPPY_GARDEN_PARSER_INCLUDES CONFIGURE_DEPENDS "inc/*.hpp" "inc/*.h")
file(GLOB_RECURSE HI_HAPPY_GARDEN_PARSER_TEST CONFIGURE_DEPENDS "test/*.cpp" "test/*.hpp")

message(STATUS "PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")

if (PARSER_ENABLE_TEST)
    include(FetchContent)
    message(STATUS "start googletest download")
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()
endif()

include_directories(inc)
include_directories(config)

if (DEFINED PARSER_ENABLE_OSAL)
    include_directories(osal/inc)
    include_directories(osal/src/unix)
    add_subdirectory(osal)
endif ()

add_library(${PROJECT_NAME} STATIC ${HI_HAPPY_GARDEN_PARSER_SOURCES} ${HI_HAPPY_GARDEN_PARSER_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

if (PARSER_ENABLE_TEST)
    add_executable("${PROJECT_NAME}_test" ${HI_HAPPY_GARDEN_PARSER_TEST})

    target_link_libraries("${PROJECT_NAME}_test"
            ${PROJECT_NAME}
            osal
            dl
            GTest::gtest
            GTest::gtest_main
    )

    add_test("${PROJECT_NAME}_gtest" "${PROJECT_NAME}_test")
endif()
