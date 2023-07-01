#include "Races.h"

#include "Configs.h"

namespace Races {
	void UpdateRaceFacialBoneRegionNames() {
		for (const auto& validRace : Configs::g_validRacesSet) {
			for (std::size_t sex = 0; sex < 2; sex++) {
				if (!validRace->faceRelatedData[sex]->facialBoneRegions)
					continue;

				for (auto faceBoneRegion : *validRace->faceRelatedData[sex]->facialBoneRegions) {
					for (const auto& indexRegionPair : Configs::g_regionNamesMap) {
						if (faceBoneRegion->id != indexRegionPair.first)
							continue;

						faceBoneRegion->name = indexRegionPair.second.VisibleName;
						break;
					}
				}
			}
		}
	}

	void UpdateRaceMorphGroups() {
		for (const auto& validRace : Configs::g_validRacesSet) {
			for (std::size_t sex = 0; sex < 2; sex++) {
				if (!validRace->faceRelatedData[sex]->morphGroups)
					continue;

				for (const auto& morphPresets : Configs::g_morphGroupsMap) {
					auto it = Configs::g_regionNamesMap.find(morphPresets.first);
					if (it == Configs::g_regionNamesMap.end())
						continue;

					for (auto group : *validRace->faceRelatedData[sex]->morphGroups) {
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
		UpdateRaceFacialBoneRegionNames();
		UpdateRaceMorphGroups();
	}
}
