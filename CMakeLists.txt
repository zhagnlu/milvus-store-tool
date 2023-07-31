cmake_minimum_required(VERSION 3.12)

project(milvus-store-tool)

set( CONAN_DISABLE_CHECK_COMPILER ON )

add_compile_options(-std=c++17 -Wall)

include( ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake )
conan_basic_setup( KEEP_RPATHS )
list(APPEND CMAKE_PREFIX_PATH ${CONAN_LIB_DIRS})

add_subdirectory(src/storage)
add_executable(MinioTool src/minio.cpp)
add_executable(AliOssTool src/oss.cpp)
add_executable(GoogleCloudTool src/gcp.cpp)

target_link_libraries(MinioTool milvus_storage ${CONAN_LIBS})
target_link_libraries(AliOssTool milvus_storage ${CONAN_LIBS})
target_link_libraries(GoogleCloudTool  milvus_storage ${CONAN_LIBS})

