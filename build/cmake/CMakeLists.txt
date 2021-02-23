cmake_minimum_required(VERSION 3.15)

include(msvc.cmake)

project(dimanche)

# set common variables
string(TOLOWER ${CMAKE_HOST_SYSTEM_NAME} HostSystem)
string(TOLOWER ${CMAKE_SYSTEM_NAME} System)
set(Platform ${CMAKE_SYSTEM_PROCESSOR})
if((${Platform} STREQUAL "AMD64") OR (${Platform} STREQUAL "x86_64"))
  set(Platform x64)
endif()
if(CMAKE_BUILD_TYPE)
  string(TOLOWER ${CMAKE_BUILD_TYPE} Configuration)
else()
  set(Configuration debug)
endif()
get_filename_component(DIMANCHE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
get_filename_component(DIMANCHE_ROOT "${DIMANCHE_ROOT}" DIRECTORY)


message(STATUS "Host system: " ${HostSystem})
message(STATUS "Target system: " ${System}) # windows/darwin
message(STATUS "Target platform: " ${Platform}) # x86/x86_64
message(STATUS "Target configuration: " ${Configuration}) # debug/release
message(STATUS "DIMANCHE root: " ${DIMANCHE_ROOT})


# specify tools
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if(MSVC)
  SET_MSVC_GLOBAL_SETTINGS(${Configuration})
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()
#enable_language(ASM_NASM)
#if(CMAKE_ASM_NASM_COMPILER_LOADED)
#  set(CAN_USE_ASSEMBLER TRUE)
#else()
#  message(FATAL_ERROR "NASM is required. Please, download the latest NASM and make sure it can be found by CMake")
#endif()
set(CMAKE_SUPPRESS_REGENERATION true)


# cross-platform variables
if(${HostSystem} STREQUAL "windows")
  set(OBJ_EXT "obj")
  set(nasm_cmd_file "${DIMANCHE_ROOT}/tools/${HostSystem}/build_asm.cmd")
else()
  set(OBJ_EXT "o")
  set(nasm_cmd_file "${DIMANCHE_ROOT}/tools/${HostSystem}/build_asm.sh")
endif()


function(COMPILE_ASM ProjectName OBJ_DIR)

  # all .asm files come as ARGN

  foreach(ASM_FILE ${ARGN})
    get_filename_component(ASM_FILE_NAME "${ASM_FILE}" NAME_WLE)
    set(ASM_OBJ ${OBJ_DIR}${ASM_FILE_NAME}.${OBJ_EXT}) 
    
    set_source_files_properties(${ASM_FILE} LANGUAGE ASM)
    add_custom_command(OUTPUT ${ASM_OBJ}
      COMMAND ${nasm_cmd_file}
      ARGS ${DIMANCHE_ROOT} ${Platform} ${ASM_FILE} ${OBJ_DIR}
      MAIN_DEPENDENCY ${ASM_FILE}
    )
    target_link_libraries(${ProjectName}
      ${ASM_OBJ}
    )
  endforeach()

endfunction()


function(DECL_TARGET name type)
  message(STATUS "Processing ${name}")
  set(ProjectName di_${name})
  set(ProjectRoot "${DIMANCHE_ROOT}/dimanche/${name}/")
  set(BASE_OUTPUT_DIRECTORY "${DIMANCHE_ROOT}/_bin/cmake/${System}/${Platform}")

  # set library properties
  add_library(${ProjectName} ${type})
  if(MSVC)
    SET_MSVC_PROJECT_SETTINGS(${ProjectName} ${Configuration})
    # MSVC adds ${Configuration} by itself to the end of OUTPUT_DIRECTORY
    set(OUTPUT_DIRECTORY "${BASE_OUTPUT_DIRECTORY}")
    set(INTERMEDIATE_DIRECTORY "${BASE_OUTPUT_DIRECTORY}/${Configuration}/${name}/")
  else()
    set(OUTPUT_DIRECTORY "${BASE_OUTPUT_DIRECTORY}/${Configuration}")
    set(INTERMEDIATE_DIRECTORY "${OUTPUT_DIRECTORY}/${name}/")
  endif()

  file(MAKE_DIRECTORY ${INTERMEDIATE_DIRECTORY})

  set_target_properties(${ProjectName} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}"
  )
  target_include_directories(${ProjectName}
    PUBLIC ${DIMANCHE_ROOT}
  )

  # parse sources
  if((${Platform} MATCHES "x86.*") OR (${Platform} MATCHES "x64"))
    file(GLOB_RECURSE ASM_SOURCES ${DIMANCHE_ROOT}/dimanche/${name}/src/**x86.asm)
  elseif(${Platform} MATCHES "arm.*")
    file(GLOB ASM_SOURCES ${DIMANCHE_ROOT}/dimanche/${name}/src/**.arm.asm) 
  endif()
  file(GLOB C_SOURCES ${ProjectRoot}/src/**.c) 
  file(GLOB CPP_SOURCES ${ProjectRoot}/src/**.cpp)
  file(GLOB INTERNAL_HEADERS ${ProjectRoot}/inc/*.h ${ProjectRoot}/inc/*.inc) 
  file(GLOB PUBLIC_HEADERS ${ProjectRoot}/*.h ${ProjectRoot}/*.inc) 
  target_sources(${ProjectName}
    PUBLIC ${ASM_SOURCES} ${C_SOURCES} ${CPP_SOURCES} ${INTERNAL_HEADERS} ${PUBLIC_HEADERS}
  )
  source_group(TREE ${ProjectRoot} FILES ${ASM_SOURCES} ${C_SOURCES} ${CPP_SOURCES} ${INTERNAL_HEADERS} ${PUBLIC_HEADERS})
  if(ASM_SOURCES)
    COMPILE_ASM(${ProjectName} ${INTERMEDIATE_DIRECTORY} ${ASM_SOURCES})
  endif()

  # set dependencies
  foreach(dep ${ARGN})
    add_dependencies(${ProjectName} di_${dep})
  endforeach()

  unset(INTERMEDIATE_DIRECTORY)
  unset(ProjectRoot)
  unset(ProjectName)
endfunction()

DECL_TARGET(basic STATIC)
DECL_TARGET(image_processing STATIC basic)
DECL_TARGET(io STATIC basic)
DECL_TARGET(system STATIC basic)
