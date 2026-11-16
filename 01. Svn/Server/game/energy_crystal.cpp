/*********************************************************************
 * File Name : energy_crystal.cpp
 * Author    : BestStudio
 * Version   : 1.0.0
 * Date      : 15-11-2026
 */

#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_ENERGY_CRYSTAL
#include "utils.h"
#include "config.h"
#include "constants.h"
#include "char.h"
#include "char_manager.h"
#include "desc.h"
#include "item.h"
#include "item_manager.h"
#include "affect.h"
#include "locale_service.h"
#include "energy_crystal.h"

#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <string>
#include <cmath>

namespace
{
	const DWORD CRYSTAL_VNUM_FIRST = 51040;	// +0
	const DWORD CRYSTAL_VNUM_LAST = 51049;		// +9
	const DWORD UNLOCKER_VNUM = 51052;
	const DWORD BLESSING_SCROLL_VNUM = 51050;	// failure: grade - 1
	const DWORD MAGIC_STONE_VNUM = 51051;		// failure: grade kept
	const DWORD CATALYST_PLUS_VNUM = 51058;
	const int UNLOCK_LEVEL = 106;
	const int BONUS_SLOT_MAX = 5;

	enum { SOCKET_ENERGY, SOCKET_RATE, SOCKET_ACTIVE };

	// artifact (unlocker) vnum -> our apply (user decision: nearest existing bonus)
	struct SArtifact { DWORD vnum; WORD apply; };	// applies go beyond 255
	const SArtifact ARTIFACTS[] =
	{
		{ 51031, APPLY_ATTBONUS_MEDI_PVM },	// Ortalama Hasar PvM
		{ 51032, APPLY_ATTBONUS_DUNGEON },		// Zindanlarda Guc
		{ 51033, APPLY_ATTBONUS_EREBUS },		// Erebus'a Karsi Guc
		{ 51034, APPLY_ATTBONUS_MELEY },		// Meley'e Karsi Guc
		{ 51035, APPLY_ENCHANT_ELEMENTS },		// Element Korumasi
	};

	// energy_crystal.txt (locale base path)
	struct SConfig
	{
		double	catalystSkew;		// roll = lo + (hi - lo) * u^skew (higher = rarer high values)
		int		combatSeconds;
		bool	loaded;
		SConfig() : catalystSkew(4.0), combatSeconds(10), loaded(false) {}
	} g_config;

	std::set<DWORD> g_activePids;
	LPEVENT g_tickEvent = NULL;

	void LoadConfig()
	{
		if (g_config.loaded)
			return;
		g_config.loaded = true;

		char szPath[256];
		snprintf(szPath, sizeof(szPath), "%s/energy_crystal.txt", LocaleService_GetBasePath().c_str());
		std::ifstream file(szPath);
		if (!file.is_open())
		{
			sys_log(0, "EnergyCrystal: %s not found, defaults used", szPath);
			return;
		}

		std::string line;
		while (std::getline(file, line))
		{
			if (line.empty() || line[0] == '#')
				continue;
			std::istringstream ss(line);
			std::string key;
			ss >> key;
			if (key == "catalyst_skew")
				ss >> g_config.catalystSkew;
			else if (key == "combat_seconds")
				ss >> g_config.combatSeconds;
		}
	}

