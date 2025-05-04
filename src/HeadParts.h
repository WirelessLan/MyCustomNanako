#pragma once

#include <set>

namespace HeadParts {
	std::set<RE::BGSHeadPart*> GetHeadParts(RE::TESRace* a_race, std::uint32_t a_regionIndex, std::uint32_t a_selectedIndex);
	void AddHeadPart(RE::TESNPC* a_npc, RE::BGSHeadPart* a_hdpt);
	void RemoveHeadPart(RE::TESNPC* a_npc, RE::BGSHeadPart* a_hdpt);
	void ChangeHeadPart(std::uint32_t a_regionIndex, std::uint32_t a_selectedIndex);
	void RestoreHeadType(std::uint32_t a_regionIndex);
	void ClearHeadPartSet();
}
