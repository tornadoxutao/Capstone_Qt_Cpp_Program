#include "Memory.h"

Memory::Memory(){
    memoryUsage = 0;
    HmemUsage = 0;
    LmemUsage = 1000;
}

void Memory::getUsage(void)
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    memoryUsage = status.dwMemoryLoad;

    if (memoryUsage > HmemUsage) {
        HmemUsage = memoryUsage;
    }
    if (memoryUsage < LmemUsage) {
        LmemUsage = memoryUsage;
    }
}
