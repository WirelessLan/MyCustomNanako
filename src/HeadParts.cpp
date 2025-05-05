#include "HeadParts.h"

#include <mutex>

#include "Configs.h"

namespace HeadParts {
	class CharacterCreation
	{
	public:
		std::uint64_t	unk00;		// 00
		void* unk08;				// 08
		std::uint64_t	unk10;		// 10		4
		std::uint64_t	unk18;		// 18		1
		std::uint64_t	unk20;		// 20
		std::uint64_t	unk28;		// 28
		std::uint64_t	unk30;		// 30
		std::uint64_t	unk38;		// 38
		std::uint64_t	unk40;		// 40
		std::uint64_t	unk48;		// 48
		std::uint64_t	unk50;		// 50
		RE::Actor*	actor;			// 58
		RE::TESNPC*	npc;			// 60
		RE::TESNPC*	npc2;			// 68
		std::uint64_t	unk70[(0x510 - 0x70) / 8];	// 70
		std::uint32_t	presetIndex;				// 510
		std::uint16_t	unk514;						// 514
		std::uint8_t	unk516;						// 516 - weight change?
		std::uint8_t	unk517;						// 517 - face dirty?
		std::uint8_t	dirty;						// 518

		static CharacterCreation* GetSingleton() {
			REL::Relocation<CharacterCreation**> singleton{ REL::ID(933771) };
			return *singleton;
		}
	};


	std::mutex g_headPartMutex;
	std::set<RE::BGSHeadPart*> g_prevHeadPartSet;

	std::set<RE::BGSHeadPart*> GetHeadParts(RE::TESRace* a_race, std::uint32_t a_regionIndex, std::uint32_t a_selectedIndex) {
		auto race_it = Configs::g_raceMorphGroupsMap.find(a_race);
		if (race_it == Configs::g_raceMorphGroupsMap.end()) {
			return std::set<RE::BGSHeadPart*>();
		}

		auto region_it = race_it->second.find(a_regionIndex);
		if (region_it == race_it->second.end()) {
			return std::set<RE::BGSHeadPart*>();
		}

		auto& presets = region_it->second;
		if (a_selectedIndex >= presets.size()) {
			return std::set<RE::BGSHeadPart*>();
		}

		return region_it->second[a_selectedIndex].HeadPartSet;
	}

	void AddHeadPart(RE::TESNPC* a_npc, RE::BGSHeadPart* a_hdpt) {
		if (!a_npc || !a_hdpt) {
			return;
		}

		using func_t = void(*)(RE::TESNPC*, RE::BGSHeadPart*, std::uint32_t, std::uint32_t);
		const REL::Relocation<func_t> func{ REL::ID(735660) };
		func(a_npc, a_hdpt, 1, false);
	}

	void RemoveHeadPart(RE::TESNPC* a_npc, RE::BGSHeadPart* a_hdpt) {
		if (!a_npc || !a_hdpt) {
			return;
		}

		using func_t = void(*)(RE::TESNPC*, RE::BGSHeadPart*, bool);
		const REL::Relocation<func_t> func{ REL::ID(880456) };
		func(a_npc, a_hdpt, true);
	}

	void ChangeHeadPart(std::uint32_t a_regionIndex, std::uint32_t a_selectedIndex) {
		std::lock_guard<std::mutex> lock(g_headPartMutex);

		CharacterCreation* g_characterCreation = CharacterCreation::GetSingleton();
		if (!g_characterCreation) {
			return;
		}

		RE::TESNPC* npc = g_characterCreation->npc;
		if (!npc) {
			return;
		}

		std::set<RE::BGSHeadPart*> headParts = HeadParts::GetHeadParts(npc->formRace, a_regionIndex, a_selectedIndex);
		if (headParts.empty()) {
			return;
		}

		std::vector<RE::BGSHeadPart*> headPartsToRemove;
		bool prevHeadPartsSetExists = !g_prevHeadPartSet.empty();

		auto headPartMapIt = Configs::g_headPartMap.find(a_regionIndex);
		if (headPartMapIt != Configs::g_headPartMap.end()) {
			const auto& headPartSet = headPartMapIt->second;

			for (std::size_t ii = 0; ii < npc->numHeadParts; ii++) {
				RE::BGSHeadPart* headPart = npc->headParts[ii];

				if (headPartSet.find(headPart) == headPartSet.end()) {
					continue;
				}

				if (!prevHeadPartsSetExists) {
					g_prevHeadPartSet.insert(headPart);
				}

				headPartsToRemove.push_back(headPart);
			}
		}

		for (auto headPart : headPartsToRemove) {
			HeadParts::RemoveHeadPart(npc, headPart);
		}

		for (auto headPart : headParts) {
			HeadParts::AddHeadPart(npc, headPart);
		}

		g_characterCreation->dirty = 1;
	}

	void RestoreHeadType(std::uint32_t a_regionIndex) {
		std::lock_guard<std::mutex> lock(g_headPartMutex);

		CharacterCreation* g_characterCreation = CharacterCreation::GetSingleton();
		if (!g_characterCreation) {
			return;
		}

		RE::TESNPC* npc = g_characterCreation->npc;
		if (!npc) {
			return;
		}

		if (g_prevHeadPartSet.empty()) {
			return;
		}

		std::vector<RE::BGSHeadPart*> headPartsToRemove;

		auto headPartMapIt = Configs::g_headPartMap.find(a_regionIndex);
		if (headPartMapIt != Configs::g_headPartMap.end()) {
			const auto& headPartSet = headPartMapIt->second;

			for (std::size_t ii = 0; ii < npc->numHeadParts; ii++) {
				RE::BGSHeadPart* headPart = npc->headParts[ii];

				if (headPartSet.find(headPart) == headPartSet.end()) {
					continue;
				}

				headPartsToRemove.push_back(headPart);
			}
		}

		for (auto headPart : headPartsToRemove) {
			HeadParts::RemoveHeadPart(npc, headPart);
		}

		for (auto headPart : g_prevHeadPartSet) {
			HeadParts::AddHeadPart(npc, headPart);
		}
		g_prevHeadPartSet.clear();

		g_characterCreation->dirty = 1;
	}

	void ClearHeadPartSet() {
		g_prevHeadPartSet.clear();
	}
}
