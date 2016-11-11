#ifndef CONFIG_H_
#define CONFIG_H_

#include "log.h"

namespace eustia
{

class Configs
{
public:
    static Configs* GetCurrentConfigs();

private:
    static Configs* instance_;

};

}

#endif