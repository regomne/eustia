#ifndef JSON_CLASS_H_
#define JSON_CLASS_H_
#include <string>

namespace eustia
{

class IJsonClass
{
public:
    virtual bool parse(const std::string& json_str) = 0;
    virtual std::string to_string();
};

}

#endif
