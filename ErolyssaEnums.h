// Root Engine / Erolyssa

#pragma once

#define EROLYSSA_ENABLE_VALIDATION_LAYERS 1
#define EROLYSSA_VLA(Type, Size) static_cast<Type*>(_alloca(Size * sizeof(Type)))

void ErolyssaLog(const char* InMessage);
void ErolyssaLogWarning(const char* InMessage);
void ErolyssaLogError(const char* InMessage);
void ErolyssaLogValidation(const char* InMessage);
void ErolyssaTerminate(const char* InMessage);
