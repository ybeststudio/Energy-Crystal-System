# Ara:
import uidropinfo
import uishopsoldnotice

# Altina ekle:
import uienergycrystal


# Interface.__init__ icinde ara:
		self.wndInventoryNew = None
		self.wndShop = None

# Altina ekle:
		self.wndEnergyCrystal = None


# Interface.__MakeTaskBar icinde ara:
			wndEnergyBar.LoadWindow()
			self.wndEnergyBar = wndEnergyBar

# Altina ekle:
		self.wndEnergyCrystal = None
		if app.ENABLE_ENERGY_CRYSTAL:
			self.wndEnergyCrystal = uienergycrystal.EnergyCrystalWindow()
			self.wndEnergyCrystal.SetInterface(self)
			if self.wndEnergyBar:
				self.wndEnergyBar.SetEnergyButtonEvent(ui.__mem_func__(self.ToggleEnergyCrystalWindow))


# Interface.MakeInterface icinde ara:
			self.wndDragonSoul.SetItemToolTip(self.tooltipItem)
			self.wndDragonSoulRefine.SetItemToolTip(self.tooltipItem)

# Altina ekle:
		if self.wndEnergyCrystal:
			self.wndEnergyCrystal.SetItemToolTip(self.tooltipItem)


# Interface.Close icinde ara:
			self.wndEnergyBar.Destroy()
			self.wndEnergyBar = None

# Altina ekle:
		if self.wndEnergyCrystal:
			self.wndEnergyCrystal.Destroy()
			self.wndEnergyCrystal = None


# Interface icinde ara:
	def AppendMaterialToRefineDialog(self, vnum, count):
		self.dlgRefineNew.AppendMaterial(vnum, count)

# Altina ekle:
	def ToggleEnergyCrystalWindow(self):
		if self.wndEnergyCrystal:
			self.wndEnergyCrystal.Toggle()

	def OpenEnergyCrystalWindow(self):
		if self.wndEnergyCrystal:
			self.wndEnergyCrystal.Open()

