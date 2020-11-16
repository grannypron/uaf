
#include "stdafx.h"
#include "ramfile.h"


void RAM_FILE::Initialize(const CString& str)
{
  Clear();
  m_pData = (unsigned char *)malloc(str.GetLength());
  memcpy(m_pData, (LPCSTR)(LPCTSTR)str, str.GetLength());
}

RAM_FILE& RAM_FILE::Write(unsigned char *c, int n)
{
  int i;
  if (m_size + n > m_space)
  {
    m_space = m_space*4/3 + 1000;
    m_pData = (unsigned char *)realloc(m_pData, m_space);
  };
  for (i=0; i<n; i++)
  {
    m_pData[m_size++] = c[i];
  };
  return *this;
}

RAM_FILE& RAM_FILE:: operator <<(unsigned int i)
{
  return Write((unsigned char *)&i, 4);
}

RAM_FILE& RAM_FILE:: operator <<(const CString& str)
{
  *this << (unsigned int)str.GetLength();
  return Write((unsigned char *)(LPCSTR)(LPCTSTR)str, str.GetLength());
}

RAM_FILE& RAM_FILE::Read(unsigned char *c, int n)
{
  int i;
  for(i=0; i<n; i++)
  {
    c[i] = m_pData[m_location++];
  };
  return *this;
}

RAM_FILE& RAM_FILE::operator >> (unsigned int& i)
{
  return Read((unsigned char *)&i, 4);
}

RAM_FILE& RAM_FILE::operator >> (CString& str)
{
  unsigned int len;
  *this >> len;
  str = CString((char *)m_pData+m_location, len);
  m_location += len;
  return *this;
}
