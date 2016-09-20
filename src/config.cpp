#include "src/config.h"

namespace Eustia
{

Configs* Configs::instance_ = nullptr;

Configs* Configs::GetCurrentConfigs()
{
    if (!instance_)
    {
        instance_ = new Configs();
    }
    return instance_;
}

}