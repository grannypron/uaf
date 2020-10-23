#pragma once 
#include <afx.h>
#include <afxtempl.h>
#include "MemoryReader.h"

#define CArchiveBufSize 7
#define CArchiveBufCur 9
#define CArchiveBufMax 10
#define CArchiveBufStart 11


public class CAR //: public CArchive
{
public:
    CArchive ar;
public:
    inline BOOL IsStoring(void) { return ar.IsStoring(); };     //Added 20121208 PRS
    inline BOOL IsLoading(void) { return ar.IsLoading(); };     //Added 20121208 PRS
    inline const CFile* GetFile(void) const { return ar.GetFile(); }; //Added 20121208 PRS
    inline DWORD GetPosition(void) const
    {

        const int* par;
        par = (int*)&ar;
        return     GetFile()->GetPosition()  // End of current buffer
            - par[CArchiveBufMax]      // Start of current buffer
            + par[CArchiveBufCur]
            + m_bufferIndex / 8;
    };
    inline BOOL ReadString(CString& str)
    {
        if (m_compressType == 0)
        {
            return ar.ReadString(str);  //Added 20121208 PRS
        }
        else
        {
            //die("Not Needed?"); // Not Implemented(0xc3a17, false);
            return FALSE;
        };
    };
private:
    inline void Write(void* buf, unsigned int num) { ar.Write(buf, num); }; //Added 20121208 PRS
    inline UINT Read(void* buf, unsigned int num)
    {
        UINT result;
        result = ar.Read(buf, num);
        return result;
    }; //Added 20121208 PRS
    class CODES
    {
    private:
        unsigned __int16 m_hashTable[9973];
        unsigned int m_codes[8192];
        unsigned __int16 m_nextCode;
        int m_numCodes;
        int m_numMisses;
    public:
        CODES(void);
        ~CODES(void);
        void Clear(void);
        unsigned __int16 AddCode(unsigned int code);
        void HashDump(FILE* f);
        bool Full(void) { return m_nextCode == 8190; };
        unsigned __int16 FindCode(unsigned int code); // zero if no find
    };
    class DDATA
    {
        friend class CAR;
        char m_stack[1000];
        unsigned __int16 m_prefix[8192];
        unsigned char   m_postfix[8192];
    };
    class CDATA
    {
        friend class CAR;
    };
private:
    char m_compressType;
    unsigned int m_nextIndex;
    CMap <CString, LPCSTR, unsigned int, unsigned int> stringIndex;
    CArray <CString, CString&> m_stringArray;
    DDATA* m_dData; // Allocated at run time on heap.  We like
    CDATA* m_cData; // to keep these large items off the stack.
    CODES* m_codes;
    unsigned __int16 m_w;
    unsigned int m_bufferIndex;
    unsigned int m_buffer[13]; // 52 bytes
    unsigned int m_stkLen;
    unsigned __int16 m_OC;
    unsigned int m_numCode;
    unsigned char m_C;
public:
    CAR(CFile* pFile, UINT nMode);
    //CAR(CFileWrapper cFile, UINT nMode);
    ~CAR(void);
    void Compress(bool compress);
    void compress(const char* chars, int num);
    void decompressInt(char* chars, int num, MemoryReader* src);
    void decompressString(CString& str, MemoryReader* src);
};
