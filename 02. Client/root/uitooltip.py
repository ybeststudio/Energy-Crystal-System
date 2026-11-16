# ItemToolTip.AddItemData icinde, ilgili if blogunun sonuna ekle:
		elif app.ENABLE_ENERGY_CRYSTAL and item.ITEM_TYPE_BLEND == itemType and 51040 <= itemVnum <= 51049:
			self.__AppendEnergyCrystalInformation(metinSlot)

		elif app.ENABLE_ENERGY_CRYSTAL and item.ITEM_TYPE_BLEND == itemType and item.GetItemSubType() >= 2:
			self.__AppendLimitInformation()


# ItemToolTip icinde ara:
		(item.ITEM_ANTIFLAG_DROP, "emoji/anti_drop"),
	)

# Altina ekle:
	ENERGY_CRYSTAL_BONUS_LABELS = {
		336 : "ENERGY_CRYSTAL_BONUS_AVERAGE_PVM",	# APPLY_ATTBONUS_MEDI_PVM
		342 : "ENERGY_CRYSTAL_BONUS_DUNGEON",		# APPLY_ATTBONUS_DUNGEON
		340 : "ENERGY_CRYSTAL_BONUS_EREBUS",		# APPLY_ATTBONUS_EREBUS
		341 : "ENERGY_CRYSTAL_BONUS_MELEY",			# APPLY_ATTBONUS_MELEY
		332 : "ENERGY_CRYSTAL_BONUS_ELEMENT",		# APPLY_ENCHANT_ELEMENTS
	}
	ENERGY_CRYSTAL_TIME_COLOR = 0xffe7c77d
	ENERGY_CRYSTAL_BONUS_ORDER = (340, 342, 341, 336, 332)	# Erebus, Dungeon, Meley, PvM, Element (uienergycrystal BONUS_ROWS)

	def __AppendEnergyCrystalInformation(self, metinSlot):
		energyMax = item.GetValue(1) or 86400
		cap = item.GetValue(0) / 100.0
		energy = energyMax
		rate = 0
		if metinSlot and not self.isShopItem:
			energy = max(0, int(metinSlot[0]))
			rate = int(metinSlot[1])

		self.AppendSpace(5)
		self.AppendTextLine(localeInfo.ENERGY_CRYSTAL_RATE.format(rate / 10.0, cap), self.POSITIVE_COLOR)

		if metinSlot and not self.isShopItem:
			bonus = dict((a, (g, u)) for (a, g, u) in constInfo.ENERGY_CRYSTAL_BONUS)
			if bonus:
				self.AppendSpace(5)
			for applyType in self.ENERGY_CRYSTAL_BONUS_ORDER:
				if applyType not in bonus:
					continue
				(gain, unlocked) = bonus[applyType]
				label = localeInfo.GetText(self.ENERGY_CRYSTAL_BONUS_LABELS.get(applyType, ""), "")
				if label:
					self.AppendTextLine(localeInfo.ENERGY_CRYSTAL_BONUS_GAIN.format(label, gain), 0xff9fd6a8 if unlocked else 0xffe57875)

		self.AppendSpace(5)
		self.AppendTextLine(localeInfo.ENERGY_CRYSTAL_TIME_LEFT.format("%02d:%02d:%02d" % (energy / 3600, energy % 3600 / 60, energy % 60)), self.ENERGY_CRYSTAL_TIME_COLOR)
		self.__AppendEnergyGauge(float(energy) / energyMax if energyMax else 0.0)

