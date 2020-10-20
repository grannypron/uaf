/** TODO **/
function PIC_DATA() {
    this.filename = "";
    this.picType = 0;
    this.filename = "";
    this.timeDelay = 0;
    this.NumFrames = 0;
    this.FrameWidth = 0;
    this.FrameHeight = 0;
    this.key = 0;        //long
    this.style = 0;
    this.lastTime = 0;    //LONGLONG
    this.sx = 0;
    this.sy = 0;
    this.flags;        //DWORD
    this.MaxLoops;     //DWORD
    this.LoopCounter;  //DWORD
    this.AllowCentering = false;
    this.UseAlpha = false;
    this.AlphaValue = 0;     //WORD
}

PIC_DATA.prototype.Clear = function () {
    /**TODO**/
}

PIC_DATA.prototype.SetDefaults = function () {
    Globals.ASSERT(this.picType != surfaceType.BogusDib, "PIC_DATA.js::SetDefaults");

    if ((this.NumFrames > 0)
        && (this.FrameWidth > 0)
        && (this.FrameHeight > 0)
        && (this.picType != surfaceType.BigPicDib)
        && (this.picType != surfaceType.SmallPicDib)
        && (this.picType != surfaceType.IconDib)
        && (this.picType != surfaceType.SpriteDib)) //DLD 12/20/00
        return;

    switch (this.picType) {
        case surfaceType.SmallPicDib:
            this.sx = this.ViewportX;
            this.sy = this.ViewportY;
            this.FrameWidth = this.ViewportWidth;
            this.FrameHeight = this.ViewportHeight;
            this.NumFrames = 1;
            this.SetPre0840LoopForeverDefaults();
            break;
        case surfaceType.IconDib:
            this.sx = -1; // never used by icons
            this.sy = -1;
            this.FrameWidth = COMBAT_TILE_WIDTH;
            this.FrameHeight = COMBAT_TILE_HEIGHT;
            this.NumFrames = 2; // 1 ready, 1 attack pose
            this.SetPre0840NonLoopDefaults();
            break;
        case surfaceType.SpriteDib:
            this.sx = this.ViewportX;
            this.sy = this.ViewportY;
            this.FrameWidth = this.ViewportWidth;
            this.FrameHeight = this.ViewportHeight;
            this.NumFrames = 3; // 1 for each distance from party: UpClose, Near, Distant
            this.SetPre0840NonLoopDefaults();
            break;
        case surfaceType.BigPicDib:
            this.sx = this.BigPic_x;         //DLD 12/20/00 //sx = ViewportScreenX;
            this.sy = this.BigPic_y;         // sy = ViewportScreenY;
            this.FrameWidth = this.BigPic_w; // FrameWidth = 608;
            this.FrameHeight = this.BigPic_h;//FrameHeight = 288; //end add change 
            this.NumFrames = 1;
            this.SetPre0840NonLoopDefaults();
            break;

        default:
            Globals.ASSERT(false, "PIC_DATA.js::SetDefaults");
            break;
    }
}

PIC_DATA.prototype.PreSerialize = function (IsStoring) {
    if (IsStoring) {
        this.filename = Globals.StripFilenamePath(this.filename);
    }
}

PIC_DATA.prototype.PostSerialize = function (IsStoring, ver) {
    if (!IsStoring && (ver < 0.954)) {
        this.filename = Globals.StripFilenamePath(this.filename);
    };
    // PicArtDir migh not be correct!!!  AddFolderToPath(filename, rte.PicArtDir());
}

PIC_DATA.prototype.SerializeCAR = function (ar, version, path) {
    this.PreSerialize(ar.IsStoring());
    var i;
    if (ar.IsStoring()) {
        i = picType;
        ar.writeInt(i);

        this.filename = Globals.StripFilenamePath(this.filename);
        AS(ar, this.filename);
        ar.writeInt(this.timeDelay);
        ar.writeInt(this.NumFrames);
        ar.writeInt(this.FrameWidth);
        ar.writeInt(this.FrameHeight);
        ar.writeDWORD(this.flags);
        ar.writeDWORD(this.MaxLoops);
        ar.writeInt(this.style);
        // no need to save LoopCounter
        ar.writeBool(this.UseAlpha);
        ar.writeWORD(this.AlphaValue);
    }
    else {
        i = ar.readInt();
        this.picType = i;

        this.filename = DAS(ar);
        if (Globals.version < 0.930269) {
            this.filename = Globals.StripFilenamePath(this.filename);
        };
        if (this.filename = "") {
            this.picType = BogusDib;
        };
        this.timeDelay = ar.readInt();
        this.NumFrames = ar.readInt();
        this.FrameWidth = ar.readInt();
        this.FrameHeight = ar.readInt();
        if (version >= _VERSION_0790_)
            this.flags = ar.readDWORD();
        if (version >= _VERSION_0810_)
            this.MaxLoops = ar.readDWORD();
        if (version >= _VERSION_0900_)
            this.style = ar.readInt();

        if (version >= _VERSION_0906_) {
            this.UseAlpha = ar.readInt();
            this.AlphaValue = ar.readWORD();
        }
    }

    this.PostSerialize(ar.IsStoring(), version);
}

PIC_DATA.prototype.SetPre0840NonLoopDefaults = function () {/**TODO**/}