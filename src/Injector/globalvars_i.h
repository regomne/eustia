#ifndef GLOBALVARS_I_H_
#define GLOBALVARS_I_H_

#include "src/common.h"

namespace EustiaInjector
{

class GlobalVars
{
public:
    static Eustia::wchar* ModulePath;

    static bool Init();
};

}

#endif