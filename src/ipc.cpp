#include "src/ipc.h"

#include <string>

#include "src/log.h"

using namespace std;

namespace eustia
{

MemoryIPC* MemoryIPC::init(InitType type, const char* ipcName)
{
    auto ipc = new MemoryIPC();
    auto ret = ipc->init_p(type, ipcName);
    if (!IsSuccess(ret))
    {
        LOGERROR("Can't init MemoryIPC:%s, err:%d", ipcName, (int)ret);
        delete ipc;
        return nullptr;
    }
    return ipc;
}

ErrType MemoryIPC::get_memory_pointer(void** ptr)
{
    if (!mem_ptr_)
    {
        auto ret = IPCHelper::lock_shared_memory(mem_key_, &mem_ptr_);
        if (!IsSuccess(ret))
        {
            return ret;
        }
    }
    *ptr = mem_ptr_;
    return ErrType::Success;
}

ErrType MemoryIPC::release_memory_pointer()
{
    ErrType ret = ErrType::Success;
    if (mem_ptr_)
    {
        ret = IPCHelper::unlock_shared_memory(mem_key_, mem_ptr_);
        if (IsSuccess(ret))
        {
            mem_ptr_ = nullptr;
        }
    }
    return ret;
}

void MemoryIPC::dispose()
{
    release_memory_pointer();
    IPCHelper::release_shared_memory(mem_key_);
    delete this;
}

ErrType MemoryIPC::init_p(InitType type, const char* ipcName)
{
    string ipcNameBase = ipcName;
    auto ipcMemNameStr = ipcNameBase + "_mem";
    auto ipcMemName = ipcMemNameStr.c_str();
    switch (type)
    {
    case eustia::MemoryIPC::InitType::OpenNew:
        return open_new_ipc(ipcMemName);
        break;
    case eustia::MemoryIPC::InitType::OpenExisting:
        return open_existing_ipc(ipcMemName);
        break;
    case eustia::MemoryIPC::InitType::OpenAlways:
        return open_ipc(ipcMemName);
        break;
    default:
        break;
    }
    return ErrType::Fail;
}

ErrType MemoryIPC::open_new_ipc(const char* ipcMemName)
{
    auto ret = IPCHelper::create_shared_memory(ipcMemName, kMemSize, &mem_key_);
    if (!IsSuccess(ret))
    {
        return ret;
    }
    return ErrType::Success;
}

ErrType MemoryIPC::open_existing_ipc(const char* ipcMemName)
{
    auto ret = IPCHelper::open_shared_memory(ipcMemName, &mem_key_);
    if (!IsSuccess(ret))
    {
        return ret;
    }
    return ErrType::Success;
}

ErrType MemoryIPC::open_ipc(const char* ipcMemName)
{
    auto ret = IPCHelper::open_shared_memory(ipcMemName, &mem_key_);
    if (!IsSuccess(ret))
    {
        ret = IPCHelper::create_shared_memory(ipcMemName, kMemSize, &mem_key_);
        if (!IsSuccess(ret))
        {
            return ret;
        }
    }
    return ErrType::Success;
}

}