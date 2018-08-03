#include "src/core/config.h"

namespace eustia
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