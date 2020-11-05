/** TODO **/
function ASLENTRY() {
    this.m_flags = 0;
    this.m_value = "";
    this.m_key = "";
    this.Clear();
}
function ASLENTRYKeyValueFlags(key, value, flags) {
    this.m_key = key;
    this.m_value = value;
    this.m_flags = flags;
}

ASLENTRY.prototype.Clear = function () {
    this.m_flags = 0;
    this.m_value = "";
    this.m_key = "";
}

ASLENTRY.prototype.DeSerialize = function (car) {
    var i, n;
    this.m_key = car.readString();
    n = this.m_key.length;
    var x20 = UAFUtil.ByteFromHexString("0x20");
    for (i = 0; i < n; i++) {
        if (this.m_key.charCodeAt(i) < x20) {
            this.m_key = UAFUtil.setCharAt(this.m_key, i, String.fromCharCode(this.m_key.charCodeAt(i) + x20));
        };
    };

    this.m_flags = car.readByte();
    this.m_value = car.readString();
}

ASLENTRY.prototype.Equals = function (atv) {
    if (atv == null) { return null; }
    return atv.m_flags == this.m_flags && atv.m_value == this.m_value;
}


ASLENTRY.prototype.Flags = function () {
    return this.m_flags;
}

ASLENTRY.prototype.Key = function () {
    return this.m_key;
}

/**
 *  TODO
ASLENTRY.prototype.Serialize = function (ar) { }
ASLENTRY.prototype.SerializeCAR = function (ar) { }
ASLENTRY.prototype.DeSerializeCAR = function (ar) { }
 */

