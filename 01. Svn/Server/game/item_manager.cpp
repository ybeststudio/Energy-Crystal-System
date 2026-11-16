// Dosyanin uygun bir yerine ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
#include "energy_crystal.h"
#endif


// LPITEM ITEM_MANAGER::CreateItem(DWORD vnum, DWORD count, DWORD id, bool bTryMagic, int iRarePct, bool bSkipSave) icinde:
// Ara:
	{
		if (ITEM_BLEND == item->GetType())
		{

// Altina ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
			EnergyCrystal::OnCreate(item);
#endif

