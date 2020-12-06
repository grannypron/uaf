function UAFUtil() {
};

UAFUtil.prototype.ByteFromHexString = function (str) {
    var val = parseInt(str, 16);
    if ((str.substr(0, 3) == "0x8") > 0) {  /**TODO**: do this hack for two's complement a little better like if (var & 0x800000000 then...) */
        //val = val - 4294967296; // is 0x100000000
        for (var idx = 0; idx < 32; idx++) {
            val = val - 134217728;  //42949672960 is too big for WebGL player to handle;
        }
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
        else c = str.charAt(i);

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

UAFUtil.prototype.parseDice = function (s) {
    var reDice = /(\d+)[D|d](\d+)(\s*[\+\-]\s*\d+)*/
    var tokens = s.match(reDice);
    var nbr = tokens[1];
    var die = tokens[2];
    var bonus = 0;

    if (tokens.length > 3 && tokens[3]) {
        var bonusExp = tokens[3];
        bonusExp = bonusExp.replace(/[\s\+]/g, '');  // take out the + but save the minus just in case we want to support negatives
        bonus = parseInt(bonusExp);
    }
    return { nbr: nbr, die: die, bonus: bonus};
}


var UAFUtil = new UAFUtil();