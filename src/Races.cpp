#include "Races.h"

#include "Configs.h"

namespace Races {
	enum Sex : std::uint32_t {
		kMale = 0,
		kFemale = 1
	};

	void UpdateRaces() {
		for (const auto& raceMorphGroups : Configs::g_raceMorphGroupsMap) {
			auto validRace = raceMorphGroups.first;

			if (validRace->faceRelatedData[Sex::kFemale]->facialBoneRegions) {
				for (auto faceBoneRegion : *validRace->faceRelatedData[Sex::kFemale]->facialBoneRegions) {
					for (const auto& indexRegionPair : Configs::g_regionNamesMap) {
						if (faceBoneRegion->id != indexRegionPair.first)
							continue;

						faceBoneRegion->name = indexRegionPair.second.VisibleName;
						break;
					}
				}
			}

			if (validRace->faceRelatedData[Sex::kFemale]->morphGroups) {
				for (const auto& morphPresets : raceMorphGroups.second) {
					auto it = Configs::g_regionNamesMap.find(morphPresets.first);
					if (it == Configs::g_regionNamesMap.end())
						continue;

					for (auto group : *validRace->faceRelatedData[Sex::kFemale]->morphGroups) {
						if (group->name != it->second.AssociatedMorphGroupName)
							continue;

						group->presets.clear();

						for (const auto& morphPreset : morphPresets.second) {
							RE::BGSCharacterMorph::Preset newPreset{};
							newPreset.id = morphPreset.Index;
							newPreset.name = morphPreset.Name;
							newPreset.flags = 1;

							group->presets.push_back(newPreset);
						}
					}
				}
			}
		}
	}

	void Update() {
		UpdateRaces();
	}
}
