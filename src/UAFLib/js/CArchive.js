/**
 * This class acts as a wrapper around the serializer that conforms to the MFC spec that I picked up from 
 * https://www.codeproject.com/Articles/32741/Implementing-MFC-Style-Serialization-in-NET-Part-1?msg=5755237#xx5755237xx
 * 
 * Also, it adopts the naming used in the MFC CArchive class so as to keep the porting from C++ simpler
 */

function CArchive(path, version) {

    var UAFLib = importNamespace("UAFLib");
    if (path == null) {
        this.MFCSerializer = new UAFLib.MFCSerializer();
    } else {
        this.MFCSerializer = new UAFLib.MFCSerializer(path);
    }
    this.MFCSerializer.debug = false;
    this.isStoring = false;
    this.version = version;
}

CArchive.prototype.IsStoring = function () {
    return this.MFCSerializer.IsStoring();
}

CArchive.prototype.IsLoading = function () {
    return !this.MFCSerializer.IsStoring();
}

CArchive.prototype.readInt = function () {
    return this.MFCSerializer.readInt32();
}

CArchive.prototype.writeInt = function (val) {
    this.MFCSerializer.writeUInt32(val);
}

CArchive.prototype.readByte = function () {
    return this.MFCSerializer.readByte();
}

CArchive.prototype.writeByte = function (val) {
    this.MFCSerializer.writeByte(val);
}

CArchive.prototype.readChar = function () {
    return this.readByte();
}

CArchive.prototype.writeChar = function (val) {
    this.writeByte(val);
}

CArchive.prototype.writeObject = function (obj) {
    obj.Serialize(this, this.version);
}

CArchive.prototype.readString = function () {
    return this.MFCSerializer.readString();
}

CArchive.prototype.writeString = function (val) {
    this.MFCSerializer.writeString(val);
}

CArchive.prototype.readBool = function () {
    return this.readInt() == 1;
}

CArchive.prototype.writeBool = function (val) {
    this.MFCSerializer.writeBool(val);
}

CArchive.prototype.readDouble = function () {
    return this.MFCSerializer.readDouble();
}

CArchive.prototype.writeDouble = function (val) {
    this.MFCSerializer.writeDouble(val);
}

CArchive.prototype.readDWORD = function () {
    return this.readInt();
}

CArchive.prototype.readWORD = function () {
    return this.MFCSerializer.readInt16();
}

CArchive.prototype.readFloat = function () {
    return this.MFCSerializer.readFloat();
}


CArchive.prototype.Close = function () {
    return this.MFCSerializer.Close();
}
