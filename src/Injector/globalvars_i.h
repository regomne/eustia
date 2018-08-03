#ifndef GLOBALVARS_I_H_
#define GLOBALVARS_I_H_

#include <string>

#include "src/common.h"
#include "src/misc/localization.h"

namespace injector
{

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
};

}

#endif