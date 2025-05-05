#include "Races.h"

#include <set>

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

	std::uint32_t GetUniquePresetID(std::set<std::uint32_t>& faceMorphIDSet, std::uint32_t& presetID) {
		while (faceMorphIDSet.find(presetID) != faceMorphIDSet.end()) {
			presetID++;
		}
		faceMorphIDSet.insert(presetID);
		return presetID;
	}

	void UpdateRaceMorphGroupPresets() {
		std::set<std::uint32_t> faceMorphIDSet;
		std::uint32_t lastPresetID = 1;

		for (const auto& raceMorphGroups : Configs::g_raceMorphGroupsMap) {
			auto race = raceMorphGroups.first;

			for (auto sex : { Sex::kMale, Sex::kFemale }) {
				if (!race->faceRelatedData[sex] || !race->faceRelatedData[sex]->morphGroups) {
					continue;
				}

				for (auto group : *race->faceRelatedData[sex]->morphGroups) {
					for (auto preset : group->presets) {
						faceMorphIDSet.insert(preset.id);
					}
				}
			}
		}

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

						for (const auto& morphPreset : morphPresets.second) {
							RE::BGSCharacterMorph::Preset newPreset{};
							newPreset.id = GetUniquePresetID(faceMorphIDSet, lastPresetID);
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
		UpdateRaceRegionNames();
		UpdateRaceMorphGroupPresets();
	}
}
