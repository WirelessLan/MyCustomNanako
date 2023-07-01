#include "Configs.h"

#include <fstream>
#include <regex>

#include "Utils.h"

namespace Configs {
	const std::string g_validRacesConfigPath = fmt::format("Data\\F4SE\\Plugins\\{}_ValidRaces.txt", Version::PROJECT);
	const std::string g_regionNamesConfigPath = fmt::format("Data\\F4SE\\Plugins\\{}_RegionNames.txt", Version::PROJECT);
	const std::string g_morphGroupsConfigPath = fmt::format("Data\\F4SE\\Plugins\\{}_MorphGroups", Version::PROJECT);

	std::set<RE::TESRace*> g_validRacesSet;
	std::map<std::uint32_t, FacialBoneRegion> g_regionNamesMap;
	std::map<std::uint32_t, std::vector<MorphPreset>> g_morphGroupsMap;
	std::map<std::uint32_t, std::set<RE::BGSHeadPart*>> g_headPartMap;

	std::uint8_t GetChar(const std::string& a_line, std::uint32_t& a_index) {
		if (a_index < a_line.length())
			return a_line[a_index++];
		return 0xFF;
	}

	std::string GetNextData(const std::string& a_line, std::uint32_t& a_index, char a_delimeter) {
		std::uint8_t ch;
		std::string retVal;

		while ((ch = GetChar(a_line, a_index)) != 0xFF) {
			if (ch == '#') {
				if (a_index > 0)
					a_index--;
				break;
			}

			if (a_delimeter != 0 && ch == a_delimeter)
				break;

			retVal += ch;
		}

		return Utils::Trim(retVal);
	}

	bool GetLine(std::ifstream& a_file, std::string& a_line) {
		if (std::getline(a_file, a_line)) {
			Utils::Trim(a_line);
			return true;
		}
		return false;
	}

	bool EmptyOrComment(const std::string& a_line) {
		if (a_line.empty() || a_line[0] == '#')
			return true;
		return false;
	}

	void ReadValidRacesConfig() {
		std::ifstream file{ g_validRacesConfigPath };
		if (!file.is_open()) {
			logger::error("Cannot read the config file: {}", g_validRacesConfigPath);
			return;
		}

		logger::info("Reading the config file: {}", g_validRacesConfigPath);

		std::string line;
		std::uint32_t lineNum = 0;
		while (GetLine(file, line)) {
			lineNum++;

			if (EmptyOrComment(line))
				continue;

			std::uint32_t lineIndex = 0;
			std::string raceFormStr;

			raceFormStr = GetNextData(line, lineIndex, 0);
			if (raceFormStr.empty()) {
				logger::error("Line {}: Cannot read the Race: {}", lineNum, line);
				continue;
			}

			RE::TESForm* raceForm = Utils::GetFormFromString(raceFormStr);
			if (!raceForm) {
				logger::error("Line {}: '{}' is an invalid form: {}", lineNum, raceFormStr, line);
				continue;
			}

			RE::TESRace* race = raceForm->As<RE::TESRace>();
			if (!race) {
				logger::error("Line {}: '{}' is not a valid Race: {}", lineNum, raceFormStr, line);
				continue;
			}

			g_validRacesSet.insert(race);
		}
	}

	void ReadRegionNamesConfig() {
		std::ifstream file{ g_regionNamesConfigPath };
		if (!file.is_open()) {
			logger::error("Cannot read the config file: {}", g_regionNamesConfigPath);
			return;
		}

		logger::info("Reading the config file: {}", g_regionNamesConfigPath);

		std::string line;
		std::uint32_t lineNum = 0;
		while (GetLine(file, line)) {
			lineNum++;

			if (EmptyOrComment(line))
				continue;

			std::uint32_t lineIndex = 0;
			std::string facialBoneRegionIndexStr, morphGroupName, name;

			facialBoneRegionIndexStr = GetNextData(line, lineIndex, ':');
			if (facialBoneRegionIndexStr.empty()) {
				logger::error("Line {}: Cannot read the FacialBoneRegionIndex: {}", lineNum, line);
				continue;
			}

			morphGroupName = GetNextData(line, lineIndex, ',');
			if (morphGroupName.empty()) {
				logger::error("Line {}: Cannot read the MorphGroupName: {}", lineNum, line);
				continue;
			}

			name = GetNextData(line, lineIndex, 0);
			if (name.empty()) {
				logger::error("Line {}: Cannot read the Name: {}", lineNum, line);
				continue;
			}

			std::uint32_t facialBoneRegionIndex;
			try {
				facialBoneRegionIndex = std::stoul(facialBoneRegionIndexStr, nullptr, 16);
			}
			catch (...) {
				logger::error("Line {}: Failed to parse a FacialBoneRegionIndex: {}", lineNum, line.c_str());
				continue;
			}

			FacialBoneRegion newRegion{};
			newRegion.AssociatedMorphGroupName = morphGroupName;
			newRegion.VisibleName = name;

			g_regionNamesMap[facialBoneRegionIndex] = newRegion;
		}
	}

