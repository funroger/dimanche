cmake_minimum_required(VERSION 3.10)
project(dimanche)

# set common variables
set(DIMANCHE_ROOT /Users/vcherepanov/git/dimanche)
string(TOLOWER ${CMAKE_SYSTEM_NAME} System)
set(Platform ${CMAKE_SYSTEM_PROCESSOR})
if (CMAKE_BUILD_TYPE)
  string(TOLOWER ${CMAKE_BUILD_TYPE} Configuration)
else()
  set(Configuration debug)
endif()

message(STATUS "System: " ${System})
message(STATUS "Platform: " ${Platform})
message(STATUS "Configuration: " ${Configuration})

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

function(DECL_TARGET ProjectName type)
  add_library(${ProjectName} ${type})
  set_target_properties(${ProjectName} PROPERTIES
    OUTPUT_NAME di_${ProjectName}
  )
  set_target_properties(${ProjectName} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${DIMANCHE_ROOT}/_bin/${System}/cmake/${Platform}/${Configuration}
    LIBRARY_OUTPUT_DIRECTORY ${DIMANCHE_ROOT}/_bin/${System}/cmake/${Platform}/${Configuration}
    RUNTIME_OUTPUT_DIRECTORY ${DIMANCHE_ROOT}/_bin/${System}/cmake/${Platform}/${Configuration}
  )
  target_include_directories(${ProjectName}
    PUBLIC ${DIMANCHE_ROOT}
  )
  if (${Platform} MATCHES "x86.*")
    file(GLOB ASM_SOURCES ${DIMANCHE_ROOT}/dimanche/${ProjectName}/src/**.x86.asm) 
  elseif()
    file(GLOB ASM_SOURCES ${DIMANCHE_ROOT}/dimanche/${ProjectName}/src/**.arm.asm) 
  endif()
  file(GLOB C_SOURCES ${DIMANCHE_ROOT}/dimanche/${ProjectName}/src/**.c) 
  file(GLOB CPP_SOURCES ${DIMANCHE_ROOT}/dimanche/${ProjectName}/src/**.cpp) 
  target_sources(${ProjectName}
    PUBLIC ${C_SOURCES} ${CPP_SOURCES}
  )

  foreach(dep ${ARGN})
    add_dependencies(${ProjectName} ${dep})
  endforeach()
endfunction()

DECL_TARGET(basic STATIC)
DECL_TARGET(image_processing STATIC basic)
DECL_TARGET(io STATIC basic)
DECL_TARGET(system STATIC basic)
