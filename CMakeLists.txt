cmake_minimum_required(VERSION 3.16...3.24)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()


PROJECT(hi-happy-garden-parser VERSION "0.10.0" LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
add_compile_options("$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:-fno-rtti>")
add_compile_options("$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:-fexceptions>")

if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
  set(CMAKE_CXX17_STANDARD_COMPILE_OPTION "-std=c++17")
  set(CMAKE_C17_STANDARD_COMPILE_OPTION "-std=c17")
  set(CMAKE_C17_EXTENSION_COMPILE_OPTION "-std=gnu17")
elseif (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.1)
    message(error "GCC too old")
endif()

file(GLOB_RECURSE HI_HAPPY_GARDEN_PARSER_SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HI_HAPPY_GARDEN_PARSER_INCLUDES CONFIGURE_DEPENDS "inc/*.hpp" "inc/*.h")
file(GLOB_RECURSE HI_HAPPY_GARDEN_PARSER_TEST CONFIGURE_DEPENDS "test/*.cpp" "test/*.hpp")

message(STATUS "PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")

include(FetchContent)
message(STATUS "start googletest download")
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


include_directories(inc)
include_directories(osal/inc)
include_directories(osal/src/unix)

add_subdirectory(osal)

add_library(${PROJECT_NAME} STATIC ${HI_HAPPY_GARDEN_PARSER_SOURCES} ${HI_HAPPY_GARDEN_PARSER_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)


enable_testing()

add_executable("${PROJECT_NAME}_test" ${HI_HAPPY_GARDEN_PARSER_TEST})

target_link_libraries("${PROJECT_NAME}_test"
                        ${PROJECT_NAME}
                        osal
                        dl
                        GTest::gtest
                        GTest::gtest_main
                        )

add_test("${PROJECT_NAME}_gtest" "${PROJECT_NAME}_test")