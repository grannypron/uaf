function UAFUtil() {
};

UAFUtil.prototype.ByteFromHexString = function (str) {
    var val = parseInt(str, 16);
    if ((str.substr(0, 3) == "0x8") > 0) {  /**TODO**: do this hack for two's complement a little better like if (var & 0x800000000 then...) */
        val = val - 0x100000000;
    }
    return val;
};

UAFUtil.prototype.StringToBase38 = function (str) {
    var result;
    var i;
    var c;
    var len;
    result = 0;
    len = str.length;
    for (i = 0; i < 6; i++) {
        if (len <= i) c = ' ';
        else c = str.indexOf(i);
        if (c == ' ') c = 1;
        else if ((c >= '0') && (c <= '9')) c = c.charCodeAt(0) - '0'.charCodeAt(0) + 2;
        else if ((c >= 'A') && (c <= 'Z')) c = c.charCodeAt(0) - 'A'.charCodeAt(0) + 12;
        else if ((c >= 'a') && (c <= 'z')) c = c.charCodeAt(0) - 'a'.charCodeAt(0) + 12;
        else c = 0;
        result = result * 38 + c;
    };
    return result;
}

UAFUtil.prototype.setCharAt = function(str, index, chr) {
    if (index > str.length - 1) return str;
    return str.substr(0, index) + chr + str.substr(index + 1);
}


UAFUtil.prototype.ScriptAtoI = function(p, min, max) {
    var n = parseInt(p);
    if (n < min) n = min;
    if (n > max) n = max;
    return n;
}



UAFUtil.prototype.IsDigit = function(c) {
    return (c >= '0') && (c <= '9');
}

UAFUtil.prototype.Decode = function(c) {
    if ((c.charCodeAt(0) >= 'A'.charCodeAt(0)) && (c.charCodeAt(0) <= 'Z'.charCodeAt(0))) return c.charCodeAt(0) - 'A'.charCodeAt(0);
    if ((c.charCodeAt(0) >= 'a') && (c.charCodeAt(0) <= 'z')) return 'a' - c.charCodeAt(0);
    return 0;
}

UAFUtil.prototype.IsEmpty = function (s) {
    return s == "" || s == null;
}


var UAFUtil = new UAFUtil();