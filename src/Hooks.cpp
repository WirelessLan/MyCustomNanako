#include "Hooks.h"

#include "HeadParts.h"

namespace Hooks {
	std::uint32_t g_lastRegionIndex;

	using LooksMenu_Call_t = void(*)(RE::GameMenuBase*, RE::Scaleform::GFx::FunctionHandler::Params&);
	REL::Relocation<std::uintptr_t> LooksMenu_Call_Target(REL::ID(8371), 0x8);
	LooksMenu_Call_t LooksMenu_Call;

	void LooksMenu_Call_Hook(RE::GameMenuBase* a_menu, RE::Scaleform::GFx::FunctionHandler::Params& a_params) {
		std::uint64_t optionID = reinterpret_cast<std::uint64_t>(a_params.userData);

		if (optionID == 0x7) {
			if (a_params.argCount >= 2 && a_params.args[1].GetType() == RE::Scaleform::GFx::Value::ValueType::kUInt) {
				HeadParts::ChangeHeadPart(g_lastRegionIndex, a_params.args[1].GetUInt());
			}
		}
		else if (optionID == 0x18) {
			HeadParts::RestoreHeadType(g_lastRegionIndex);
		}
		else if (optionID == 0x19) {
			HeadParts::ClearHeadPartSet();
		}
		else if (optionID == 0x23) {
			if (a_params.argCount >= 3 && a_params.args[2].GetType() == RE::Scaleform::GFx::Value::ValueType::kUInt) {
				g_lastRegionIndex = a_params.args[2].GetUInt();
			}
		}

		LooksMenu_Call(a_menu, a_params);
	}

	void Install() {
		LooksMenu_Call = *reinterpret_cast<LooksMenu_Call_t*>(LooksMenu_Call_Target.get());
		REL::safe_write(LooksMenu_Call_Target.get(), (std::uintptr_t)LooksMenu_Call_Hook);
	}
}
