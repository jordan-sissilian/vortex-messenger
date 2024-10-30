cmake_minimum_required(VERSION 3.16)
project(CryptoMadness_Client)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(LOCAL_SERVER "Build for local server" OFF)
option(DISTANT_SERVER "Build for distant server" OFF)

set(ENV_USER $ENV{USER})
set(CMAKE_PREFIX_PATH /Users/${ENV_USER}/Qt/6.7.2/macos/lib/cmake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

find_path(CRYPTOPP_INCLUDE_DIR NAMES cryptlib.h PATHS /opt/homebrew/Cellar/cryptopp/8.9.0/include/cryptopp)
find_library(CRYPTOPP_LIBRARY NAMES cryptopp PATHS /opt/homebrew/lib)

if(NOT CRYPTOPP_LIBRARY)
    message(FATAL_ERROR "Crypto++ library not found.")
endif()

include_directories(${CRYPTOPP_INCLUDE_DIR})

find_package(OpenSSL REQUIRED)
if(NOT OPENSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found.")
endif()
include_directories(${OPENSSL_INCLUDE_DIR})

set(OBJS ${CMAKE_SOURCE_DIR}/../key/key.o)

file(GLOB_RECURSE SOURCES "src/ui/*.cpp" "src/*.cpp" "include/ui/*.h" "include/*.h" "include/ui/page_type/*.h")

add_executable(CryptoMadness_Client ${SOURCES} ${OBJS})

target_link_libraries(CryptoMadness_Client
    Qt6::Widgets
    Qt6::Core
    ${CRYPTOPP_LIBRARY}
    ${OPENSSL_SSL_LIBRARY} 
    ${OPENSSL_CRYPTO_LIBRARY}
)

target_include_directories(CryptoMadness_Client PRIVATE
    include
    include/ui
    include/ui/page_type
)

if(LOCAL_SERVER)
    message(STATUS "Building client for local server.")
    target_compile_definitions(CryptoMadness_Client PRIVATE LOCAL_SERVER)
    target_compile_definitions(CryptoMadness_Client PRIVATE SERVER_ADDRESS="127.0.0.1")
else()
    message(STATUS "Building client for distant server.")
    target_compile_definitions(CryptoMadness_Client PRIVATE DISTANT_SERVER)
    target_compile_definitions(CryptoMadness_Client PRIVATE SERVER_ADDRESS="87.106.58.126")
endif()
