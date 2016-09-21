#include "src/ipc.h"

#include <string>

#include "src/log.h"

using namespace std;

namespace Eustia
{

MemoryIPC* MemoryIPC::Init(InitType type, const char* ipcName)
{
    auto ipc = new MemoryIPC();
    auto ret = ipc->Init(type, ipcName);
    if (!IsSuccess(ret))
    {
        LOGERROR("Can't init MemoryIPC:%s, err:%d", ipcName, (int)ret);
        delete ipc;
        return nullptr;
    }
    return ipc;
}

ErrType MemoryIPC::GetMemoryPointer(void** ptr)
{
    if (!memPtr_)
    {
        auto ret = IPCHelper::LockSharedMemory(memKey_, &memPtr_);
        if (!IsSuccess(ret))
        {
            return ret;
        }
    }
    *ptr = memPtr_;
    return ErrType::Success;
}

ErrType MemoryIPC::ReleaseMemoryPointer()
{
    ErrType ret;
    if (memPtr_)
    {
        ret = IPCHelper::UnlockSharedMemory(memKey_, memPtr_);
        if (IsSuccess(ret))
        {
            memPtr_ = nullptr;
        }
    }
    return ret;
}

void MemoryIPC::Dispose()
{
    ReleaseMemoryPointer();
    IPCHelper::ReleaseSharedMemory(memKey_);
    delete this;
}

ErrType MemoryIPC::Init(InitType type, const char* ipcName)
{
    string ipcNameBase = ipcName;
    auto ipcMemNameStr = ipcNameBase + "_mem";
    auto ipcMemName = ipcMemNameStr.c_str();
    switch (type)
    {
    case Eustia::MemoryIPC::InitType::OpenNew:
        return OpenNewIPC(ipcMemName);
        break;
    case Eustia::MemoryIPC::InitType::OpenExisting:
        return OpenExistingIPC(ipcMemName);
        break;
    case Eustia::MemoryIPC::InitType::OpenAlways:
        return OpenIPC(ipcMemName);
        break;
    default:
        break;
    }
    return ErrType::Fail;
}

ErrType MemoryIPC::OpenNewIPC(const char* ipcMemName)
{
    auto ret = IPCHelper::CreateSharedMemory(ipcMemName, kMemSize, &memKey_);
    if (!IsSuccess(ret))
    {
        return ret;
    }
    return ErrType::Success;
}

ErrType MemoryIPC::OpenExistingIPC(const char* ipcMemName)
{
    auto ret = IPCHelper::OpenSharedMemory(ipcMemName, &memKey_);
    if (!IsSuccess(ret))
    {
        return ret;
    }
    return ErrType::Success;
}

ErrType MemoryIPC::OpenIPC(const char* ipcMemName)
{
    auto ret = IPCHelper::OpenSharedMemory(ipcMemName, &memKey_);
    if (!IsSuccess(ret))
    {
        ret = IPCHelper::CreateSharedMemory(ipcMemName, kMemSize, &memKey_);
        if (!IsSuccess(ret))
        {
            return ret;
        }
    }
    return ErrType::Success;
}

}