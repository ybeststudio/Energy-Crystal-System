# InventoryWindow.__DropSrcItemToDestItemInInventory icinde ara:
			if player.GetChangeLookWindowOpen() == 1:
				return

# Altina ekle:
		if app.ENABLE_ENERGY_CRYSTAL and srcItemVID in self.ENERGY_CRYSTAL_SOURCE_ITEMS and self.__IsEnergyCrystal(dstItemSlotPos):
			if srcItemVID in (51050, 51051) and app.ENABLE_AUTO_REFINE:
				constInfo.AUTO_REFINE_TYPE = 1
				constInfo.AUTO_REFINE_DATA["ITEM"] = [srcItemSlotPos, dstItemSlotPos]
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
			self.wndItem.SetUseMode(FALSE)
			return


# InventoryWindow icinde ara:
						return
		self.ShowToolTip(overSlotPos)

# Altina ekle:
	ENERGY_CRYSTAL_SOURCE_ITEMS = (51031, 51032, 51033, 51034, 51035, 51036, 51037, 51038, 51039, 51050, 51051, 51053, 51054, 51055, 51058)

	def __IsEnergyCrystal(self, slotPos):
		vnum = player.GetItemIndex(slotPos)
		return 51040 <= vnum <= 51049


# InventoryWindow.__IsUsableItemToItem icinde ara:
	def __IsUsableItemToItem(self, srcItemVNum, srcSlotPos):

# Altina ekle:
		if app.ENABLE_ENERGY_CRYSTAL and srcItemVNum in self.ENERGY_CRYSTAL_SOURCE_ITEMS:
			return TRUE


# InventoryWindow.__CanUseSrcItemToDstItem icinde, ilgili if blogunun sonuna ekle:
		elif app.ENABLE_ENERGY_CRYSTAL and srcItemVNum in self.ENERGY_CRYSTAL_SOURCE_ITEMS:
			return self.__IsEnergyCrystal(dstSlotPos)


# InventoryWindow.__UseItem icinde ara:
		ItemVNum = player.GetItemIndex(slotIndex)
		item.SelectItem(ItemVNum)

# Altina ekle:
		if app.ENABLE_ENERGY_CRYSTAL and 51040 <= ItemVNum <= 51049 and self.interface:
			self.interface.OpenEnergyCrystalWindow()
			return

