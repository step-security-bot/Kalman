#[[ __          _      __  __          _   _
| |/ /    /\   | |    |  \/  |   /\   | \ | |
| ' /    /  \  | |    | \  / |  /  \  |  \| |
|  <    / /\ \ | |    | |\/| | / /\ \ | . ` |
| . \  / ____ \| |____| |  | |/ ____ \| |\  |
|_|\_\/_/    \_\______|_|  |_/_/    \_\_| \_|

Kalman Filter
Version 0.3.0
https://github.com/FrancoisCarouge/Kalman

SPDX-License-Identifier: Unlicense

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org> ]]

include(CheckSourceCompiles)
include(FetchContent)

add_library(kalman_format INTERFACE)

check_source_compiles(
  CXX
  "
#include <format>
int main() {}
"
  CPP_LIB_FORMAT)

if(NOT CPP_LIB_FORMAT)
  FetchContent_Declare(
    fmt
    GIT_REPOSITORY "https://github.com/fmtlib/fmt"
    FIND_PACKAGE_ARGS NAMES fmt)
  FetchContent_MakeAvailable(fmt)

  target_sources(
    kalman_format
    INTERFACE FILE_SET
              "kalman_format_headers"
              TYPE
              "HEADERS"
              FILES
              "format"
              "print")
  target_link_libraries(kalman_format INTERFACE fmt::fmt)
endif()

install(
  TARGETS kalman_format
  EXPORT "kalman-target"
  FILE_SET "kalman_format_headers"
  DESTINATION "include/fcarouge")

add_library(kalman_generator INTERFACE)

check_source_compiles(
  CXX
  "
#include <generator>
int main() {}
"
  CPP_LIB_GENERATOR)

if(NOT CPP_LIB_GENERATOR)
  FetchContent_Declare(
    stdgenerator
    GIT_REPOSITORY "https://github.com/FrancoisCarouge/generator"
    FIND_PACKAGE_ARGS NAMES stdgenerator)
  FetchContent_MakeAvailable(stdgenerator)

  target_sources(kalman_generator INTERFACE FILE_SET "kalman_generator_headers"
                                            TYPE "HEADERS" FILES "generator")
  target_link_libraries(kalman_generator INTERFACE stdgenerator::stdgenerator)
  install(
    TARGETS kalman_generator
    EXPORT "kalman-target"
    FILE_SET "kalman_generator_headers"
    DESTINATION "include/fcarouge")
endif()
