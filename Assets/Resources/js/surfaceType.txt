function surfaceType() {
    this.BogusDib = 0;
    this.CommonDib = 1;
    this.CombatDib = 2;
    this.WallDib = 4;
    this.DoorDib = 8;
    this.BackGndDib = 16;
    this.OverlayDib = 32;
    this.IconDib = 64;
    this.OutdoorCombatDib = 128;
    this.BigPicDib = 256;
    this.MapDib = 512;
    this.SmallPicDib = 1024;
    this.SpriteDib = 2048;
    this.TitleDib = 4096;
    this.BufferDib = 8192;
    this.FontDib = 16384;
    this.MouseDib = 32768;
    this.TransBufferDib = 65536;
    this.SpecialGraphicsOpaqueDib = UAFUtil.ByteFromHexString("0x20000");
    this.SpecialGraphicsTransparentDib = UAFUtil.ByteFromHexString("0x40000");
    this.NUM_SURFACE_TYPES = 17;
    this.AllSurfTypes = UAFUtil.ByteFromHexString("0xFFFFFFFF");
}

function surfaceTypeObj() {
    surfaceType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}



