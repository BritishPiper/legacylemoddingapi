#pragma once

import assembly;
import memory;
using namespace assembly;

namespace assembly_ {
	constexpr void NakedFunction(void) {}
}

#define Call(FUNCTION, ...) SetupShadowSpace(); \
					   FunctionCast(&FUNCTION, &FUNCTION ## _)(__VA_ARGS__); \
					   ClearShadowSpace();

#define CallEx(FUNCTION) SetupShadowSpaceEx(); \
						 FunctionCast(&FUNCTION, &assembly_::NakedFunction)(); \
					     ClearShadowSpaceEx();