	void Notify(LPCHARACTER ch, const char* key)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "EnergyCrystalMsg %s", key);
	}

	WORD ArtifactApply(DWORD vnum)
	{
		for (size_t i = 0; i < _countof(ARTIFACTS); ++i)
			if (ARTIFACTS[i].vnum == vnum)
				return ARTIFACTS[i].apply;
		return APPLY_NONE;
	}

	int CapOf(LPITEM crystal)		{ return crystal->GetValue(0) / 10; }			// value0 = cap x100 -> 0.1 % units
	int EnergyMaxOf(LPITEM crystal)	{ return crystal->GetValue(1) > 0 ? crystal->GetValue(1) : 86400; }

	LPITEM FindCrystal(LPCHARACTER ch)
	{
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item = ch->GetInventoryItem(i);
			if (item && EnergyCrystal::IsCrystal(item->GetVnum()))
				return item;
		}
		return NULL;
	}

	bool IsInCombat(LPCHARACTER ch)
	{
		const DWORD now = get_dword_time();
		const DWORD last = ch->GetLastAttackTime();
		return last && last <= now && now - last < (DWORD)(g_config.combatSeconds * 1000);
	}

	// ----------------------------------------------------------------- bonus affect
	void ClearBonus(LPCHARACTER ch)
	{
		while (ch->FindAffect(AFFECT_ENERGY_CRYSTAL))
			ch->RemoveAffect(AFFECT_ENERGY_CRYSTAL);
	}

	std::map<DWORD, std::string> g_sentBonus;

	bool IsUnlocked(LPITEM crystal, WORD apply)
	{
		for (int i = 0; crystal && i < BONUS_SLOT_MAX; ++i)
			if (crystal->GetAttributeType(i) == apply)
				return true;
		return false;
	}

	void RefreshBonus(LPCHARACTER ch, LPITEM crystal)
	{
		const bool on = crystal && crystal->GetSocket(SOCKET_ACTIVE) && crystal->GetSocket(SOCKET_ENERGY) > 0;
		const int rate = crystal ? crystal->GetSocket(SOCKET_RATE) : 0;	// 0.1 % units
		std::string list;

		for (size_t i = 0; crystal && i < _countof(ARTIFACTS); ++i)
		{
			const WORD apply = ARTIFACTS[i].apply;
			const BYTE point = aApplyInfo[apply].bPointType;
			if (point == POINT_NONE)
				continue;
			const bool unlocked = IsUnlocked(crystal, apply);
			CAffect* aff = ch->FindAffect(AFFECT_ENERGY_CRYSTAL, point);
			const long own = aff ? aff->lApplyValue : 0;
			const long base = ch->GetPoint(point) - own;

			long gain = 0;
			if (base > 0 && rate > 0)
				gain = MAX(1L, (long)floor(base * rate / 1000.0 + 0.5));

			char part[48];
			snprintf(part, sizeof(part), "%s%u,%ld,%d", list.empty() ? "" : "|", apply, gain, unlocked ? 1 : 0);
			list += part;

			if (!unlocked)
			{
				if (aff)
					ch->RemoveAffect(aff);
				continue;
			}
			if (!on || gain == own)
				continue;
			if (aff)
				ch->RemoveAffect(aff);
			if (gain > 0)
				ch->AddAffect(AFFECT_ENERGY_CRYSTAL, point, gain, 0, INFINITE_AFFECT_DURATION, 0, false);
		}

		if (!on)
			ClearBonus(ch);

		const std::string msg = std::string("EnergyCrystalBonus ") + (list.empty() ? "-" : list);
		std::string& sent = g_sentBonus[ch->GetPlayerID()];
		if (sent != msg)
		{
			sent = msg;
			ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", msg.c_str());
		}
	}

	// ----------------------------------------------------------------- energy tick (1 s, all active players)
	EVENTINFO(energy_crystal_event_info)
	{
		int dummy;
		energy_crystal_event_info() : dummy(0) {}
	};

	EVENTFUNC(energy_crystal_tick)
	{
		static int s_counter = 0;
		++s_counter;

		for (std::set<DWORD>::iterator it = g_activePids.begin(); it != g_activePids.end(); )
		{
			LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(*it);
			LPITEM crystal = ch ? FindCrystal(ch) : NULL;
			if (!ch || !crystal || !crystal->GetSocket(SOCKET_ACTIVE))
			{
				if (ch)
					ClearBonus(ch);
				g_activePids.erase(it++);
				continue;
			}

			if (IsInCombat(ch))
			{
				const int energy = MAX(0, crystal->GetSocket(SOCKET_ENERGY) - 1);
				crystal->SetSocket(SOCKET_ENERGY, energy, false);
				if (energy == 0)
				{
					crystal->SetSocket(SOCKET_ACTIVE, 0, false);
					ClearBonus(ch);
					Notify(ch, "ENERGY_CRYSTAL_EXPIRED");
					g_activePids.erase(it++);
					continue;
				}
			}

			if (s_counter % 5 == 0)
				RefreshBonus(ch, crystal);
			++it;
		}

		if (g_activePids.empty())
		{
			g_tickEvent = NULL;
			return 0;
		}
		return PASSES_PER_SEC(1);
	}

	void StartTick(LPCHARACTER ch)
	{
		g_activePids.insert(ch->GetPlayerID());
		if (!g_tickEvent)
		{
			energy_crystal_event_info* info = AllocEventInfo<energy_crystal_event_info>();
			g_tickEvent = event_create(energy_crystal_tick, info, PASSES_PER_SEC(1));
		}
	}

	// ----------------------------------------------------------------- actions
	bool ConsumeOne(LPITEM item)
	{
		item->SetCount(item->GetCount() - 1);
		return true;
	}

	bool Toggle(LPCHARACTER ch, LPITEM crystal)
	{
		if (crystal->GetSocket(SOCKET_ACTIVE))
		{
			crystal->SetSocket(SOCKET_ACTIVE, 0);
			RefreshBonus(ch, crystal);
			g_activePids.erase(ch->GetPlayerID());
			return true;
		}
		if (crystal->GetSocket(SOCKET_ENERGY) <= 0)
		{
			Notify(ch, "ENERGY_CRYSTAL_EXPIRED");
			return false;
		}
		crystal->SetSocket(SOCKET_ACTIVE, 1);
		RefreshBonus(ch, crystal);
		StartTick(ch);
		return true;
	}

	bool Recharge(LPCHARACTER ch, LPITEM crystal, LPITEM elixir)
	{
		const int maxEnergy = EnergyMaxOf(crystal);
		const int energy = crystal->GetSocket(SOCKET_ENERGY);
		if (energy >= maxEnergy)
		{
			Notify(ch, "ENERGY_CRYSTAL_FULL");
			return false;
		}
		const int add = elixir->GetValue(0) > 0 ? elixir->GetValue(0) : maxEnergy;
		crystal->SetSocket(SOCKET_ENERGY, MIN(maxEnergy, energy + add));
		return ConsumeOne(elixir);
	}

	int Roll(int lo, int hi)
	{
		if (hi <= lo)
			return lo;
		const double u = number(0, 100000) / 100000.0;
		return lo + (int)floor((hi - lo) * pow(u, g_config.catalystSkew) + 0.5);
	}

	bool CatalystPlus(LPCHARACTER ch, LPITEM crystal, LPITEM catalyst)
	{
		const int rate = crystal->GetSocket(SOCKET_RATE);	// 0.1 % units
		int gain = 0;
		if (rate >= 200 && rate <= 230)
			gain = 4;
		else if (rate >= 300 && rate <= 330)
			gain = 3;
		else if (rate >= 400 && rate <= 430)
			gain = 2;
		else if (rate >= 480)
			gain = 1;
		if (!gain || rate + gain > CapOf(crystal))
		{
			Notify(ch, "ENERGY_CRYSTAL_PLUS_HINT");
			return false;
		}
		crystal->SetSocket(SOCKET_RATE, rate + gain);
		RefreshBonus(ch, crystal);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "EnergyCrystalRate %d", rate + gain);
		return ConsumeOne(catalyst);
	}

	bool Catalyst(LPCHARACTER ch, LPITEM crystal, LPITEM catalyst)
	{
		if (catalyst->GetVnum() == CATALYST_PLUS_VNUM)
			return CatalystPlus(ch, crystal, catalyst);
		const int cap = CapOf(crystal);
		const int lo = catalyst->GetValue(0) / 10;
		const int hi = MIN(cap, catalyst->GetValue(1) / 10);
		if (cap < lo || hi < lo)
		{
			Notify(ch, "ENERGY_CRYSTAL_SWITCH_CAP_REQUIRED");
			return false;
		}
		const int rate = Roll(lo, hi);
		crystal->SetSocket(SOCKET_RATE, rate);
		RefreshBonus(ch, crystal);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "EnergyCrystalRate %d", rate);
		return ConsumeOne(catalyst);
	}

	bool Unlock(LPCHARACTER ch, LPITEM crystal, LPITEM artifact)
	{
		const WORD apply = ArtifactApply(artifact->GetVnum());
		if (apply == APPLY_NONE)
			return false;
		int freeSlot = -1;
		for (int i = 0; i < BONUS_SLOT_MAX; ++i)
		{
			const WORD t = crystal->GetAttributeType(i);
			if (t == apply)
			{
				Notify(ch, "ENERGY_CRYSTAL_DUPLICATE");
				return false;
			}
			if (t == APPLY_NONE && freeSlot < 0)
				freeSlot = i;
		}
		if (freeSlot < 0)
		{
			Notify(ch, "ENERGY_CRYSTAL_NO_SLOT");
			return false;
		}
		crystal->SetForceAttribute(freeSlot, apply, 1);
		RefreshBonus(ch, crystal);
		Notify(ch, "ENERGY_CRYSTAL_BONUS_UNLOCKED");
		return ConsumeOne(artifact);
	}

	bool UnlockCrystal(LPCHARACTER ch, LPITEM unlocker)
	{
		if (ch->GetLevel() < UNLOCK_LEVEL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your level is too low to use this item."));
			return false;
		}
		if (FindCrystal(ch))
		{
			Notify(ch, "ENERGY_CRYSTAL_ALREADY_OWNED");
			return false;
		}
		const int cell = ch->GetEmptyInventory(1);
		if (cell < 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have enough space in your inventory."));
			return false;
		}
		LPITEM crystal = ITEM_MANAGER::instance().CreateItem(CRYSTAL_VNUM_FIRST, 1, 0, true);
		if (!crystal)
			return false;
		crystal->SetSocket(SOCKET_ENERGY, EnergyMaxOf(crystal), false);
		crystal->SetSocket(SOCKET_RATE, 0, false);
		crystal->SetSocket(SOCKET_ACTIVE, 0, false);
		crystal->AddToCharacter(ch, TItemPos(INVENTORY, cell));
		ITEM_MANAGER::instance().FlushDelayedSave(crystal);
		return ConsumeOne(unlocker);
	}

	int ApplyToCrystal(LPCHARACTER ch, LPITEM crystal, LPITEM item)
	{
		LoadConfig();
		if (item->GetType() != ITEM_BLEND)
			return -1;
		switch (item->GetSubType())
		{
			case BLEND_ENERGY_RECHARGE:		return Recharge(ch, crystal, item);
			case BLEND_ENERGY_CATALYST:		return Catalyst(ch, crystal, item);
			case BLEND_ENERGY_ARTIFACT:		return Unlock(ch, crystal, item);
		}
		return -1;
	}

	bool IsEnergyItem(LPITEM item)
	{
		if (item->GetType() != ITEM_BLEND)
			return false;
		const BYTE sub = item->GetSubType();
		return sub == BLEND_ENERGY_RECHARGE || sub == BLEND_ENERGY_CATALYST || sub == BLEND_ENERGY_ARTIFACT;
	}
}

