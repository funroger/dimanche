
function(SET_MSVC_GLOBAL_SETTINGS Configuration)
  # reset configurations
  set(CMAKE_CONFIGURATION_TYPES "${Configuration}" CACHE STRING "" FORCE)

  add_definitions(-DUNICODE -D_UNICODE)

  if("${CMAKE_CXX_SIZEOF_DATA_PTR}" STREQUAL "8")
    string(REGEX REPLACE "/DWIN32" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  endif()
  string(REGEX REPLACE "/W[1-3]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  add_compile_options(/W4 /WX)
  string(REGEX REPLACE "/D_WINDOWS" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)

  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /GS- /Ot /Oi /Zi" PARENT_SCOPE)

  string(REGEX REPLACE "/Ob0" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_DEBUG" PARENT_SCOPE)

  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" PARENT_SCOPE)
endfunction()


function(PRINT_ALL_VARIABLES)
  get_cmake_property(_varNames VARIABLES)
  list(REMOVE_DUPLICATES _varNames)
  list(SORT _varNames)
  foreach(_varName ${_varNames})
    message(STATUS "${_varName}=${${_varName}}")
  endforeach()
endfunction()


function(SET_MSVC_PROJECT_SETTINGS ProjectName Configuration)

  # enable link time code generation
  if(${Configuration} STREQUAL "release")
    #check_ipo_supported(RESULT result OUTPUT output)
    #if(result)
      set_target_properties(${ProjectName} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION TRUE
      )
    #endif()
  endif()

endfunction()