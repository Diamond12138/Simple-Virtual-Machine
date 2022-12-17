#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#define STR(x) #x
#define UTIL_GREGISTER_NAME(reg)                  \
    case RegisterEnum::GeneralRegister::##reg: \
        name = STR(reg);                        \
        break;

namespace svm
{

} // namespace svm

#endif