#include "pch.h"
#include "CAR.h"



//CAR::CAR(CFile *pFile, UINT nMode) :  CArchive(pFile, nMode)
CAR::CAR(CFile* pFile, UINT nMode) : ar(pFile, nMode)
{
    m_compressType = 0; // No compression yet.
    m_nextIndex = 1;
    m_cData = NULL;
    m_dData = NULL;
    m_codes = NULL;
    m_w = 0;
    m_bufferIndex = 0;
    memset(m_buffer, 0, sizeof(m_buffer));
    m_stkLen = 0;
    m_OC = 0;
    m_numCode = 0;
    m_C = 0;
}


CAR::~CAR(void)
{
    // Make sure we flush the output buffer
    //Flush();
    if (m_cData != NULL) delete m_cData;
    if (m_dData != NULL) delete m_dData;
    if (m_codes != NULL) delete m_codes;
}

void CAR::compress(const char* chars, int n)
{
    //  static unsigned int buffer[13];
    //  static unsigned int bufferIndex=0;
    //  static unsigned _int16 w=0xffff;
    unsigned __int16 cd;
    unsigned char c;
    for (; n > 0; n--, chars++)
    {
        if (m_codes->Full())
        {
            // And output the old code from w.
            *((unsigned int*)(((char*)m_buffer) + (m_bufferIndex >> 3))) |= m_w << (m_bufferIndex & 7);
            m_bufferIndex = (m_bufferIndex + 13) % 416;
            if (m_bufferIndex == 0)
            {
                Write(m_buffer, 52);
                memset(m_buffer, 0, 52);
            };
            m_w = 0xffff;
            m_codes->Clear();
            // And output the RESET code
            *((unsigned int*)(((char*)m_buffer) + (m_bufferIndex >> 3))) |= 8190 << (m_bufferIndex & 7);
            m_bufferIndex = (m_bufferIndex + 13) % 416;
            if (m_bufferIndex == 0)
            {
                Write(m_buffer, 52);
                memset(m_buffer, 0, 52);
            };
        };
        c = *chars;
        if (m_w == 0xffff)
        { // Single characters are in code list by definition.
            m_w = c;
            continue;
        };
        if (((cd = m_codes->FindCode((m_w << 8) | c)) != 0) /*&& (c<256) What was this for????*/)
        {
            // We found the new, larger string in the table.
            m_w = cd;
            if (m_w > 8189) // 8191 reserved for EOF, 8190 for RESET
            {
                _exit(1);
            };
            continue;
        };
        // We want to add the new, larger string to the table.
        m_codes->AddCode((m_w << 8) | c);
        // And output the old string from m_w.
        *((unsigned int*)(((char*)m_buffer) + (m_bufferIndex >> 3))) |= m_w << (m_bufferIndex & 7);
        m_bufferIndex = (m_bufferIndex + 13) % 416;
        if (m_bufferIndex == 0)
        {
            Write(m_buffer, sizeof(m_buffer));
            memset(m_buffer, 0, sizeof(m_buffer));
        };
        m_w = c;
    };
}

