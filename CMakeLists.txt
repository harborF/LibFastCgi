project(FastCgiLib)
cmake_minimum_required(VERSION 2.8)

#########################################################################################
# Root folders
set(ProjectRootDir ${${PROJECT_NAME}_SOURCE_DIR}/)
set(ProjectBinaryDir ${${PROJECT_NAME}_BINARY_DIR}/)
set(ACLRootDir /usr/local/protruly/acl)

# Source Subfolders
SET(EXECUTABLE_OUTPUT_PATH ${ProjectRootDir}/bin)
SET(LIBRARY_OUTPUT_PATH ${ProjectRootDir}/lib)

set(CMAKE_BUILD_TYPE "Debug")
if (CMAKE_BUILD_TYPE STREQUAL "RELEASE")
	add_definitions("-O3 -std=c++11")
else()
	add_definitions("-g -std=c++11")
endif()

# All following targets should search these directories for headers
include_directories( 
	${ProjectRootDir}
	${ProjectRootDir}/include
	${ACLRootDir}/dist/include
	${ACLRootDir}/dist/include/acl
)
LINK_DIRECTORIES(
	${ProjectRootDir}/lib
	${ACLRootDir}/dist/lib/linux64
)
#ADD_DEFINITIONS(-DHAS_MYSQL -DLINUX2)

aux_source_directory(./include/pugixml SRC_pugixml)
add_library("pugixml" SHARED ${SRC_pugixml})
SET_TARGET_PROPERTIES("pugixml" PROPERTIES LINKER_LANGUAGE CXX)

aux_source_directory(./fastcgi2-library SRC_library)
add_library("fastcgi2-library" SHARED ${SRC_library} )
SET_TARGET_PROPERTIES("fastcgi2-library" PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries("fastcgi2-library" crypto fcgi++ fcgi pugixml jsoncpp)

aux_source_directory(./request-cache SRC_request-cache)
add_library("fastcgi-request-cache" SHARED ${SRC_request-cache})
SET_TARGET_PROPERTIES("fastcgi-request-cache" PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries("fastcgi-request-cache"  "fastcgi2-library") 

aux_source_directory(./file-logger SRC_file-logger)
add_library("fastcgi-file-logger" SHARED ${SRC_file-logger})
SET_TARGET_PROPERTIES("fastcgi-file-logger" PROPERTIES LINKER_LANGUAGE CXX)

aux_source_directory(./syslog SRC_syslog)
add_library("fastcgi-syslog" SHARED ${SRC_syslog})
SET_TARGET_PROPERTIES("fastcgi-syslog" PROPERTIES LINKER_LANGUAGE CXX)

aux_source_directory(./fastcgi2-server SRC_server)
add_executable("fastcgi2-server" ${SRC_server})
SET_TARGET_PROPERTIES("fastcgi2-server" PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries("fastcgi2-server" crypto fcgi++ fcgi "fastcgi2-library" pthread)

aux_source_directory(./module-src SRC_work)
add_library("fastcgi2-module" SHARED ${SRC_work})
SET_TARGET_PROPERTIES("fastcgi2-module" PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries("fastcgi2-module" "fastcgi2-library" jsoncpp)
