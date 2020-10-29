function AURA_WAVELENGTH() {
    this.AURA_WAVELENGTH_VISIBLE = 0;
    this.AURA_WAVELENGTH_XRAY = 1;
    this.AURA_WAVELENGTH_NEUTRINO = 2;
}

function AURA_WAVELENGTHObj() {
    AURA_WAVELENGTH.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}
