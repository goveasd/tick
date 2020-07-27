cmake_minimum_required(VERSION 3.15)
project(clion_test VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install )

option(BUILD_GRPC "Use grpc implementaion" ON)

# Build Protobuf Lib
if(BUILD_GRPC)
    add_subdirectory(tickProto)
    add_subdirectory(tickServer)
    add_subdirectory(tickClient)
	add_subdirectory(tickDirectionClient)
	add_subdirectory(tickDirectionServer)
endif()
