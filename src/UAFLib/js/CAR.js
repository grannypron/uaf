function CAR(path, version) {
    CArchive.call(this, path, version);
    Object.setPrototypeOf(this, CArchive.prototype);

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


    this.Compress = function (compression) {
        this.m_compressType = 0;
        if (compression) {
            this.m_codes = new CODES();
            this.m_w = 0xffff;
            this.m_bufferIndex = 0;
            this.m_stkLen = 0;
            this.m_OC = 0xffff;
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

    this.readInt = function () {
        if (m_compressType == 0) return ar.readInt(); //20121208 PRS
        else return decompress(4);
        return this;
    }

    this.decompress = function (n) {
        throw "todo CAR::decompress";
    }

    this.readString = function () {
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
                throw (0x23);
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
            throw 0x23;
        }
        str = this.m_stringArray[index];
        return this;


        return this.MFCSerializer.readString();
    }
}


