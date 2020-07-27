cmake_minimum_required(VERSION 3.16)

project(tickDirectionClient VERSION 1.0)

# Set in Root project?
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/../install )

if (APPLE)
	set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl/")
        
	message("Current Apple RPATH Setting ${CMAKE_MACOSX_RPATH}")

	# enable @rpath in the install name for any shared library being built
	# note: it is planned that a future version of CMake will enable this by default
	set(CMAKE_MACOSX_RPATH 1)

endif()

message("${PROJECT_NAME} CMAKE_CXX_STANDARD = ${CMAKE_CXX_STANDARD}")
message("${PROJECT_NAME} CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
message("${PROJECT_NAME} PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
message("${PROJECT_NAME} PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}")
message("${PROJECT_NAME} CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}")
message("${PROJECT_NAME} CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
if(Protobuf_FOUND)
	if(Protobuf_VERSION VERSION_LESS 3.5)
		message(FATAL_ERROR "Version of Protobuf too old (${Protobuf_VERSION}), should be >=3.5.")
	elseif(NOT TARGET protobuf::libprotobuf)
		message(AUTHOR_WARNING "Cmake too old to define protobuf::libprotobuf, will try PkgConfig instead.")
	else()
		message("Found Protobuf via find_package Version:${protobuf_VERSION}")
		set(PROTOBUF_LIBS ${Protobuf_LIBRARIES})
		#set(PROTOBUF_LIBS protobuf::libprotobuf)
		set(PROTOBUF_INCS ${Protobuf_INCLUDE_DIRS})
		set(PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
	endif()
endif()

if(NOT DEFINED PROTOBUF_LIBS)
	message("Could not find Protobuf via find_package. Searching via PkgConfig")

	find_package(PkgConfig REQUIRED)
	pkg_check_modules(protobuf REQUIRED IMPORTED_TARGET protobuf>=3.5)
	set(PROTOBUF_LIBS PkgConfig::protobuf)
	set(PROTOBUF_INCS ${protobuf_INCLUDE_DIRS})
	set(PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

	message("Found Protobuf via PkgConfig Version:${protobuf_VERSION}")
endif()

message("PROTOBUF_LIBS=${PROTOBUF_LIBS}")
message("PROTOBUF_INCLUDE_DIRS=${PROTOBUF_INCS}")
message("PROTOBUF_PROTOC=${PROTOBUF_PROTOC}")


find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
	message("Found OpenSsl Version:${oPENssl_VERSION}")
else()
	message("Could Not find OpenSsl")
endif()
set(OPENSSL_TARGET OpenSSL::Crypto)


find_package(Threads REQUIRED)
if (THREADS_FOUND)
	message("${PROJECT_NAME} Threads found - Version:${Threads_VERSION}")
else()
	message(FATAL_ERROR, "${PROJECT_NAME} Cannot find Threads")
endif()

list(APPEND CMAKE_MODULE_PATH /usr/local/lib/cmake/grpc/modules)
find_package(c-ares REQUIRED MODULE)
if(NOT c-ares_FOUND)
	message(FATAL_ERROR, "Couldnt find c-ares")
endif()


# Try to find gRPC with find_package and fall back to pkg_config
find_package(gRPC REQUIRED CONFIG)
if(gRPC_FOUND)
	message("Found gRPC via find_package Version:${gRPC_VERSION}")

	set(GRPC_INCS $<TARGET_PROPERTY:gRPC::grpc++,INTERFACE_INCLUDE_DIRECTORIES>)

	list(APPEND GRPC_LIBS $<TARGET_PROPERTY:gRPC::grpc++_reflection,INTERFACE_LINK_LIBRARIES>)

	set(GRPC_PLUGIN $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

else()
	message("Finding gRPC via PkgConfig")

	find_package(PkgConfig REQUIRED)
	pkg_check_modules(grpc REQUIRED IMPORTED_TARGET grpc>=9.0)
	pkg_check_modules(grpc++ REQUIRED IMPORTED_TARGET grpc++>=1.28)

	set(GRPC_INCS ${GRPC_INCLUDE_DIRS})

	list(APPEND GRPC_LIBS PkgConfig::grpc++_reflection)

	find_program(GRPC_PLUGIN grpc_cpp_plugin)

endif()

message("GRPC_LIBS=${GRPC_LIBS}")
message("GRPC_INCLUDE_DIRS=${GRPC_INCS}")
message("GRPC_PLUGIN=${GRPC_PLUGIN}")


# Find the proto library created in tickProto
message("Searching for tickProto library")
find_library(TickProtoLib tickproto PATHS ${CMAKE_INSTALL_PREFIX}/lib)
if(TickProtoLib-NOTFOUND)
	message(FATAL_ERROR "Could not find tickProto library. Searched in ${CMAKE_INSTALL_PREFIX}/lib")
else()
	message("Found tickProto library in ${CMAKE_INSTALL_PREFIX}/lib")
endif()


# -I directories
include_directories(.)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${CMAKE_INSTALL_PREFIX}/include")
include_directories("${GRPC_INCS}")

# Generate Version
configure_file(tick_direction_client_version.h.in general/tick_direction_client_version.h)

# Build Client
add_executable(tickDirectionClient main.cpp)

# the install RPATH for tickDirectionClient to find tickproto in the install tree.
# if the install RPATH is not provided, the install tickDirectionClient will have none
#set_target_properties(tickDirectionClient PROPERTIES INSTALL_RPATH “@loader_path/../lib”)
set_target_properties(tickDirectionClient PROPERTIES INSTALL_RPATH “@executable_path/../lib”)

target_link_libraries(tickDirectionClient
        ${TickProtoLib} ${GRPC_LIBS} ${PROTOBUF_LIBS})

	#install(TARGETS tickDirectionClient
#        CONFIGURATION Debug
#        RUNTIME DESTINATION Debug/bin)

install(TARGETS tickDirectionClient
        COMPONENT Exes
        DESTINATION bin)
