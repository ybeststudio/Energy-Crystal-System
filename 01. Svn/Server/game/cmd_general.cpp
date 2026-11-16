// Ara:
{
	DamageTrace::Request(ch, argument);
}

// Altina ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
#include "energy_crystal.h"
ACMD(do_energy_crystal)
{
	EnergyCrystal::OnCommand(ch, argument);
}
#endif

