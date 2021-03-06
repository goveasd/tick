cmake_minimum_required(VERSION 3.16)

project(tickProto VERSION 1.0)

# Set in Root project?
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/../install )
set(CMAKE_CURRENT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)

if (APPLE)
	set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl/")
endif()

message("${PROJECT_NAME} CMAKE_CXX_STANDARD = ${CMAKE_CXX_STANDARD}")
message("${PROJECT_NAME} CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
message("${PROJECT_NAME} PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
message("${PROJECT_NAME} PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}")
message("${PROJECT_NAME} CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}")
message("${PROJECT_NAME} CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

find_package(Protobuf REQUIRED)
if(Protobuf_FOUND)
	if(Protobuf_VERSION VERSION_LESS 3.5)
		message(FATAL_ERROR "Version of Protobuf too old (${Protobuf_VERSION}), should be >=3.5.")
	elseif(NOT TARGET protobuf::libprotobuf)
		message(AUTHOR_WARNING "Cmake too old to define protobuf::libprotobuf, will try PkgConfig instead.")
	else()
		message("Found Protobuf via find_package Version:${Protobuf_VERSION}")
		set(PROTOBUF_LIBS ${Protobuf_LIBRARIES})
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
	message("Found OpenSsl Version:${openssl_VERSION}")
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


# Try to find gRPC with find_package and fall back to pkg_config
find_package(gRPC CONFIG REQUIRED)
if(gRPC_FOUND)
	message("Found gRPC via find_package Version:${gRPC_VERSION}")


	list(APPEND GRPC_LIBS $<TARGET_PROPERTY:gRPC::grpc++_reflection,INTERFACE_LINK_LIBRARIES>)

	set(GRPC_PLUGIN $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

else()
	message("Finding gRPC via PkgConfig")

	find_package(PkgConfig REQUIRED)
	pkg_check_modules(grpc REQUIRED IMPORTED_TARGET grpc>=9.0)
	pkg_check_modules(grpc++ REQUIRED IMPORTED_TARGET grpc++>=1.28)

	list(APPEND GRPC_LIBS PkgConfig::grpc++_reflection)
	set(GRPC_INCS ${GRPC_INCLUDE_DIRS})

	find_program(GRPC_PLUGIN grpc_cpp_plugin)
endif()

message("GRPC_LIBS=${GRPC_LIBS}")
message("GRPC_INCLUDE_DIRS=${GRPC_INCS}")
message("GRPC_PLUGIN=${GRPC_PLUGIN}")


file(GLOB PROTOBUF_DEFINITION_FILES "*.proto")
set(PROTOBUF_INPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")
set(PROTOBUF_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/")


message ("PROTOBUF_INCLUDE_DIRS = ${PROTOBUF_INCS}")
message ("PROTOBUF_OUTPUT_DIRECTORY = ${PROTOBUF_OUTPUT_DIRECTORY}")
message ("CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")

foreach(file ${PROTOBUF_DEFINITION_FILES})

	# Proto file
	get_filename_component(the_proto ${file} NAME)
	get_filename_component(the_proto_path "${file}" PATH)

	message("Processing Proto File ${the_proto} in ${the_proto_path}")
	string(REGEX REPLACE "\.proto" "" the_proto_base ${the_proto})

	# Generated sources
	set(the_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/${the_proto_base}.pb.cc")
	set(the_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${the_proto_base}.pb.h")
	set(the_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/${the_proto_base}.grpc.pb.cc")
	set(the_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${the_proto_base}.grpc.pb.h")

	message ("PROTO_SRCS = ${the_proto_srcs} ${the_grpc_srcs}")
	message ("PROTO_HDRS = ${the_proto_hdrs} ${the_grpc_hdrs}")

	add_custom_command(
				OUTPUT "${the_proto_srcs}" "${the_proto_hdrs}" "${the_grpc_srcs}" "${the_grpc_hdrs}"
				COMMAND ${PROTOBUF_PROTOC}
				ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
				--cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
				-I "${the_proto_path}"
				--plugin=protoc-gen-grpc="${GRPC_PLUGIN}"
				"${the_proto}"
				DEPENDS "${the_proto}")

	# Generated sources
	list(APPEND pt_proto_srcs "${the_proto_srcs}")
	list(APPEND pt_proto_hdrs "${the_proto_hdrs}")
	list(APPEND pt_grpc_srcs "${the_grpc_srcs}")
	list(APPEND pt_grpc_hdrs "${the_grpc_hdrs}")

endforeach()


message ("ALL PROTO_SRCS = ${pt_proto_srcs} ${pt_grpc_srcs}")
message ("ALL PROTO_HDRS = ${pt_proto_hdrs} ${pt_grpc_hdrs}")


# Include generated *.pb.h files
include_directories(.)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_library(tickproto SHARED ${pt_proto_srcs} ${pt_grpc_srcs} )

target_include_directories(tickproto
                           PUBLIC ${GRPC_INCS} 
                           PUBLIC ${PROTOBUF_INCS})

target_link_libraries(tickproto
                           PUBLIC ${GRPC_LIBS}
                           PUBLIC ${PROTOBUF_LIBS})

install(TARGETS tickproto
        COMPONENT Libraries
        DESTINATION lib)
install(FILES ${pt_proto_hdrs} ${pt_grpc_hdrs} DESTINATION include/tickproto)
