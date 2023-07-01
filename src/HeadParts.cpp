#include "HeadParts.h"

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

	std::set<RE::BGSHeadPart*> g_originalHDPTSet;

	bool IsValidRace(RE::TESRace* a_race) {
		if (!a_race)
			return false;

		for (RE::TESRace* races : Configs::g_validRacesSet) {
			if (races == a_race)
				return true;
		}

		return false;
	}

	std::set<RE::BGSHeadPart*> GetHeadParts(std::uint32_t a_regionIndex, std::uint32_t a_selectedIndex) {
		auto it = Configs::g_morphGroupsMap.find(a_regionIndex);
		if (it == Configs::g_morphGroupsMap.end())
			return std::set<RE::BGSHeadPart*>();

		return it->second[a_selectedIndex].HeadPartSet;
	}

	void AddHeadPart(RE::TESNPC* a_npc, RE::BGSHeadPart* a_hdpt) {
		if (!a_npc || !a_hdpt)
			return;

		using func_t = void(*)(RE::TESNPC*, RE::BGSHeadPart*, std::uint32_t, std::uint32_t);
		const REL::Relocation<func_t> func{ REL::ID(735660) };
		func(a_npc, a_hdpt, 1, false);

	}

	void RemoveHeadPart(RE::TESNPC* a_npc, RE::BGSHeadPart* a_hdpt) {
		if (!a_npc || !a_hdpt)
			return;

		using func_t = void(*)(RE::TESNPC*, RE::BGSHeadPart*, bool);
		const REL::Relocation<func_t> func{ REL::ID(880456) };
		func(a_npc, a_hdpt, true);
	}

	void ChangeHeadPart(std::uint32_t a_regionIndex, std::uint32_t a_selectedIndex) {
		CharacterCreation* g_characterCreation = CharacterCreation::GetSingleton();
		if (!g_characterCreation)
			return;

		RE::TESNPC* npc = g_characterCreation->npc;
		if (!npc)
			return;

		if (!HeadParts::IsValidRace(npc->formRace))
			return;

		std::set<RE::BGSHeadPart*> headParts = HeadParts::GetHeadParts(a_regionIndex, a_selectedIndex);
		if (headParts.empty())
			return;

		std::set<RE::BGSHeadPart*> remTargetSet;
		auto it = Configs::g_headPartMap.find(a_regionIndex);
		if (it != Configs::g_headPartMap.end()) {
			for (std::size_t ii = 0; ii < npc->numHeadParts; ii++) {
				auto rem_iter = it->second.find(npc->headParts[ii]);
				if (rem_iter == it->second.end())
					continue;

				remTargetSet.insert(npc->headParts[ii]);
			}
		}

		bool originSet = false;
		if (!g_originalHDPTSet.empty())
			originSet = true;

		for (const auto& headPart : remTargetSet) {
			if (!originSet)
				g_originalHDPTSet.insert(headPart);
			HeadParts::RemoveHeadPart(npc, headPart);
		}

		for (const auto& headPart : headParts)
			HeadParts::AddHeadPart(npc, headPart);

		g_characterCreation->dirty = 1;
	}

	void RestoreHeadType(std::uint32_t a_regionIndex) {
		CharacterCreation* g_characterCreation = CharacterCreation::GetSingleton();
		if (!g_characterCreation)
			return;

		RE::TESNPC* npc = g_characterCreation->npc;
		if (!npc)
			return;

		if (!HeadParts::IsValidRace(npc->formRace))
			return;

		if (g_originalHDPTSet.empty())
			return;

		std::set<RE::BGSHeadPart*> remTargetSet;
		auto it = Configs::g_headPartMap.find(a_regionIndex);
		if (it != Configs::g_headPartMap.end()) {
			for (std::size_t ii = 0; ii < npc->numHeadParts; ii++) {
				auto rem_iter = it->second.find(npc->headParts[ii]);
				if (rem_iter == it->second.end())
					continue;

				remTargetSet.insert(npc->headParts[ii]);
			}
		}

		for (auto const& headPart : remTargetSet)
			HeadParts::RemoveHeadPart(npc, headPart);

		for (auto const& headPart : g_originalHDPTSet)
			HeadParts::AddHeadPart(npc, headPart);

		g_characterCreation->dirty = 1;

		g_originalHDPTSet.clear();
	}

	void ClearHeadPartSet() {
		g_originalHDPTSet.clear();
	}
}
