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

set (GoToFile_files
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
  ${SRC_DIR}/SettingsDialog.h
  ${SRC_DIR}/SettingsDialog.cpp
  ${SRC_DIR}/AboutDialog.h
  ${SRC_DIR}/AboutDialog.cpp
  ${SRC_DIR}/GotoFileManager.h
  ${SRC_DIR}/GotoFileManager.cpp
  ${SRC_DIR}/npp/EditorInterface.h
  ${SRC_DIR}/npp/NppInterface.h
  ${SRC_DIR}/npp/NppInterface.cpp
  ${SRC_DIR}/utils/bimap.h
  ${SRC_DIR}/utils/enum_array.h
  ${SRC_DIR}/utils/enum_range.h
  ${SRC_DIR}/utils/optional.h
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

set (GoToFileLib_files
   ${SRC_DIR}/DllMain.cpp
   ${SRC_DIR}/PluginInterface.h
   ${SRC_DIR}/GoToFile.rc
)

set (gtest_files
  ${SRC_DIR}/TestMain.cpp
  ${SRC_DIR}/gtest/gtest.h
  ${SRC_DIR}/gtest/gtest-all.cc
  ${SRC_DIR}/test/utf8string_test.cpp
  ${SRC_DIR}/test/GoToFileManager_test.cpp
  ${SRC_DIR}/test/TestEditorInterface.cpp
  ${SRC_DIR}/test/TestEditorInterface.h
  ${SRC_DIR}/PrecompiledHeader.h
)

SET(LINK_LIBRARY shlwapi comctl32 version wininet Ws2_32)
add_library (GoToFileLib STATIC ${GoToFile_files})
target_link_libraries (GoToFileLib ${LINK_LIBRARY})
target_link_libraries (GoToFileLib libiconv)
set_target_properties (GoToFileLib PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/PrecompiledHeader.h")
cotire(GoToFileLib)

add_library (GoToFile SHARED ${GoToFileLib_files})
target_link_libraries (GoToFile GoToFileLib)

add_executable (GoToFileTest ${gtest_files})
target_link_libraries (GoToFileTest GoToFileLib)
set_target_properties (GoToFileTest PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/PrecompiledHeader.h")
cotire(GoToFileTest)

if (DEFINED DEBUG_OUTPUT_PATH)
    set_target_properties (GoToFile PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${DEBUG_OUTPUT_PATH} )
endif (DEFINED DEBUG_OUTPUT_PATH)

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


