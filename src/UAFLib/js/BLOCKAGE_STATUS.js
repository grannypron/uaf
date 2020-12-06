/** TODO **/
function BLOCKAGE_STATUS() {
    this.data = new CList();
}

/**TODO
void Serialize(CArchive & ar);
POSITION FindData(int level, int x, int y);
BOOL IsSecret(int level, int x, int y, int facing);
void ClearSecret(int level, int x, int y, int facing);
BOOL IsLocked(int level, int x, int y, int facing);
void ClearLocked(int level, int x, int y, int facing);
BOOL IsSpelled(int level, int x, int y, int facing);
void ClearSpelled(int level, int x, int y, int facing);

BOOL IsLockedSecret(int level, int x, int y, int facing);
BOOL IsSpelledSecret(int level, int x, int y, int facing);
**/

BLOCKAGE_STATUS.prototype.Clear = function () {
    this.data.RemoveAll();
}

BLOCKAGE_STATUS.prototype.SerializeCAR = function (ar) {
    var pos;
    if (ar.IsStoring()) {
        ar.writeInt(this.data.GetCount());
        pos = this.data.GetHeadPosition();
        while (pos != null) {
            this.data.GetAt(pos).SerializeCAR(ar);
            this.data.GetNext(pos);
            pos = this.data.NextPos();
        }
    }
    else {
        var temp = new BlockageDataType();
        var count;
        count = ar.readInt();

        for (var i = 0; i < count; i++)
        {
            temp.SerializeCAR(ar);
            this.data.AddTail(temp);
        }
    }
}


function BlockageDataType() {
    this.level = 0;
    this.x = 0;
    this.y = 0;
    this.Stats = new BlockageStats();
}

BlockageDataType.prototype.SerializeCAR = function (ar) {
    if (ar.IsStoring()) {
        ar.writeInt(this.level);
        ar.writeInt(this.x);
        ar.writeInt(this.y);
        ar.writeWORD(this.Stats.StatsFull);
    }
    else {
        this.level = ar.readInt();
        this.x = ar.readInt();
        this.y = ar.readInt();
        this.Stats.StatsFull = ar.readDWORD();  //**TODO: Something in this setter should probably set the individual flags and directionStats values **/
    }
}


function BlockageStats() {
    this.StatsFull = UAFUtil.ByteFromHexString("0xFFFF"); // WORD // access to all 16 flags at once

    this.directionStats = {};
    this.directionStats.NorthFlags = 4;
    this.directionStats.SouthFlags = 4;
    this.directionStats.EastFlags = 4;
    this.directionStats.WestFlags = 4;
    
    // each bit starts as 1, then gets set to 0 when party
    // clears the blockage. 
    this.flags = {};
    this.flags.NorthLocked = 1;
    this.flags.NorthSecret = 1;
    this.flags.NorthSpelled = 1;
    this.flags.NorthUnused = 1;
    this.flags.SouthLocked = 1;
    this.flags.SouthSecret = 1;
    this.flags.SouthSpelled = 1;
    this.flags.SouthUnused = 1;
    this.flags.EastLocked = 1;
    this.flags.EastSecret = 1;
    this.flags.EastSpelled = 1;
    this.flags.EastUnused = 1;
    this.flags.WestLocked = 1;
    this.flags.WestSecret = 1;
    this.flags.WestSpelled = 1;
    this.flags.WestUnused = 1;
    
}