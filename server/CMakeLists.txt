cmake_minimum_required(VERSION 3.10)
project(CryptoMadness_Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.14")

include_directories(include)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

include_directories(/opt/homebrew/Cellar/cryptopp/8.9.0/include/cryptopp)
link_directories(/opt/homebrew/Cellar/cryptopp/8.9.0/lib)

option(LOCAL_SERVER "Use local server" ON)  # default
option(DISTANT_SERVER "Use distant server" OFF)

if(LOCAL_SERVER)
    add_definitions(-DLOCAL_SERVER)
    message(STATUS "Using local server.")
elseif(DISTANT_SERVER)
    add_definitions(-DDISTANT_SERVER)
    message(STATUS "Using distant server.")
endif()

option(INCLUDE_OPCODE "Include opcode.o in the build" ON)

set(OBJS)

if(INCLUDE_OPCODE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-u,__ZN7Command22function_table_op_codeE")
    add_definitions(-DINCLUDE_OPCODE)
    message(STATUS "Including opcode.o")
    
    set(ASM_SRC ${CMAKE_SOURCE_DIR}/src/opcode/opcode.s)
    add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/src/opcode/opcode.o
        COMMAND as -arch arm64 -mmacosx-version-min=10.14 -o ${CMAKE_SOURCE_DIR}/src/opcode/opcode.o ${ASM_SRC}
        DEPENDS ${ASM_SRC}
        COMMENT "Building opcode.o from opcode.s"
    )
    add_custom_target(build_opcode ALL DEPENDS ${CMAKE_SOURCE_DIR}/src/opcode/opcode.o)
    list(APPEND OBJS ${CMAKE_SOURCE_DIR}/src/opcode/opcode.o)
else()
    message(STATUS "Not including opcode.o")
endif()

file(GLOB SOURCES "src/*.cpp" "src/command/*.cpp")

add_executable(CryptoMadness_Server ${CMAKE_SOURCE_DIR}/../key/key.o ${SOURCES} ${OBJS})

target_link_libraries(CryptoMadness_Server
    cryptopp
    OpenSSL::SSL
    OpenSSL::Crypto
)

message(STATUS "Building CryptoMadness_Server with INCLUDE_OPCODE=${INCLUDE_OPCODE}, LOCAL_SERVER=${LOCAL_SERVER}, DISTANT_SERVER=${DISTANT_SERVER}")