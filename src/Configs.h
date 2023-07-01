#pragma once

#include <set>

namespace Configs {
	extern std::set<RE::TESRace*> g_validRacesSet;

	struct FacialBoneRegion {
		std::string AssociatedMorphGroupName;
		std::string VisibleName;
	};

	extern std::map<std::uint32_t, FacialBoneRegion> g_regionNamesMap;

	struct MorphPreset {
		std::string Name;
		std::uint32_t Index;
		std::set<RE::BGSHeadPart*> HeadPartSet;
	};

	extern std::map<std::uint32_t, std::vector<MorphPreset>> g_morphGroupsMap;

	extern std::map<std::uint32_t, std::set<RE::BGSHeadPart*>> g_headPartMap;

	void ReadConfigs();
}
