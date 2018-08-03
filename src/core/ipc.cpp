#include "src/core/ipc.h"

#include <string>

#include "src/core/log.h"
#include "third_party/json/json.hpp"

using namespace std;
using json = nlohmann::json;

namespace eustia
{

MemoryIPC* MemoryIPC::init(InitType type, const std::string& ipcName)
{
    auto ipc = new MemoryIPC();
    auto ret = ipc->init_p(type, ipcName);
    if (!IsSuccess(ret))
    {
        LOGERROR("Can't init MemoryIPC:%s, err:%d", ipcName.c_str(), (int)ret);
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

ErrType MemoryIPC::init_p(InitType type, const std::string& ipcName)
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

ErrType MemoryIPC::open_new_ipc(const std::string& ipcMemName)
{
    auto ret = IPCHelper::create_shared_memory(ipcMemName, kMemSize, &mem_key_);
    if (!IsSuccess(ret))
    {
        return ret;
    }
    return ErrType::Success;
}

ErrType MemoryIPC::open_existing_ipc(const std::string& ipcMemName)
{
    auto ret = IPCHelper::open_shared_memory(ipcMemName, &mem_key_);
    if (!IsSuccess(ret))
    {
        return ret;
    }
    return ErrType::Success;
}

ErrType MemoryIPC::open_ipc(const std::string& ipcMemName)
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

//todo return bool
static void convert_ip_addr(const std::string& ip_str,IPAddrType* ip_type, uint8_t* ip)
{
	ip;
    if (ip_str.find(':') != std::string::npos)
    {
        *ip_type = IPAddrType::IPv6;
        
    }
    else
    {
        *ip_type = IPAddrType::IPv4;
        
    }
}

bool EustiaIPC::parse(const std::string& json_str)
{
    json j;
    try
    {
        j = std::move(json::parse(json_str));
    }
    catch (std::exception& e)
    {
        LOGERROR("json parse error: %s",e.what());
        return false;
    }
    auto host_ip = j.find("host_ip");
    auto host_port = j.find("host_port");
    if (host_ip == j.end() ||
        host_port == j.end() ||
        !host_ip->is_string() ||
        !host_port->is_number_integer())
    {
        LOGERROR("json data error!");
        return false;
    }
	std::string s = *host_ip;
    convert_ip_addr(s, &this->ip_addr_type_, this->host_ip_);
    this->host_port_ = *host_port;
    return true;
}

}