bool EnergyCrystal::IsCrystal(DWORD vnum)
{
	return vnum >= CRYSTAL_VNUM_FIRST && vnum <= CRYSTAL_VNUM_LAST;
}

bool EnergyCrystal::IsCrystalScroll(DWORD vnum)
{
	return vnum == BLESSING_SCROLL_VNUM || vnum == MAGIC_STONE_VNUM;
}

void EnergyCrystal::CopyCrystalState(LPITEM from, LPITEM to)
{
	if (!from || !to || !IsCrystal(to->GetVnum()))
		return;
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		to->SetSocket(i, from->GetSocket(i), false);
	if (to->GetSocket(SOCKET_RATE) > CapOf(to))
		to->SetSocket(SOCKET_RATE, CapOf(to), false);
}

void EnergyCrystal::OnCrystalChanged(LPCHARACTER ch)
{
	OnLogin(ch);
}

int EnergyCrystal::OnUseItem(LPCHARACTER ch, LPITEM item, TItemPos DestCell)
{
	if (!ch || !item)
		return -1;

	if (item->GetVnum() == UNLOCKER_VNUM)
		return UnlockCrystal(ch, item);

	if (IsCrystal(item->GetVnum()))
		return 0;

	if (!IsEnergyItem(item))
		return -1;

	LPITEM target = NULL;
	if (DestCell.IsValidItemPosition())
		target = ch->GetItem(DestCell);
	if (!target)
		target = FindCrystal(ch);
	if (!target || !IsCrystal(target->GetVnum()))
	{
		Notify(ch, "ENERGY_CRYSTAL_INVALID_TARGET");
		return 0;
	}
	const int r = ApplyToCrystal(ch, target, item);
	sys_log(0, "ENERGY_CRYSTAL: %s drop item %u on crystal %u -> %d", ch->GetName(), item->GetVnum(), target->GetVnum(), r);
	return r < 0 ? 0 : r;
}

