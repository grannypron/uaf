var surfaceType = {};
surfaceType.BogusDib = 0;
surfaceType.CommonDib = 1;
surfaceType.CombatDib = 2;
surfaceType.WallDib = 4;
surfaceType.DoorDib = 8;
surfaceType.BackGndDib = 16;
surfaceType.OverlayDib = 32;
surfaceType.IconDib = 64;
surfaceType.OutdoorCombatDib = 128;
surfaceType.BigPicDib = 256;
surfaceType.MapDib = 512;
surfaceType.SmallPicDib = 1024;
surfaceType.SpriteDib = 2048;
surfaceType.TitleDib = 4096;
surfaceType.BufferDib = 8192;
surfaceType.FontDib = 16384;
surfaceType.MouseDib = 32768;
surfaceType.TransBufferDib = 65536;
surfaceType.SpecialGraphicsOpaqueDib = UAFUtil.ByteFromHexString("0x20000");
surfaceType.SpecialGraphicsTransparentDib = UAFUtil.ByteFromHexString("0x40000");
surfaceType.NUM_SURFACE_TYPES = 17;
surfaceType.AllSurfTypes = UAFUtil.ByteFromHexString("0xFFFFFFFF");

function surfaceTypeObj() {
    entityType.call(this, surfaceType);
    Object.setPrototypeOf(this, entityType.prototype);
}



