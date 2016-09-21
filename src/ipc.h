#ifndef IPC_H_
#define IPC_H_

#include "src/common.h"

namespace Eustia
{

typedef intptr IPCKey;

class IPCHelper
{
public:
    static ErrType CreateSem(const char* name, IPCKey* key);
    static ErrType OpenSem(const char* name, IPCKey* key);
    static ErrType ReleaseSem(IPCKey key);

    static ErrType CreateSharedMemory(const char* name, u32 size, IPCKey* key);
    static ErrType OpenSharedMemory(const char* name, IPCKey* key);
    static ErrType LockSharedMemory(IPCKey key, void** memPtr);
    static ErrType UnlockSharedMemory(IPCKey key, void* memPtr);
    static ErrType ReleaseSharedMemory(IPCKey key);
    
};

// is not thread-safe
// 1 process write, other processes read.
class MemoryIPC
{
public:
    enum class InitType
    {
        OpenNew,
        OpenExisting,
        OpenAlways,
    };

public:
    static MemoryIPC* Init(InitType type, const char* ipcName);

    ErrType GetMemoryPointer(void** ptr);
    ErrType ReleaseMemoryPointer();
    void Dispose();

private:
    //size of shared memory
    static const u32 kMemSize = 0x1000;

    MemoryIPC() :memKey_(0), memPtr_(nullptr) {}
    ~MemoryIPC() {}

    ErrType Init(InitType type, const char* ipcName);
    ErrType OpenNewIPC(const char* ipcMemName);
    ErrType OpenExistingIPC(const char* ipcMemName);
    ErrType OpenIPC(const char* ipcMemName);

private:
    IPCKey memKey_;
    void* memPtr_;
};

struct IPCInfo
{
    InjectType injectType;
    intptr hostProcessId; //process injector
    intptr destProcessId; //process to inject 
    intptr destThreadId; //thread to inject
    IPAddrType ipAddrType;
    u8 hostIp[16]; //always 127.0.0.1
    u16 hostPort; //host port to connect, little endian
    wchar eustiaDllName[MAX_NAME_LEN_OF_EUSTIA_DLL]; //end with zero
    union
    {
        u32 keyToWait; //if type is keyboard hook
    };
};

}

#endif
