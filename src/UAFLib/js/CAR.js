function CAR(path, version) {
    //CArchive.call(this, path, version);
    //Object.setPrototypeOf(this, CArchive.prototype);

    if (path == null) {
        this.MFCSerializer = new UAFLib.MFCSerializer();
    } else {
        this.MFCSerializer = new UAFLib.MFCSerializer(path);
    }
    this.MFCSerializer.debug = false;
    this.isStoring = false;
    this.version = version;

    this.m_compressType = 0; //char
    this.stringIndex = { };
    this.m_stringArray = [];
    this.m_nextIndex = 0; //unsigned int 
    this.m_dData = []; // Allocated at run time on heap.  We like
    this.m_cData = []; // to keep these large items off the stack.
    this.m_codes = [];
    this.m_w = 0; //unsigned __int16  
    this.m_bufferIndex = 0; // unsigned int 
    this.m_buffer = [0,0,0,0,0,0,0,0,0,0,0,0,0]; //unsigned int  // 52 bytes  
    this.m_stkLen = 0; // unsigned int 
    this.m_OC = 0; // unsigned __int16 
    this.m_numCode = 0; // unsigned int 
    this.m_C; //unsigned char 


}


CAR.prototype.Compress = function (compression) {
    this.m_compressType = 0;
    if (compression) {
        this.m_codes = new CODES();
        this.m_w = UAFUtil.ByteFromHexString("0xffff");
        this.m_bufferIndex = 0;
        this.m_stkLen = 0;
        this.m_OC = UAFUtil.ByteFromHexString("0xffff");
        this.m_numCode = 256;
        if (this.IsStoring()) {
            this.writeChar(2); // Compression type in clear
            var bufferLen = this.m_buffer.length; for (i = 0; i < bufferLen; i++) { this.m_buffer[i] = 0; };  //takes the place of memset(m_buffer,0,sizeof(m_buffer));
            this.m_compressType = 2;
            this.m_cData = new CDATA(); // Compression Data
        }
        else {
            this.m_dData = new DDATA(); // Decompression Data
            this.m_compressType = this.readChar();
        };
    }
    else {
        if (this.m_cData != null) delete this.m_cData;
        if (this.m_dData != null) delete this.m_dData;
        if (this.m_codes != null) delete this.m_codes;
        this.m_cData = null;
        this.m_dData = null;
        this.m_codes = null;
    }
}

CAR.prototype.readString = function () {
    Globals.debug("In CAR::readString");

    if (this.m_compressType == 0) {
        return this.MFCSerializer.readString();
    }
    var index; // unsigned int
    index = this.MFCSerializer.readInt();
    Globals.debug("index:" + index);
    if (index == 0) { // A new string
        //    *(CArchive *)this >> str;
        var len;
        var temp = null;
        len = this.MFCSerializer.readInt();
        if (len > 1000000) {
            throw (UAFUtil.ByteFromHexString("0x23"));
        };
        if (len == 0) {
            str = "";
        }
        else {
            temp = [];
            if (temp == null) {
                MsgBoxError("Memory exhausted");
                str = "";
            }
            else {
                temp = this.decompress(temp, len);
                //temp[len] = 0;   // PORT NOTE:  null termination not necessary
                str = "" + temp;
                if (this.m_compressType > 1) {
                    if (len != temp.length) {
                        //My_free(temp);
                        return this;
                    };
                };
            };
        };
        this.m_stringArray.SetAtGrow(m_nextIndex, str);
        this.m_nextIndex++;
        //if (temp != null) My_free(temp);
        return this;
    }
    if (index >= this.m_stringArray.length) {
        throw UAFUtil.ByteFromHexString("0x23");
    }
    str = this.m_stringArray[index];
    return this;


    return this.MFCSerializer.readString();
}

CAR.prototype.IsStoring = function () {
    return this.MFCSerializer.IsStoring();
}

CAR.prototype.IsLoading = function () {
    return !this.MFCSerializer.IsStoring();
}

CAR.prototype.readInt = function () {
    if (this.m_compressType == 0) return this.MFCSerializer.readInt32(); //20121208 PRS
    else return this.decompress(4);
}

