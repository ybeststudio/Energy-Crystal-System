// Dosyanin uygun bir yerine ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
#include "energy_crystal.h"
#endif


// void CInputLogin::Entergame(LPDESC d, const char * data) icinde:
// Ara:
	Mokoko_OnLogin(ch);

// Altina ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
	EnergyCrystal::OnLogin(ch);
#endif

