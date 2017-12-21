#
# Copyright 2017, Intel Corporation
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#
#     * Neither the name of the copyright holder nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

function(set_source_groups FILTER_PREFIX)
	foreach(file ${ARGN})
		file(RELATIVE_PATH relativePath ${DIR} ${file})
		get_filename_component(dirName ${relativePath} DIRECTORY)
		if (NOT ("${dirName}" STREQUAL ""))
			string(REPLACE "/" "\\" windowsFilterPath ${dirName})
			source_group("${FILTER_PREFIX}\\${windowsFilterPath}" FILES ${file})
		endif()
	endforeach()
endfunction(set_source_groups)

function(powershell_download_file URL PATH)
	execute_process(COMMAND powershell ".\\powershell_download_file.ps1 -URL '${URL}' -PATH '${PATH}'" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/etc/scripts")
endfunction()

function(download_gtest)
	include(ExternalProject)
	set(GTEST_VERSION 1.8.0)

	# CMake uses curl to download files, however on Windows systems HTTP_PROXY and similar environment variables
	# are not set, which means that it will fail when we are behind a proxy
	# Enable Multiprocess build with Visual Studio
	# Also we need to set gtest's CMake variable, so it will use /MD flag to build the library
	# On Linux we need to pass build type to CMake
	if (WIN32)
		set(FORCE_SHARED_CRT_WINDOWS "-Dgtest_force_shared_crt=ON")
		set(BUILD_FLAGS_WINDOWS "-DCMAKE_CXX_FLAGS=/MP")
		powershell_download_file("https://github.com/google/googletest/archive/release-${GTEST_VERSION}.zip"
			"${CMAKE_SOURCE_DIR}\\ext\\gtest\\googletest-${GTEST_VERSION}.zip")
	else ()
		set(BUILD_TYPE_LINUX "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
	endif ()

	if (EXISTS ${CMAKE_SOURCE_DIR}/ext/gtest/googletest-${GTEST_VERSION}.zip)
		set(GTEST_URL ${CMAKE_SOURCE_DIR}/ext/gtest/googletest-${GTEST_VERSION}.zip)
	else ()
		set(GTEST_URL https://github.com/google/googletest/archive/release-${GTEST_VERSION}.zip)
	endif ()

	ExternalProject_Add(
		gtest
		URL ${GTEST_URL}
		URL_HASH SHA256=f3ed3b58511efd272eb074a3a6d6fb79d7c2e6a0e374323d1e6bcbcc1ef141bf
		DOWNLOAD_NAME googletest-${GTEST_VERSION}.zip
		DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/ext/gtest
		PREFIX ${CMAKE_CURRENT_BINARY_DIR}/ext/gtest
		INSTALL_COMMAND ""
		CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} ${FORCE_SHARED_CRT_WINDOWS} ${BUILD_FLAGS_WINDOWS} ${BUILD_TYPE_LINUX}
	)
	ExternalProject_Get_Property(gtest source_dir binary_dir)
	add_library(libgtest IMPORTED STATIC GLOBAL)
	add_dependencies(libgtest gtest)
	if (WIN32)
		set_target_properties(libgtest PROPERTIES
			"IMPORTED_LOCATION_DEBUG" "${CMAKE_CURRENT_BINARY_DIR}/ext/gtest/src/gtest-build/googlemock/gtest/Debug/gtest.lib"
			"IMPORTED_LOCATION_RELEASE" "${CMAKE_CURRENT_BINARY_DIR}/ext/gtest/src/gtest-build/googlemock/gtest/Release/gtest.lib"
			"IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
		)
	else ()
		set_target_properties(libgtest PROPERTIES
			"IMPORTED_LOCATION" "${CMAKE_CURRENT_BINARY_DIR}/ext/gtest/src/gtest-build/googlemock/gtest/libgtest.a"
			"IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
		)
	endif ()

	include_directories(SYSTEM "${CMAKE_CURRENT_BINARY_DIR}/ext/gtest/src/gtest/googletest/include")
endfunction()

function(download_pugixml)
	include(ExternalProject)
	set(PUGIXML_VERSION 1.8.1)

	# CMake uses curl to download files, however on Windows systems HTTP_PROXY and similar environment variables
	# are not set, which means that it will fail when we are behind a proxy
	# Enable Multiprocess build with Visual Studio
	# Enable exception-handling in pugiXML
	# On Linux we need to pass build type to CMake
	if (WIN32)
		set(BUILD_FLAGS_WINDOWS "-DCMAKE_CXX_FLAGS=/MP /EHsc")
		powershell_download_file("https://github.com/zeux/pugixml/releases/download/v1.8.1/pugixml-${PUGIXML_VERSION}.tar.gz"
			"${CMAKE_SOURCE_DIR}\\ext\\pugixml\\pugixml-${PUGIXML_VERSION}.tar.gz")
	else ()
		set(BUILD_TYPE_LINUX "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
	endif ()

	if (EXISTS ${CMAKE_SOURCE_DIR}/ext/pugixml/pugixml-${PUGIXML_VERSION}.tar.gz)
		set(PUGIXML_URL ${CMAKE_SOURCE_DIR}/ext/pugixml/pugixml-${PUGIXML_VERSION}.tar.gz)
	else ()
		set(PUGIXML_URL https://github.com/zeux/pugixml/releases/download/v1.8.1/pugixml-${PUGIXML_VERSION}.tar.gz)
	endif ()

	ExternalProject_Add(
		pugixml
		URL ${PUGIXML_URL}
		URL_HASH SHA256=00d974a1308e85ca0677a981adc1b2855cb060923181053fb0abf4e2f37b8f39
		DOWNLOAD_NAME pugixml-${PUGIXML_VERSION}.tar.gz
		DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/ext/pugixml
		PREFIX ${CMAKE_CURRENT_BINARY_DIR}/ext/pugixml
		INSTALL_COMMAND ""
		CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} ${BUILD_FLAGS_WINDOWS} ${BUILD_TYPE_LINUX}
	)
	ExternalProject_Get_Property(pugixml source_dir binary_dir)
	add_library(libpugixml IMPORTED STATIC GLOBAL)
	add_dependencies(libpugixml pugixml)
	if (WIN32)
		set_target_properties(libpugixml PROPERTIES
			"IMPORTED_LOCATION_DEBUG" "${CMAKE_CURRENT_BINARY_DIR}/ext/pugixml/src/pugixml-build/Debug/pugixml.lib"
			"IMPORTED_LOCATION_RELEASE" "${CMAKE_CURRENT_BINARY_DIR}/ext/pugixml/src/pugixml-build/Release/pugixml.lib"
			"IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
		)
	else ()
		set_target_properties(libpugixml PROPERTIES
			"IMPORTED_LOCATION" "${CMAKE_CURRENT_BINARY_DIR}/ext/pugixml/src/pugixml-build/libpugixml.a"
			"IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
		)
	endif ()

	include_directories(SYSTEM "${CMAKE_CURRENT_BINARY_DIR}/ext/pugixml/src/pugixml/src")
endfunction()
