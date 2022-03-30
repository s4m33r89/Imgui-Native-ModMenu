#include "Includes.h"

namespace Tools
{
    void Hook(void *target, void *replace, void **backup);
    bool Read(void *addr, void *buffer, size_t length);
    bool Write(void *addr, void *buffer, size_t length);
    void writeValue(uintptr_t address, void *value, ssize_t size);
    bool PVM_ReadAddr(void *addr, void *buffer, size_t length);
    bool PVM_WriteAddr(void *addr, void *buffer, size_t length);
    bool ProcessRead(void *address, void *buffer, size_t size, bool write);
    bool IsPtrValid(void *addr);

    uintptr_t GetBaseAddress(const char *name);
    uintptr_t GetEndAddress(const char *name);
    uintptr_t FindPattern(const char *lib, const char* pattern);

    std::string RandomString(const int len);
    std::string GetPackageName(JNIEnv *env, jobject context);

    std::string CalcMD5(std::string s);
    std::string CalcSHA256(std::string s);
}