	void ReadMorphGroupsConfig() {
		if (!std::filesystem::exists(g_morphGroupsConfigPath)) {
			logger::error("Config directory does not exist: {}", g_morphGroupsConfigPath);
			return;
		}

		const std::regex filter(".*\\.txt", std::regex_constants::icase);
		const std::filesystem::directory_iterator dir_iter(g_morphGroupsConfigPath);

		for (auto& iter : dir_iter) {
			if (!std::filesystem::is_regular_file(iter.status()))
				continue;

			if (!std::regex_match(iter.path().filename().string(), filter))
				continue;

			std::string filePath = iter.path().string();

			std::ifstream file{ filePath };
			if (!file.is_open()) {
				logger::error("Cannot read the config file: {}", filePath);
				return;
			}

			logger::info("Reading the config file: {}", filePath);

			std::string line;
			std::uint32_t lineNum = 0;
			while (GetLine(file, line)) {
				lineNum++;

				if (EmptyOrComment(line))
					continue;

				std::uint32_t lineIndex = 0;
				std::string facialBoneRegionIndexStr, morphPresetIndexStr, morphPresetName, headPartListStart;

				facialBoneRegionIndexStr = GetNextData(line, lineIndex, ':');
				if (facialBoneRegionIndexStr.empty()) {
					logger::error("Line {}: Cannot read the FacialBoneRegionIndex : {}", lineNum, line);
					continue;
				}

				morphPresetIndexStr = GetNextData(line, lineIndex, ',');
				if (morphPresetIndexStr.empty()) {
					logger::error("Line {}: Cannot read the MorphPresetIndex: {}", lineNum, line);
					continue;
				}

				morphPresetName = GetNextData(line, lineIndex, ',');
				if (morphPresetName.empty()) {
					logger::error("Line {}: Cannot read the MorphPresetName: {}", lineNum, line);
					continue;
				}

				headPartListStart = GetNextData(line, lineIndex, 0);
				if (headPartListStart.empty() || headPartListStart != "[") {
					logger::error("Line {}: '[' Expected: {}", lineNum, line);
					continue;
				}

				std::uint32_t facialBoneRegionIndex;
				try {
					facialBoneRegionIndex = std::stoul(facialBoneRegionIndexStr, nullptr, 16);
				}
				catch (...) {
					logger::error("Line {}: Failed to parse a FacialBoneRegionIndex: {}", lineNum, line);
					continue;
				}

				std::uint32_t morphPresetIndex;
				try {
					morphPresetIndex = std::stoul(morphPresetIndexStr, nullptr, 16);
				}
				catch (...) {
					logger::error("Line {}: Failed to parse a MorphPresetIndex: {}", lineNum, line.c_str());
					continue;
				}

				std::vector<RE::BGSHeadPart*> headPartList;
				while (GetLine(file, line)) {
					lineNum++;

					if (EmptyOrComment(line))
						continue;

					lineIndex = 0;

					std::string data = GetNextData(line, lineIndex, 0);
					if (data == "]")
						break;

					RE::TESForm* headPartForm = Utils::GetFormFromString(data);
					if (!headPartForm) {
						logger::error("Line {}: '{}' is an invalid form: {}", lineNum, data, line);
						continue;
					}

					RE::BGSHeadPart* headPart = headPartForm->As<RE::BGSHeadPart>();
					if (!headPart) {
						logger::error("Line {}: '{}' is not a valid HeadPart: {}", lineNum, data, line);
						continue;
					}

					headPartList.push_back(headPart);
				}

				auto result = g_morphGroupsMap.insert(std::make_pair(facialBoneRegionIndex, std::vector<MorphPreset>()));
				auto it = result.first;

				bool found = false;
				for (MorphPreset& preset : it->second) {
					if (preset.Index != morphPresetIndex)
						continue;

					found = true;
					preset.Name = morphPresetName;
					for (auto headPart : headPartList)
						preset.HeadPartSet.insert(headPart);

					break;
				}

				if (found) {
					logger::error("Preset index {:08X} already exists in FacialBoneRegion {:08X}", morphPresetIndex, facialBoneRegionIndex);
					continue;
				}

				MorphPreset newPreset{};
				newPreset.Index = morphPresetIndex;
				newPreset.Name = morphPresetName;
				for (auto headPart : headPartList)
					newPreset.HeadPartSet.insert(headPart);

				it->second.push_back(newPreset);
			}
		}
	}

	void SetHeadPartMap() {
		for (const auto& indexRegionPair : g_regionNamesMap) {
			auto it = g_morphGroupsMap.find(indexRegionPair.first);
			if (it == g_morphGroupsMap.end())
				continue;

			auto result = g_headPartMap.insert(std::make_pair(indexRegionPair.first, std::set<RE::BGSHeadPart*>()));
			auto hdptMap_iter = result.first;

			for (const auto& morphPreset : it->second)
				for (const auto& headPart : morphPreset.HeadPartSet)
					hdptMap_iter->second.insert(headPart);
		}
	}

	void ReadConfigs() {
		ReadValidRacesConfig();
		ReadRegionNamesConfig();
		ReadMorphGroupsConfig();
		SetHeadPartMap();
	}
}
