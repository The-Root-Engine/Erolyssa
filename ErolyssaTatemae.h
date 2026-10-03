// Root Engine / Erolyssa

#pragma once

#include <stdio.h>
#include <stdarg.h>

class FErolyssaDebug
{
    
public:
#if defined(__RESHARPER__)
    [[jetbrains::format(printf, 1, 2)]]
#endif
    static void Log(const char* InFormat, ...)
    {
        va_list Args;
        va_start(Args, InFormat);
        
        printf("\033[0;37m[Erolyssa][Log] ");
        vprintf(InFormat, Args);  // NOLINT(clang-diagnostic-format-nonliteral)
        printf("\033[0m\n");
        
        va_end(Args);
    }
    
#if defined(__RESHARPER__)
    [[jetbrains::format(printf, 1, 2)]]
#endif
    static void Warning(const char* InFormat, ...)
    {
        va_list Args;
        va_start(Args, InFormat);
        
        printf("\033[93m[Erolyssa][Warning] ");
        vprintf(InFormat, Args);  // NOLINT(clang-diagnostic-format-nonliteral)
        printf("\033[0m\n");
        
        va_end(Args);
    }
    
#if defined(__RESHARPER__)
    [[jetbrains::format(printf, 1, 2)]]
#endif
    static void Error(const char* InFormat, ...)
    {
        va_list Args;
        va_start(Args, InFormat);
        
        printf("\033[91m[Erolyssa][Error] ");
        vprintf(InFormat, Args);  // NOLINT(clang-diagnostic-format-nonliteral)
        printf("\033[0m\n");
        
        va_end(Args);
    }
    
#if defined(__RESHARPER__)
    [[jetbrains::format(printf, 1, 2)]]
#endif
    static void Validation(const char* InFormat, ...)
    {
        va_list Args;
        va_start(Args, InFormat);
        
        printf("\033[96m[Erolyssa][Validation] ");
        vprintf(InFormat, Args);  // NOLINT(clang-diagnostic-format-nonliteral)
        printf("\033[0m\n");
        
        va_end(Args);
    }
    
#if defined(__RESHARPER__)
    [[jetbrains::format(printf, 1, 2)]]
#endif
    static void Terminate(const char* InFormat, ...)
    {
        va_list Args;
        va_start(Args, InFormat);
        
        printf("\033[1;91m[Erolyssa][Terminate] ");
        vprintf(InFormat, Args);  // NOLINT(clang-diagnostic-format-nonliteral)
        printf("\033[0m\n");
        
        va_end(Args);
        
        __debugbreak();
    }
};

#include "../Basement/Aliases/Common.h"
#include "../Basement/Macros/Assertion.h"
#include "../Basement/Misc/EnumClassFlags.h"
#include "../Basement/HAL/PlatformDebug.h"
#include "../Basement/Containers/Array.h"
#include "../Basement/Containers/StaticArray.h"
#include "../Basement/Meta.h"
#include "../Basement/Math/Math.h"

/*
// From "Root Engine / Basement"
#define check(LikelyExpression, Format, ...) \
    do { \
        if(!(LikelyExpression)) \
        { \
            FErolyssaDebug::Terminate(Format, ##__VA_ARGS__); \
        } \
    } while(false)

// From "Root Engine / Basement"
// Credit: Unreal Engine (Epic Games)

#pragma once

#define ENUM_CLASS_FLAGS(Enum) \
	inline           Enum& operator|=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	inline           Enum& operator&=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	inline           Enum& operator^=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator| (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator& (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator^ (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	inline constexpr bool  operator! (Enum  E)             { return !(__underlying_type(Enum))E; } \
	inline constexpr Enum  operator~ (Enum  E)             { return (Enum)~(__underlying_type(Enum))E; }

template<typename Enum> constexpr bool EnumHasAllFlags(Enum Flags, Enum Contains) { return (static_cast<__underlying_type(Enum)>(Flags) & static_cast<__underlying_type(Enum)>(Contains)) == static_cast<__underlying_type(Enum)>(Contains); }
template<typename Enum> constexpr bool EnumHasAnyFlags(Enum Flags, Enum Contains) { return (static_cast<__underlying_type(Enum)>(Flags) & static_cast<__underlying_type(Enum)>(Contains)) != 0; }

template<typename Enum> void EnumAddFlags(Enum& Flags, Enum FlagsToAdd) { Flags |= FlagsToAdd; }
template<typename Enum> void EnumRemoveFlags(Enum& Flags, Enum FlagsToRemove) { Flags &= ~FlagsToRemove; }
*/
