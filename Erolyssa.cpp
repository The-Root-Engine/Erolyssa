// Root Engine / Erolyssa

#include "Erolyssa.h"

#include <cstdio>
#include <cstdlib>

void ErolyssaLog(const char* InMessage)
{
    printf("[Log][Erolyssa] %s\n", InMessage);
}

void ErolyssaLogWarning(const char* InMessage)
{
    printf("[Warning][Erolyssa] %s\n", InMessage);
}

void ErolyssaLogError(const char* InMessage)
{
    printf("[Error][Erolyssa] %s\n", InMessage);
}

void ErolyssaLogValidation(const char* InMessage)
{
    printf("[Validation][Erolyssa] %s\n", InMessage);
}

void ErolyssaTerminate(const char* InMessage)
{
    printf("[Terminated][Erolyssa] %s\n", InMessage);
    exit(-1);
}