void CAR::decompress(char* chars, int n)
{
    // static unsigned _int16 OC=0xffff;
    // static unsigned _int16 prefix[8192];
    // static unsigned char   postfix[8192];
    // static unsigned int numCode=256;
    unsigned __int16 NC;
    // static unsigned char stack[1000];
    // static unsigned char C;
    // static unsigned int stkLen=0;
    unsigned int code;
    // static unsigned int buffer[13];
    // static unsigned int bufferIndex=0;
    unsigned __int16 theN;
    for (; n > 0; n--, chars++)
    {
        if (m_stkLen != 0)
        {
            *chars = m_dData->m_stack[--m_stkLen];
            continue;
        };
        if (m_bufferIndex == 0)
        {
            if (Read(m_buffer, sizeof(m_buffer)) != sizeof(m_buffer)) return;
        };
        theN = (unsigned __int16)(((*((unsigned int*)(((char*)m_buffer) + (m_bufferIndex >> 3)))) >> (m_bufferIndex & 7)) & 0x1fff);
        m_bufferIndex = (m_bufferIndex + 13) % 416;
        while (theN == 8190) // RESET
        {
            m_OC = 0xffff;
            m_numCode = 256;
            if (m_bufferIndex == 0)
            {
                if (Read(m_buffer, 52) != 52) return;
            };
            theN = (unsigned __int16)(((*((unsigned int*)(((char*)m_buffer) + (m_bufferIndex >> 3)))) >> (m_bufferIndex & 7)) & 0x1fff);
            m_bufferIndex = (m_bufferIndex + 13) % 416;
        };
        if (m_OC == 0xffff)
        {
            m_OC = theN;
            *chars = theN;
            m_C = theN;
            continue;
        };
        NC = theN;
        if (NC > 255)
        {
            if (NC >= m_numCode)
            {
                if (NC == 8191) return;
                // Expand OC+C into stack;
                m_dData->m_stack[m_stkLen++] = m_C;
                code = m_OC;
            }
            else
            {
                // Expand NC onto stack.
                code = NC;
            };
            while (code > 255)
            {
                ASSERT(code < m_numCode);
                m_dData->m_stack[m_stkLen++] = m_dData->m_postfix[code];
                code = m_dData->m_prefix[code];
            };
            m_dData->m_stack[m_stkLen++] = (unsigned char)code;
            m_C = m_dData->m_stack[m_stkLen - 1];
        }
        else
        {
            m_C = (unsigned char)NC;
            m_dData->m_stack[m_stkLen++] = m_C;
        };
        m_dData->m_prefix[m_numCode] = m_OC;
        m_dData->m_postfix[m_numCode++] = m_C;
        m_OC = NC;
        *chars = m_dData->m_stack[--m_stkLen];
    };
}



CAR::CODES::CODES(void)
{
    Clear();
    m_numMisses = 0;
    m_numCodes = 0;
}

CAR::CODES::~CODES(void)
{
}

void CAR::CODES::Clear(void)
{
    memset(m_hashTable, 0, sizeof(m_hashTable));
    memset(m_codes, 0xff, 256 * sizeof(unsigned int));
    m_nextCode = 256;
}

unsigned __int16 CAR::CODES::AddCode(unsigned int code)
{
    unsigned int hashcode, incr;
    m_numCodes++;
    ASSERT(m_nextCode <= 8189);
    incr = hashcode = code % (sizeof(m_hashTable) / sizeof(m_hashTable[0]));
    if (incr == 0) incr = 1;
    while (m_hashTable[hashcode] != 0)
    {
        m_numMisses++;
        hashcode = (hashcode + incr) % (sizeof(m_hashTable) / sizeof(m_hashTable[0]));
    };
    m_hashTable[hashcode] = m_nextCode;
    m_codes[m_nextCode] = code;
    return m_nextCode++;
}

unsigned __int16 CAR::CODES::FindCode(unsigned int code)
{
    unsigned int hashcode, incr;
    incr = hashcode = code % (sizeof(m_hashTable) / sizeof(m_hashTable[0]));
    if (incr == 0) incr = 1;
    while (m_hashTable[hashcode] != 0)
    {
        if (m_codes[m_hashTable[hashcode]] == code) return m_hashTable[hashcode];
        hashcode = (hashcode + incr) % (sizeof(m_hashTable) / sizeof(m_hashTable[0]));
    };
    return 0;
}

void CAR::CODES::HashDump(FILE* f)
{
    int code, stklen;
    int stk[1000];
    for (int i = 256; i < m_nextCode; i++)
    {
        code = i;
        fprintf(f, "%4d ", code);
        stklen = 0;
        while (code > 255)
        {
            code = m_codes[code];
            stk[stklen++] = code & 0xff;
            code = code >> 8;
        };
        stk[stklen++] = code;
        while (stklen > 0)
        {
            fprintf(f, "%3d ", stk[--stklen]);
        };
        fprintf(f, "\n");
    };
}
