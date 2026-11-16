# EnergyBar.LoadWindow icinde ara:
			self.energyGaugeToolTip = self.GetChild("EnergyGauge_ToolTip")
			self.energyButton = self.GetChild("EnergyButton")

# Altina ekle:
			if not app.ENABLE_ENERGY_CRYSTAL:
				self.energyButton.Hide()


# EnergyBar.RefreshStatus icinde ara:
		def RefreshStatus(self):

# Altina ekle:
			if app.ENABLE_ENERGY_CRYSTAL:
				self._RefreshCrystalGauge()
				return


# EnergyBar._RefreshCrystalGauge icinde ara:
			if not self.energyEmpty:
				return

# Altina ekle:
			import uiEnergyCrystal
			cell = uiEnergyCrystal.FindCrystalCell()


# EnergyBar._RefreshCrystalGauge icinde ara:
				rate = player.GetItemMetinSocket(cell, 1)
				active = bool(player.GetItemMetinSocket(cell, 2)) and energy > 0

# Altina ekle:
				lines = uiEnergyCrystal.GetCrystalSummaryLines(cell, rate, cap, energy, energyMax)


# EnergyBar._RefreshCrystalGauge icinde ara:
			else:
				self.energyFull.Show()

# Altina ekle:
			self.crystalLines = lines or [(localeInfo.ENERGY_CRYSTAL_UNLOCK, 0xffd7b25b)]


# EnergyBar.OnUpdate icinde ara:
		def OnUpdate(self):

# Altina ekle:
			if app.ENABLE_ENERGY_CRYSTAL and app.GetTime() >= self.nextCrystalRefresh:
				self.nextCrystalRefresh = app.GetTime() + 1.0
				self._RefreshCrystalGauge()


# EnergyBar.OnUpdate icinde ara:
			if not self.energyGaugeToolTip or not self.tooltipEnergy:
				return

# Altina ekle:
			if app.ENABLE_ENERGY_CRYSTAL and self.energyGaugeToolTip.IsIn():
				if not self.tooltipEnergy.IsShow() or self.shownEnergyText != self.energyText:
					self.tooltipEnergy.ClearToolTip()
					for (text, color) in self.crystalLines:
						self.tooltipEnergy.AutoAppendTextLine(text, color)
					self.tooltipEnergy.AlignHorizonalCenter()
					self.tooltipEnergy.ShowToolTip()
					self.shownEnergyText = self.energyText
				return

