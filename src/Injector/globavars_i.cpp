#include "src/Injector/globalvars_i.h"

#include "src/utils.h"

using namespace Eustia;
using namespace EustiaInjector;

wchar* GlobalVars::ModulePath = nullptr;

bool GlobalVars::Init()
{
    GlobalVars::ModulePath = GetThisModulePath();

    return true;
}