#pragma once
#include "ConfigParser.h"

extern ModConfigs modConfigs;
extern RE::ActorValue* avSingleton;

namespace Hooks
{
	void Install(F4SE::Trampoline& trampoline);
}
