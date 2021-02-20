
function(SET_MSVC_GLOBAL_SETTINGS)
  # reset configurations
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

  add_definitions(-DUNICODE -D_UNICODE)

  string(REGEX REPLACE "/W[1-3]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  add_compile_options(/W4 /WX)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /GS- /Ot /Oi /Zi" PARENT_SCOPE)
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


function(SET_MSVC_PROJECT_SETTINGS ProjectName)

endfunction()