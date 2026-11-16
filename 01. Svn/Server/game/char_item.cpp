// Dosyanin uygun bir yerine ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
#include "energy_crystal.h"
#endif


// static bool RefineScrollRenewal_CanUse(LPCHARACTER ch, LPITEM scroll, LPITEM item) icinde:
// Ara:
};
static bool RefineScrollRenewal_CanUse(LPCHARACTER ch, LPITEM scroll, LPITEM item)
{

// Altina ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
	if (EnergyCrystal::IsCrystalScroll(scroll->GetVnum()))
		return true;	// energy crystal scrolls: own rules (energy_crystal.cpp)
#endif


// static bool RefineScrollRenewal_Rule(LPITEM scroll, LPITEM item, int baseProb, BYTE& type, int& prob, bool& keep) icinde:
// Ara:
}
static bool RefineScrollRenewal_Rule(LPITEM scroll, LPITEM item, int baseProb, BYTE& type, int& prob, bool& keep)
{

// Altina ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
	if (EnergyCrystal::IsCrystalScroll(scroll->GetVnum()))
		return false;
#endif


// bool CHARACTER::DoRefineWithScroll(LPITEM item, ...) icinde:
// Ara:
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);

// Altina ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
			EnergyCrystal::CopyCrystalState(item, pkNewItem);
#endif


// bool CHARACTER::DoRefineWithScroll(LPITEM item, ...) icinde:
// Ara:
		NotifyRefineFail(this, item, szRefineType);
		PayRefineFee(prt->cost);
	}

// Altina ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
	if (EnergyCrystal::IsCrystal(result_vnum))
		EnergyCrystal::OnCrystalChanged(this);
#endif


// bool CHARACTER::RefineItem(LPITEM pkItem, LPITEM pkTarget) icinde:
// Ara:
		return false;
	if (pkItem->GetSubType() == USE_TUNING)
	{

// Altina ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
		if (EnergyCrystal::IsCrystal(pkTarget->GetVnum()) != EnergyCrystal::IsCrystalScroll(pkItem->GetVnum()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot upgrade this item."));
			return false;
		}
#endif


// bool CHARACTER::UseItemEx(LPITEM item, TItemPos DestCell) icinde:
// Ara:
}
bool CHARACTER::UseItemEx(LPITEM item, TItemPos DestCell)
{

// Altina ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
	{
		const int iEnergyResult = EnergyCrystal::OnUseItem(this, item, DestCell);
		if (iEnergyResult >= 0)
			return iEnergyResult != 0;
	}
#endif


// bool CHARACTER::UseItem(TItemPos Cell, TItemPos DestCell) icinde:
// Ara:
		else
			apply_type = aApplyInfo[item->GetValue(0)].bPointType;
		if (

// Altina ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
			apply_type != POINT_NONE &&
#endif

