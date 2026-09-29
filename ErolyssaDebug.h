// Root Engine / Erolyssa

#pragma once

class FErolyssaDebug
{
public:
    static void Info(const char* InMessage, ...);
    static void Warning(const char* InMessage, ...);
    static void Error(const char* InMessage, ...);
    static void Validation(const char* InMessage, ...);
    static void ErolyssaTerminate(const char* InMessage, ...);
};

void ErolyssaLog(const char* InMessage);
void ErolyssaLogWarning(const char* InMessage);
void ErolyssaLogError(const char* InMessage);
void ErolyssaLogValidation(const char* InMessage);
void ErolyssaTerminate(const char* InMessage);
