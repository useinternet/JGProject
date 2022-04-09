#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Zep::Zep" for configuration "Debug"
set_property(TARGET Zep::Zep APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Zep::Zep PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/Zep-debug.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS Zep::Zep )
list(APPEND _IMPORT_CHECK_FILES_FOR_Zep::Zep "${_IMPORT_PREFIX}/lib/Zep-debug.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
