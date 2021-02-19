cmake_minimum_required(VERSION 3.10)
project(dimanche)

# set common variables
get_filename_component(DIMANCHE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
get_filename_component(DIMANCHE_ROOT "${DIMANCHE_ROOT}" DIRECTORY)
string(TOLOWER ${CMAKE_SYSTEM_NAME} System)
set(Platform ${CMAKE_SYSTEM_PROCESSOR})
if (${Platform} STREQUAL "AMD64")
  set(Platform x86_64)
endif()
if (CMAKE_BUILD_TYPE)
  string(TOLOWER ${CMAKE_BUILD_TYPE} Configuration)
else()
  set(Configuration debug)
endif()

message(STATUS "System: " ${System})
message(STATUS "Platform: " ${Platform})
message(STATUS "Configuration: " ${Configuration})
message(STATUS "DIMANCHE root: " ${DIMANCHE_ROOT})

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

function(DECL_TARGET name type)
  set(ProjectName di_${name})

  add_library(${ProjectName} ${type})
  set_target_properties(${ProjectName} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${DIMANCHE_ROOT}/_bin/${System}/cmake/${Platform}/${Configuration}
    LIBRARY_OUTPUT_DIRECTORY ${DIMANCHE_ROOT}/_bin/${System}/cmake/${Platform}/${Configuration}
    RUNTIME_OUTPUT_DIRECTORY ${DIMANCHE_ROOT}/_bin/${System}/cmake/${Platform}/${Configuration}
  )
  target_include_directories(${ProjectName}
    PUBLIC ${DIMANCHE_ROOT}
  )
  if (${Platform} MATCHES "x86.*")
    file(GLOB ASM_SOURCES ${DIMANCHE_ROOT}/dimanche/${name}/src/**.x86.asm) 
  elseif()
    file(GLOB ASM_SOURCES ${DIMANCHE_ROOT}/dimanche/${name}/src/**.arm.asm) 
  endif()
  file(GLOB C_SOURCES ${DIMANCHE_ROOT}/dimanche/${name}/src/**.c) 
  file(GLOB CPP_SOURCES ${DIMANCHE_ROOT}/dimanche/${name}/src/**.cpp) 
  target_sources(${ProjectName}
    PUBLIC ${C_SOURCES} ${CPP_SOURCES}
  )

  foreach(dep ${ARGN})
    add_dependencies(${ProjectName} di_${dep})
  endforeach()

  unset(ProjectName)
endfunction()

DECL_TARGET(basic STATIC)
DECL_TARGET(image_processing STATIC basic)
DECL_TARGET(io STATIC basic)
DECL_TARGET(system STATIC basic)
