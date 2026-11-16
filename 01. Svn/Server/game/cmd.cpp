// Ara:
ACMD(do_damage_trace);

// Altina ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
ACMD(do_energy_crystal);
#endif


// Ara:
	{ "damage_trace", do_damage_trace, 0, POS_DEAD, GM_PLAYER },

// Altina ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
	{ "energy_crystal", do_energy_crystal, 0, POS_DEAD, GM_PLAYER },
#endif

