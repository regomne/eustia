#include "src/core/ipc.h"

#include <windows.h>

#include "src/core/log.h"

namespace eustia
{

ErrType IPCHelper::create_shared_memory(const std::string& name, uint32_t size, IPCKey* key)
{
    auto handle = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, size, name.c_str());
    if (handle == nullptr)
    {
        LOGERROR("CreateFileMappingA failed, err:%d", GetLastError());
        return ErrType::OSMem;
    }
    *key = (IPCKey)handle;
    return ErrType::Success;
}

ErrType IPCHelper::open_shared_memory(const std::string& name, IPCKey* key)
{
    auto handle = OpenFileMappingA(PAGE_READWRITE, FALSE, name.c_str());
    if (handle == nullptr)
    {
        LOGERROR("OpenFileMappingA failed, err:%d", GetLastError());
        return ErrType::OSMem;
    }
    *key = (IPCKey)handle;
    return ErrType::Success;
}

ErrType IPCHelper::lock_shared_memory(IPCKey key, void** memPtr)
{
    auto handle = (HANDLE)key;
    *memPtr = MapViewOfFile(handle, PAGE_READWRITE, 0, 0, 0);
    if (!*memPtr)
    {
        LOGERROR("MapViewOfFile failed, err:%d", GetLastError());
        return ErrType::OSMem;
    }
    return ErrType::Success;
}

ErrType IPCHelper::unlock_shared_memory(IPCKey key, void* memPtr)
{
    UNREFERENCED_PARAMETER(key);
    UnmapViewOfFile(memPtr);
    return ErrType::Success;
}

ErrType IPCHelper::release_shared_memory(IPCKey key)
{
    CloseHandle((HANDLE)key);
    return ErrType::Success;
}

}