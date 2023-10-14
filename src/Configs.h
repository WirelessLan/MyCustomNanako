#pragma once

#include <set>

namespace Configs {
	struct FacialBoneRegion {
		std::string AssociatedMorphGroupName;
		std::string VisibleName;
	};

	extern std::map<RE::TESRace*, std::map<std::uint32_t, FacialBoneRegion>> g_regionNamesMap;

	struct MorphPreset {
		std::string Name;
		std::set<RE::BGSHeadPart*> HeadPartSet;
	};

	extern std::map<RE::TESRace*, std::map<std::uint32_t, std::vector<MorphPreset>>> g_raceMorphGroupsMap;

	extern std::map<std::uint32_t, std::set<RE::BGSHeadPart*>> g_headPartMap;

	void ReadConfigs();
}
