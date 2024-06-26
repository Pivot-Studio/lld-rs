# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.5)
# Set the project name
project (lld-c)

# find llvm 18.0.x, if not, try find llvm 18.1.x

find_package(LLVM 18.0.0...18.1.0 CONFIG)

if (NOT LLVM_FOUND)
  find_package(LLVM 18.1.0...18.2.0 REQUIRED CONFIG)
endif()

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
include_directories(${LLVM_INCLUDE_DIRS})


if (WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

set(SOURCES
  lld-c.cpp
)
set(CMAKE_INSTALL_PREFIX .)


add_library(lld-c STATIC lld-c.cpp)
install(TARGETS lld-c
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)

if (NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()




llvm_map_components_to_libnames(
    llvm_libs
    Core
)

message(STATUS "LLVM_LIBS: ${llvm_libs}")

target_link_libraries(lld-c
  lldCommon
  lldCOFF
  lldELF
  lldMachO
  lldMinGW
  lldWasm
  ${llvm_libs}
)