CAR.prototype.decompress = function (n) {
    return this.MFCSerializer.decompress(n);

/*    var NC = 0;  //unsigned __int16 
    var code = 0;    //unsigned int 
    var theN = 0; //unsigned __int16 
    for (; n > 0; n--, chars++) {
        if (this.m_stkLen != 0) {
        chars = this.m_dData.m_stack[--m_stkLen];
            continue;
        };
        if (this.m_bufferIndex == 0) {
            var bufferLength = 13;  // PORT NOTE: Hard coded since JavaScript has dynamically sized arrays
            if (this.readBytes(this.m_buffer) != bufferLength) return;
        };
        // 
        theN = (unsigned __int16) (((* ((unsigned int *) (((char *)this.m_buffer) + (this.m_bufferIndex >> 3))))>> (m_bufferIndex & 7))& 0x1fff);
        m_bufferIndex = (m_bufferIndex + 13) % 416;
        while (theN == 8190) // RESET
        {
            m_OC = 0xffff;
            m_numCode = 256;
            if (m_bufferIndex == 0) {
                if (Read(m_buffer, 52) != 52) return;
            };
            theN = (unsigned __int16) (((* ((unsigned int *) (((char *)m_buffer) +(m_bufferIndex >> 3))))>> (m_bufferIndex & 7))& 0x1fff);
            m_bufferIndex = (m_bufferIndex + 13) % 416;
        };
        if (m_OC == 0xffff) {
            m_OC = theN;
      * chars = theN;
            m_C = theN;
            continue;
        };
        NC = theN;
        if (NC > 255) {
            if (NC >= m_numCode) {
                if (NC == 8191) return;
                // Expand OC+C into stack;
                m_dData -> m_stack[m_stkLen++] = m_C;
                code = m_OC;
            }
            else {
                // Expand NC onto stack.
                code = NC;
            };
            while (code > 255) {
                ASSERT(code < m_numCode);
                m_dData -> m_stack[m_stkLen++] = m_dData -> m_postfix[code];
                code = m_dData -> m_prefix[code];
            };
            m_dData -> m_stack[m_stkLen++] = (unsigned char) code;
            m_C = m_dData -> m_stack[m_stkLen - 1];
        }
        else {
            m_C = (unsigned char) NC;
            m_dData -> m_stack[m_stkLen++]=m_C;
        };
        m_dData -> m_prefix[m_numCode] = m_OC;
        m_dData -> m_postfix[m_numCode++] = m_C;
        m_OC = NC;
    * chars = m_dData -> m_stack[--m_stkLen];
    };*/
    throw "todo CAR::decompressInt";
}

CAR.prototype.writeInt = function (val) {
    this.MFCSerializer.writeUInt32(val);
}

CAR.prototype.readByte = function () {
    return this.MFCSerializer.readByte();
}

CAR.prototype.writeByte = function (val) {
    this.MFCSerializer.writeByte(val);
}

CAR.prototype.readChar = function () {
    return this.readByte();
}

CAR.prototype.writeChar = function (val) {
    this.writeByte(val);
}

CAR.prototype.writeObject = function (obj) {
    obj.Serialize(this, this.version);
}

CAR.prototype.readString = function () {
    return this.MFCSerializer.readString();
}

CAR.prototype.writeString = function (val) {
    this.MFCSerializer.writeString(val);
}

CAR.prototype.readBool = function () {
    return this.readInt() == 1;
}

CAR.prototype.writeBool = function (val) {
    this.MFCSerializer.writeBool(val);
}

CAR.prototype.readDouble = function () {
    return this.MFCSerializer.readDouble();
}

CAR.prototype.writeDouble = function (val) {
    this.MFCSerializer.writeDouble(val);
}

CAR.prototype.readDWORD = function () {
    return this.readInt();
}

CAR.prototype.readWORD = function () {
    return this.MFCSerializer.readInt16();
}

CAR.prototype.readFloat = function () {
    return this.MFCSerializer.readFloat();
}


CAR.prototype.Close = function () {
    return this.MFCSerializer.Close();
}
