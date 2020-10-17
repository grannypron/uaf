/** TODO **/
function ASLENTRY() {
}

ASLENTRY.prototype.DeSerialize = function (car) {
    var i, n;
    m_key = car.readString();
    n = m_key.length;
    var x20 = UAFUtil.ByteFromHexString("0x20");
    for (i = 0; i < n; i++) {
        if (m_key.charCodeAt(i) < x20) {
            m_key = UAFUtil.setCharAt(m_key, i, String.fromCharCode(m_key.charCodeAt(i) + x20));
        };
    };

    this.m_flags = car.readByte();
    this.m_value = car.readString();
}

ASLENTRY.prototype.Equals = function (atv) {
    if (atv == null) { return null; }
    return atv.m_flags == this.m_flags && atv.m_value == this.m_value;
}
/**
 *  TODO
ASLENTRY.prototype.Serialize = function (ar) { }
ASLENTRY.prototype.SerializeCAR = function (ar) { }
ASLENTRY.prototype.DeSerializeCAR = function (ar) { }
 */

