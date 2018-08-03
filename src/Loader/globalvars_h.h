#ifndef GLOBALVARS_H_H_
#define GLOBALVARS_H_H_

#include <windows.h>
#include <string>

#include "src/common.h"
#include "src/core/ipc.h"

class GlobalVars
{
public:
    static void init()
    {
        if (!instance_)
        {
            instance_ = new GlobalVars();
        }
    }

    static GlobalVars* get()
    {
        return instance_;
    }

    void dispose()
    {
        if (instance_)
        {
            delete instance_;
            instance_ = nullptr;
        }
    }

private:
    static GlobalVars* instance_;

public:
    std::string module_path;
    HANDLE dll_handle;
    bool need_delay_check;
    intptr_t check_process_id;
    eustia::LoaderIPC ipc_info;;
};


#endif