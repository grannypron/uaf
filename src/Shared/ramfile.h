class RAM_FILE
{
  unsigned char *m_pData;
  int   m_size;
  int   m_space;
  int   m_location;
public:
  RAM_FILE(void){m_pData = NULL; m_space=0; m_size=0;m_location=0;};
  void Clear(void)
  {
    if (m_pData!=NULL)
    {
      free(m_pData);
      m_pData = NULL;
    };
    m_space=0; 
    m_size=0;
    m_location=0;
  };
  ~RAM_FILE(void){if (m_pData!=NULL) free(m_pData);};
  void Initialize(const CString& str);

  CString ConvertToCString(void){return CString((char *)m_pData, m_size);};

  RAM_FILE& Write(unsigned char *c, int n);
  RAM_FILE& operator << (unsigned int i);
  RAM_FILE& operator << (const CString& str);

  RAM_FILE& Read(unsigned char *c, int n);
  RAM_FILE& operator >> (unsigned int &i);
  RAM_FILE& operator >> (CString& str);

};

