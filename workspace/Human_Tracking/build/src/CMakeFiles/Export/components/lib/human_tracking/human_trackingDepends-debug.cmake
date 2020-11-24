#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "human_tracking" for configuration "Debug"
set_property(TARGET human_tracking APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(human_tracking PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/components/lib/human_tracking.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "RTC112_vc10d;coil112_vc10d;omniORB421_rtd;omniDynamic421_rtd;omnithread40_rtd;advapi32;ws2_32;mswsock"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/components/bin/human_tracking.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS human_tracking )
list(APPEND _IMPORT_CHECK_FILES_FOR_human_tracking "${_IMPORT_PREFIX}/components/lib/human_tracking.lib" "${_IMPORT_PREFIX}/components/bin/human_tracking.dll" )

# Import target "human_trackingcomp" for configuration "Debug"
set_property(TARGET human_trackingcomp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(human_trackingcomp PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/components/bin/human_trackingcomp.exe"
  )

list(APPEND _IMPORT_CHECK_TARGETS human_trackingcomp )
list(APPEND _IMPORT_CHECK_FILES_FOR_human_trackingcomp "${_IMPORT_PREFIX}/components/bin/human_trackingcomp.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
