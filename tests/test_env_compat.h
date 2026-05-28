#pragma once
#include <cstdlib>

#ifdef _WIN32
inline int setenv(const char *name, const char *value, int)
{
    return _putenv_s(name, value);
}
inline int unsetenv(const char *name)
{
    return _putenv_s(name, "");
}
#endif
