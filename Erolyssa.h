// Root Engine / Erolyssa

#pragma once

#define EROLYSSA_ENABLE_VALIDATION_LAYERS 1
#define EROLYSSA_VLA(Type, Size) static_cast<Type*>(_alloca(Size * sizeof(Type)))
