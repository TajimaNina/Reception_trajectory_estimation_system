# Human_Tracking CMake config file
#
# This file sets the following variables:
# Human_Tracking_FOUND - Always TRUE.
# Human_Tracking_INCLUDE_DIRS - Directories containing the Human_Tracking include files.
# Human_Tracking_IDL_DIRS - Directories containing the Human_Tracking IDL files.
# Human_Tracking_LIBRARIES - Libraries needed to use Human_Tracking.
# Human_Tracking_DEFINITIONS - Compiler flags for Human_Tracking.
# Human_Tracking_VERSION - The version of Human_Tracking found.
# Human_Tracking_VERSION_MAJOR - The major version of Human_Tracking found.
# Human_Tracking_VERSION_MINOR - The minor version of Human_Tracking found.
# Human_Tracking_VERSION_REVISION - The revision version of Human_Tracking found.
# Human_Tracking_VERSION_CANDIDATE - The candidate version of Human_Tracking found.

message(STATUS "Found Human_Tracking-1.0.0")
set(Human_Tracking_FOUND TRUE)

find_package(<dependency> REQUIRED)

#set(Human_Tracking_INCLUDE_DIRS
#    "C:/Program Files (x86)/Human_Tracking/include/human_tracking-1"
#    ${<dependency>_INCLUDE_DIRS}
#    )
#
#set(Human_Tracking_IDL_DIRS
#    "C:/Program Files (x86)/Human_Tracking/include/human_tracking-1/idl")
set(Human_Tracking_INCLUDE_DIRS
    "C:/Program Files (x86)/Human_Tracking/include/"
    ${<dependency>_INCLUDE_DIRS}
    )
set(Human_Tracking_IDL_DIRS
    "C:/Program Files (x86)/Human_Tracking/include//idl")


if(WIN32)
    set(Human_Tracking_LIBRARIES
        "C:/Program Files (x86)/Human_Tracking/components/lib/human_tracking.lib"
        ${<dependency>_LIBRARIES}
        )
else(WIN32)
    set(Human_Tracking_LIBRARIES
        "C:/Program Files (x86)/Human_Tracking/components/lib/human_tracking.dll"
        ${<dependency>_LIBRARIES}
        )
endif(WIN32)

set(Human_Tracking_DEFINITIONS ${<dependency>_DEFINITIONS})

set(Human_Tracking_VERSION 1.0.0)
set(Human_Tracking_VERSION_MAJOR 1)
set(Human_Tracking_VERSION_MINOR 0)
set(Human_Tracking_VERSION_REVISION 0)
set(Human_Tracking_VERSION_CANDIDATE )

