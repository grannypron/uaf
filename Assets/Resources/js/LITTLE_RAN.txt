function LITTLE_RAN() {
    this.w = 0;  //DWORD
    this.z = 0;  //DWORD
}

LITTLE_RAN.prototype.Random = function() {
    w = 69069 * w + 1;
    return w + ((w >> 16) & 65535);  /* 32-bit result */
}

LITTLE_RAN.prototype.Roll = function(sides, num, bonus) {
    if (sides > 0) {
        for (; num > 0; num--) {
            bonus += this.Random() % sides + 1;
        };
    };
    return bonus;
}

LITTLE_RAN.prototype.Init = function(seed) {
    w = seed & UAFUtil.ByteFromHexString("0xffff");
    z = seed >> 16;
    if (w == 0) w = 0xc451;
    if (z == 0) z = 0x541b;
}