#pragma once

#include <string>

#if defined(__linux__)
#define _ROOT_DEFAULT_DIRECTORY_NAME "servercryptomadnessvortex"
#define _ROOT_DIRECTORY "/usr/share/servercryptomadnessvortex/"
#elif defined(__APPLE__) && defined(__MACH__)
#define _ROOT_DEFAULT_DIRECTORY_NAME "ServerCryptoMadnessVortex"
#define _ROOT_DIRECTORY "/Users/Shared/ServerCryptoMadnessVortex/"
#else
#error "Unsupported operating system"
#endif

#define _DEFAULT_DIR_SERVER std::string(std::string(_ROOT_DIRECTORY) + std::string("server/")).c_str()
#define _DEFAULT_FILE_USERS std::string(std::string(_DEFAULT_DIR_SERVER) + std::string("users")).c_str()
