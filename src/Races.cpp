#include "Races.h"

#include "Configs.h"

namespace Races {
	enum Sex : std::uint32_t {
		kMale = 0,
		kFemale = 1,
	};

	void UpdateRaceRegionNames() {
		for (const auto& raceRegionNames : Configs::g_regionNamesMap) {
			auto race = raceRegionNames.first;

			for (auto sex : { Sex::kMale, Sex::kFemale }) {
				if (!race->faceRelatedData[sex] || !race->faceRelatedData[sex]->facialBoneRegions) {
					continue;
				}

				for (auto faceBoneRegion : *race->faceRelatedData[sex]->facialBoneRegions) {
					auto regionIt = raceRegionNames.second.find(faceBoneRegion->id);
					if (regionIt != raceRegionNames.second.end()) {
						faceBoneRegion->name = regionIt->second.VisibleName;
					}
				}
			}
		}
	}

	void UpdateRaceMorphGroupPresets() {
		for (const auto& raceMorphGroups : Configs::g_raceMorphGroupsMap) {
			auto race = raceMorphGroups.first;

			auto regionNamesMapIt = Configs::g_regionNamesMap.find(race);
			if (regionNamesMapIt == Configs::g_regionNamesMap.end()) {
				continue;
			}

			for (auto sex : { Sex::kMale, Sex::kFemale }) {
				if (!race->faceRelatedData[sex] || !race->faceRelatedData[sex]->morphGroups) {
					continue;
				}

				std::uint32_t groupID = 1;

				for (const auto& morphPresets : raceMorphGroups.second) {
					auto regionIt = regionNamesMapIt->second.find(morphPresets.first);
					if (regionIt == regionNamesMapIt->second.end()) {
						continue;
					}

					for (auto group : *race->faceRelatedData[sex]->morphGroups) {
						if (group->name != regionIt->second.AssociatedMorphGroupName) {
							continue;
						}

						group->presets.clear();

						std::uint32_t presetID = (groupID << 24) + 1;

						for (const auto& morphPreset : morphPresets.second) {
							RE::BGSCharacterMorph::Preset newPreset{};
							newPreset.id = presetID++;
							newPreset.name = morphPreset.Name;
							newPreset.flags = 1;

							group->presets.push_back(newPreset);
						}

						groupID++;
					}
				}
			}
		}
	}

	void Update() {
		UpdateRaceRegionNames();
		UpdateRaceMorphGroupPresets();
	}
}
