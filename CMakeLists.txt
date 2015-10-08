cmake_minimum_required (VERSION 2.8)

if (POLICY CMP0011)
	cmake_policy(SET CMP0011 NEW)
endif()

include(cotire.CMAKE)
project (goto_file)
set (goto_file_VERSION_MAJOR 1)
set (goto_file_VERSION_MINOR 0)
set (SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set (BIN_DIR ${PROJECT_SOURCE_DIR})

set (GENERIC_OUTPUT_DIR, ${BIN_DIR})
set (DEBUG_OUTPUT_DIR, ${BIN_DIR}/debug)
set (RELEASE_OUTPUT_DIR, ${BIN_DIR}/release)
add_definitions ("-DUSING_STATIC_LIBICONV -D_CRT_NONSTDC_NO_DEPRECATE -D_UNICODE -DUNICODE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NON_CONFORMING_SWPRINTFS -D_CRT_SECURE_NO_DEPRECATE -D_WIN32_WINNT=0x501 -DWINVER=0x501")

set(DEBUG_OUTPUT_PATH "" CACHE STRING "Output for debug library (Possibly notepad++ plugin folder)")
set(VLD_BUILD_ON "OFF" CACHE BOOL "Use Visual Leak Detector")

include_directories (${SRC_DIR})
include_directories (${SRC_DIR}/controls)
include_directories (libiconv/include)

add_subdirectory ("libiconv")

add_definitions ()

if ( VLD_BUILD_ON )
  add_definitions ("-DVLD_BUILD")
endif ( VLD_BUILD_ON )

if ( MSVC )
    #set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GENERIC_OUTPUT_DIR} )
    if (NOT DEFINED DEBUG_OUTPUT_PATH)
    	set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${DEBUG_OUTPUT_DIR} )
   	else (NOT DEFINED DEBUG_OUTPUT_PATH)
   		set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${DEBUG_OUTPUT_PATH} )
   	endif (NOT DEFINED DEBUG_OUTPUT_PATH)
    # set ( CMAKE_PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${RELEASE_OUTPUT_DIR} )
    set (add_definitions, UNICODE)
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG} )
	set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG} )
else ( MSVC )
# For possible MinGW compilation
	set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GENERIC_OUTPUT_DIR} )
	set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GENERIC_OUTPUT_DIR} )
	set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GENERIC_OUTPUT_DIR} )
endif ( MSVC )

set (list_of_files_goto_file
  ${SRC_DIR}/DllMain.cpp
  ${SRC_DIR}/Plugin.cpp
  ${SRC_DIR}/Plugin.h
  ${SRC_DIR}/PluginInterface.h
  ${SRC_DIR}/PrecompiledHeader.h
  ${SRC_DIR}/PrecompiledHeader.cpp
  ${SRC_DIR}/Notepad_plus_msgs.h
  ${SRC_DIR}/Scintilla.h
  ${SRC_DIR}/menuCmdID.h
  ${SRC_DIR}/Settings.h
  ${SRC_DIR}/Settings.cpp
  ${SRC_DIR}/NppInterface.h
  ${SRC_DIR}/NppInterface.cpp
  ${SRC_DIR}/SettingsDialog.h
  ${SRC_DIR}/SettingsDialog.cpp
  ${SRC_DIR}/AboutDialog.h
  ${SRC_DIR}/AboutDialog.cpp
  ${SRC_DIR}/GoToFile.rc
  ${SRC_DIR}/utils/bimap.h
  ${SRC_DIR}/utils/raii.h
  ${SRC_DIR}/utils/raii.cpp
  ${SRC_DIR}/utils/macro.h
  ${SRC_DIR}/utils/misc.h
  ${SRC_DIR}/utils/utf8string.h
  ${SRC_DIR}/utils/utf8string.cpp
  ${SRC_DIR}/utils/IniWorker.cpp
  ${SRC_DIR}/utils/IniWorker.h
  ${SRC_DIR}/utils/winapi.h
  ${SRC_DIR}/utils/winapi.cpp
  ${SRC_DIR}/controls/window.h
  ${SRC_DIR}/controls/StaticDialog/StaticDialog.h
  ${SRC_DIR}/controls/StaticDialog/StaticDialog.cpp
)

SET(LINK_LIBRARY shlwapi comctl32 version wininet Ws2_32)
add_library (goto_file SHARED ${list_of_files_goto_file})
target_link_libraries(goto_file ${LINK_LIBRARY})
target_link_libraries (goto_file debug ${PROJECT_BINARY_DIR}/libiconv/DebugStatic_Win32/libiconv.lib)
target_link_libraries (goto_file optimized ${PROJECT_BINARY_DIR}/libiconv/ReleaseStatic_Win32/libiconv.lib)
set_target_properties(goto_file PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/PrecompiledHeader.h")
cotire(goto_file)

SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SUBSYSTEM:CONSOLE,5.01")
SET (CMAKE_CXX_FLAGS "/WX /EHsc /MP /W4 /wd4127")
foreach(flag_var CMAKE_C_FLAGS_DEBUG CMAKE_CXX_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
endforeach(flag_var)
SET (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" CACHE STRING "MSVC C Debug MT flags " FORCE)
SET (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "MSVC CXX Debug MT flags " FORCE)
SET (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "MSVC C Release MT flags " FORCE)
SET (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "MSVC CXX Release MT flags " FORCE)
SET (CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}" CACHE STRING "MSVC C Debug MT flags " FORCE)
SET (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}" CACHE STRING "MSVC C Release MT flags " FORCE)
SET (CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}" CACHE STRING "MSVC CXX Debug MT flags " FORCE)
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING "MSVC CXX Release MT flags " FORCE)


