# GameWindow icinde ara:
			self.targetBoard.OnDamageTraceEnd(int(request), int(displayed), int(hpLoss))
	if app.ENABLE_MOKOKO_SYSTEM:

# Altina ekle:
		def __EnergyCrystalMsg(self, key):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GetText(key, key))
			self.__EnergyCrystalRefresh()

		def __EnergyCrystalRate(self, rate):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ENERGY_CRYSTAL_RATE_RESULT.format(int(rate) / 10.0))
			self.__EnergyCrystalRefresh()

		def __EnergyCrystalRefine(self, success, grade):
			text = localeInfo.ENERGY_CRYSTAL_REFINE_SUCCESS if int(success) else localeInfo.ENERGY_CRYSTAL_REFINE_FAIL
			chat.AppendChat(chat.CHAT_TYPE_INFO, text.format(int(grade)))
			self.__EnergyCrystalRefresh()

		def __EnergyCrystalBonus(self, data):
			bonus = []
			for part in data.split("|"):
				fields = part.split(",")
				if len(fields) == 3:
					bonus.append((int(fields[0]), int(fields[1]), int(fields[2])))
			constInfo.ENERGY_CRYSTAL_BONUS = bonus
			constInfo.ENERGY_CRYSTAL_GAINS = [(a, g) for (a, g, u) in bonus if u]
			self.__EnergyCrystalRefresh()

		def __EnergyCrystalRefresh(self):
			wnd = self.interface and self.interface.wndEnergyCrystal
			if wnd and wnd.IsShow():
				wnd.Refresh()


# GameWindow.__ServerCommand_Build icinde ara:
			serverCommandList["MokokoData"] = self.__MokokoData
			serverCommandList["MokokoFound"] = self.__MokokoFound

# Altina ekle:
			if app.ENABLE_ENERGY_CRYSTAL:
				serverCommandList["EnergyCrystalMsg"] = self.__EnergyCrystalMsg
				serverCommandList["EnergyCrystalRate"] = self.__EnergyCrystalRate
				serverCommandList["EnergyCrystalRefine"] = self.__EnergyCrystalRefine
				serverCommandList["EnergyCrystalBonus"] = self.__EnergyCrystalBonus

