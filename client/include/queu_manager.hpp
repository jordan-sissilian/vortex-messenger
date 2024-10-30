#pragma once

#include <iostream>
#include <fstream>
#include <cstring>

#include "universal_structs_client.hpp"

//server OpCode
#define AUTH_CODE 0x01 // auth                      //ok
#define CREA_CODE 0x02 // create account            //ok
#define DECO_CODE 0x03 // deco                      //todo
#define MPMS_CODE 0x04 // private message send      //ok
#define RMSG_CODE 0x05 // room    message send      //todo
#define CRMS_CODE 0x06 // create room               //todo
#define JRMS_CODE 0x07 // join   room               //todo
#define JNMP_CODE 0x08 // join   mp                 //ok
#define SDPK_CODE 0x09 // send public key           //ok
#define JMPS_CODE 0xa // send joint mp status       //ok
#define CHRP_CODE 0xb // send challenge reponse     //ok


#include "responseToServerStruct.hpp"
//client OpCode
#define CNNT_CODE 0x01 // connected to server       //ok
#define RVUM_CODE 0x02 // recive user message       //ok
#define RVRM_CODE 0x03 // recive room message       //todo
#define NJMP_CODE 0x04 // new    join mp            //ok
#define SJMP_CODE 0x05 // status join mp            //ok


#ifdef __linux__
#define PATH_TO_QUEUE "/usr/share/clientcryptomadnessvortex/dir_user/queue/"
#elif __APPLE__
#define PATH_TO_QUEUE "/Users/Shared/ClientCryptoMadnessVortex/dir_user/queue/"
#endif

class QueueManager {
public:
    QueueManager() {};
    void setQueue(const StuctToServ& stuct);
    void getQueue(uint32_t id, StuctToServ& stuct);
    void removeQueue(uint32_t id);
    void getInfo(uint32_t id);
};
