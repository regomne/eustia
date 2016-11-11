#ifndef IPC_H_
#define IPC_H_

#include "src/common.h"
#include <string>

namespace eustia
{

typedef intptr_t IPCKey;

class IPCHelper
{
public:
    static ErrType CreateSem(const std::string& name, IPCKey* key);
    static ErrType OpenSem(const std::string& name, IPCKey* key);
    static ErrType ReleaseSem(IPCKey key);

    static ErrType create_shared_memory(const std::string& name, uint32_t size, IPCKey* key);
    static ErrType open_shared_memory(const std::string& name, IPCKey* key);
    static ErrType lock_shared_memory(IPCKey key, void** memPtr);
    static ErrType unlock_shared_memory(IPCKey key, void* memPtr);
    static ErrType release_shared_memory(IPCKey key);
    
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
    static MemoryIPC* init(InitType type, const std::string& ipcName);

    ErrType get_memory_pointer(void** ptr);
    ErrType release_memory_pointer();
    void dispose();

private:
    //size of shared memory
    static const uint32_t kMemSize = 0x1000;

    MemoryIPC() :mem_key_(0), mem_ptr_(nullptr) {}
    ~MemoryIPC() {}

    ErrType init_p(InitType type, const std::string& ipcName);
    ErrType open_new_ipc(const std::string& ipcMemName);
    ErrType open_existing_ipc(const std::string& ipcMemName);
    ErrType open_ipc(const std::string& ipcMemName);

private:
    IPCKey mem_key_;
    void* mem_ptr_;
};

struct IPCInfo
{
    InjectType inject_type;
    intptr_t host_process_id; //process injector
    intptr_t dest_process_id; //process to inject 
    intptr_t dest_thread_id; //thread to inject
    IPAddrType ip_addr_type;
    uint8_t host_ip[16]; //always 127.0.0.1
    uint16_t host_port; //host port to connect, little endian
    wchar_t eusita_dll_name[MAX_NAME_LEN_OF_EUSTIA_DLL]; //end with zero
    union
    {
        uint32_t key_to_wait; //if type is keyboard hook
    };
};

}

#endif
