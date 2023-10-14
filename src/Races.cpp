#include "Races.h"

#include "Configs.h"

namespace Races {
	enum Sex : std::uint32_t {
		kMale = 0,
		kFemale = 1,
		kTotal
	};

	void UpdateRaces() {
		for (const auto& raceRegionNames : Configs::g_regionNamesMap) {
			auto validRace = raceRegionNames.first;

			for (std::uint32_t sex = kMale; sex < kTotal; sex++) {
				if (!validRace->faceRelatedData[sex]->facialBoneRegions)
					continue;

				for (auto faceBoneRegion : *validRace->faceRelatedData[sex]->facialBoneRegions) {
					for (const auto& indexRegionPair : raceRegionNames.second) {
						if (faceBoneRegion->id != indexRegionPair.first)
							continue;

						faceBoneRegion->name = indexRegionPair.second.VisibleName;
						break;
					}
				}
			}
		}

		for (const auto& raceMorphGroups : Configs::g_raceMorphGroupsMap) {
			auto validRace = raceMorphGroups.first;

			for (std::uint32_t sex = kMale; sex < kTotal; sex++) {
				if (!validRace->faceRelatedData[sex]->morphGroups)
					continue;

				std::uint32_t groupID = 1;

				for (const auto& morphPresets : raceMorphGroups.second) {
					auto race_it = Configs::g_regionNamesMap.find(validRace);
					if (race_it == Configs::g_regionNamesMap.end())
						continue;

					auto region_it = race_it->second.find(morphPresets.first);
					if (region_it == race_it->second.end())
						continue;

					for (auto group : *validRace->faceRelatedData[sex]->morphGroups) {
						if (group->name != region_it->second.AssociatedMorphGroupName)
							continue;

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
		UpdateRaces();
	}
}
