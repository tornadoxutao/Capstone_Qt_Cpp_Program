#include "CPUusage.h"
#include "CPUusagethread.h"

CPUusagethread::CPUusagethread()
{
    cpuUsage = 0;
    HcpuUse = 0;
    LcpuUse = 1000;
}

void CPUusagethread::run()
{
    CPUusage cpu;
    while(true)
    {
        cpu.getUsage();
        cpuUsage = (int)cpu.cpuUsage + .5;
        if (cpuUsage > HcpuUse) {
            HcpuUse = cpuUsage;
        }
        if (cpuUsage < LcpuUse) {
            LcpuUse = cpuUsage;
        }
        Sleep(500);
    }
    array[cu] = cpuUsage;
    if(cu == 9)
    {
        cu = 0;
    }
    else
    {
        cu++;
    }
}
