/*********************************************************************
 * File Name : energy_crystal.h
 * Author    : BestStudio
 * Version   : 1.0.0
 * Date      : 15-11-2026
 */

#pragma once

#include "../../common/service.h"

#ifdef ENABLE_ENERGY_CRYSTAL
//	socket0 = remaining energy (seconds, consumed only in combat), socket1 = amplification (0.1 % units),
//	socket2 = active flag; attributes 0..4 = unlocked bonuses (our apply type, value 1).
namespace EnergyCrystal
{
	int		OnUseItem(LPCHARACTER ch, LPITEM item, TItemPos DestCell);
	// /energy_crystal toggle <cell> | use <crystal cell> <item cell>
	void	OnCommand(LPCHARACTER ch, const char* argument);
	void	OnLogin(LPCHARACTER ch);
	void	OnCreate(LPITEM item);
	bool	IsCrystal(DWORD vnum);
	bool	IsCrystalScroll(DWORD vnum);
	void	CopyCrystalState(LPITEM from, LPITEM to);
	void	OnCrystalChanged(LPCHARACTER ch);
}
#endif
