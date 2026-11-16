// void initapp() icinde:
// Ara:
	PyModule_AddIntConstant(poModule, "ENABLE_DS_GRADE_MYTH", 0);

// Altina ekle:
#ifdef ENABLE_ENERGY_CRYSTAL
	PyModule_AddIntConstant(poModule, "ENABLE_ENERGY_CRYSTAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ENERGY_CRYSTAL", 0);
#endif

