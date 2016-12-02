#ifndef INJECTOR_H_
#define INJECTOR_H_

#include "src/common.h"
#include <string>

namespace injector
{

bool create_and_inject(const std::string& app_path, const std::string& params, const std::string& mod_path);

bool open_and_inject_process(uint32_t pid, const std::string& mod_path);

bool hook_process_cb(uint32_t pid, const std::string& mod_path, const std::string& loader_path);

}

#endif
