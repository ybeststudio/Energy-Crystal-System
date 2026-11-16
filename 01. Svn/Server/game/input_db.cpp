// Dosyanin uygun bir yerine ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
#include "energy_crystal.h"
#endif


// void CInputDB::ItemLoad(LPDESC d, const char * c_pData) icinde:
// Ara:
	ch->CheckMaximumPoints();
	ch->PointsPacket();
	ch->SetItemLoaded();

// Altina ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
	EnergyCrystal::OnLogin(ch);
#endif