void EnergyCrystal::OnCommand(LPCHARACTER ch, const char* argument)
{
	char arg1[64], arg2[64], arg3[64];
	const char* rest = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(rest, arg3, sizeof(arg3));

	if (!ch || !*arg1 || !*arg2)
		return;

	WORD cell = 0;
	str_to_number(cell, arg2);
	LPITEM crystal = cell < INVENTORY_MAX_NUM ? ch->GetInventoryItem(cell) : NULL;
	if (!crystal || !IsCrystal(crystal->GetVnum()))
	{
		Notify(ch, "ENERGY_CRYSTAL_NOT_OWNED");
		return;
	}
	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PLEASE_BEFORE_CLOSE_WINDOW_AND_USE_THIS_FUNCTION"));
		return;
	}

	LoadConfig();
	if (!strcmp(arg1, "info"))
	{
		g_sentBonus.erase(ch->GetPlayerID());
		RefreshBonus(ch, crystal);
	}
	else if (!strcmp(arg1, "toggle"))
		Toggle(ch, crystal);
	else if (!strcmp(arg1, "use") && *arg3)
	{
		WORD itemCell = 0;
		str_to_number(itemCell, arg3);
		LPITEM item = itemCell < INVENTORY_MAX_NUM ? ch->GetInventoryItem(itemCell) : NULL;
		const int result = (item && IsEnergyItem(item)) ? ApplyToCrystal(ch, crystal, item) : -1;
		sys_log(0, "ENERGY_CRYSTAL: %s use crystal %u (cell %u) item %u (cell %u) -> %d", ch->GetName(), crystal->GetVnum(), cell, item ? item->GetVnum() : 0, itemCell, result);
		if (result < 0)
			Notify(ch, "ENERGY_CRYSTAL_INVALID_TARGET");
	}
}

void EnergyCrystal::OnCreate(LPITEM item)
{
	if (item && IsCrystal(item->GetVnum()) && item->GetSocket(SOCKET_ENERGY) == 0 && item->GetSocket(SOCKET_RATE) == 0)
		item->SetSocket(SOCKET_ENERGY, EnergyMaxOf(item), false);
}

void EnergyCrystal::OnLogin(LPCHARACTER ch)
{
	if (!ch || !ch->IsPC())
		return;
	g_sentBonus.erase(ch->GetPlayerID());
	LPITEM crystal = FindCrystal(ch);
	LoadConfig();
	RefreshBonus(ch, crystal);
	if (crystal && crystal->GetSocket(SOCKET_ACTIVE) && crystal->GetSocket(SOCKET_ENERGY) > 0)
		StartTick(ch);
}
#endif
