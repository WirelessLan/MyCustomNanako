#include "Utils.h"

namespace Utils {
	std::string& Trim(std::string& a_str) {
		a_str.erase(a_str.begin(), std::find_if(a_str.begin(), a_str.end(), [](unsigned char ch) {
			return !std::isspace(ch);
			}));
		a_str.erase(std::find_if(a_str.rbegin(), a_str.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
			}).base(), a_str.end());
		return a_str;
	}

	std::string_view Trim(std::string_view a_str) {
		std::size_t sIdx, eIdx;

		for (sIdx = 0; sIdx < a_str.length(); ++sIdx) {
			if (!std::isspace(static_cast<unsigned char>(a_str[sIdx]))) {
				break;
			}
		}

		if (sIdx >= a_str.length()) {
			return std::string_view{};
		}

		for (eIdx = a_str.length(); eIdx > 0; --eIdx) {
			if (!std::isspace(static_cast<unsigned char>(a_str[eIdx - 1]))) {
				break;
			}
		}

		return a_str.substr(sIdx, eIdx - sIdx);
	}

	std::uint32_t ParseFormID(std::string_view a_formIdStr) {
		std::uint32_t retID = 0;
		try {
			retID = std::stoul(std::string(a_formIdStr), nullptr, 16) & 0xFFFFFF;
		}
		catch (...) {}
		return retID;
	}

	RE::TESForm* GetFormFromIdentifier(std::string_view a_pluginName, std::uint32_t a_formID) {
		RE::TESDataHandler* g_dataHandler = RE::TESDataHandler::GetSingleton();
		if (!g_dataHandler) {
			return nullptr;
		}
		return g_dataHandler->LookupForm(a_formID, a_pluginName);
	}

	RE::TESForm* GetFormFromIdentifier(std::string_view pluginName, std::string_view formIdStr) {
		return GetFormFromIdentifier(pluginName, ParseFormID(formIdStr));
	}

	RE::TESForm* GetFormFromString(std::string_view a_formStr) {
		auto delimiter = a_formStr.find('|');
		if (delimiter == std::string_view::npos) {
			return nullptr;
		}

		std::string_view pluginName = Trim(a_formStr.substr(0, delimiter));
		std::string_view formID = Trim(a_formStr.substr(delimiter + 1));

		return Utils::GetFormFromIdentifier(pluginName, formID);
	}
}
