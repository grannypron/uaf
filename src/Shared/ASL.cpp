/******************************************************************************
* Filename: ASL.cpp (formerly attrib.cpp)
* Copyright (c) 2000, UAF Development Team (email CocoaSpud@hotmail.com)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/

#include "..\Shared\stdafx.h"
//#include "..\Shared\Version.h"
#include "Externs.h"
#include "ASL.h"
#include "class.h"
#include "specAb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef UAFEngine
// THe following line was inserted to get rid of an undefined when attempting
// to create a 'DLL' version of UAFwin.  I don't know what it does, but it seems to work.
// PRS 20150708
//extern "C" { FILE _iob[3] = { __iob_func()[0], __iob_func()[1], __iob_func()[2] }; }
#endif

#pragma warning (disable:4201) // Nameless structure

void die(const char *msg);

CString emptyString = "";





void StoreIntAsASL(A_ASLENTRY_L &asl, int data, const char *prefix, unsigned char flags)
{
  CString val;
  CString key;
  key.Format("%s", prefix);
  val.Format("%i", data);
  asl.Insert(key, val, flags);
}

void RetrieveIntFromASL(A_ASLENTRY_L &asl, int &data, const char *prefix)
{
  CString key;
  CString val;
  key.Format("%s", prefix);
  val = asl.Lookup(key);
  data = atoi(val);
}

void StoreStringAsASL(A_ASLENTRY_L &asl, CString &data, const char *prefix, unsigned char flags)
{
  CString val;
  CString key;
  key.Format("%s", prefix);
  val = data;
  asl.Insert(key, val, flags);
}

void RetrieveStringFromASL(A_ASLENTRY_L &asl, CString &data, const char *prefix)
{
  CString key;
  CString val;
  key.Format("%s", prefix);
  val = asl.Lookup(key);
  data = val;
}

DELIMITED_STRING::DELIMITED_STRING(void)
{
  m_string = "";
}

DELIMITED_STRING::~DELIMITED_STRING(void)
{
  //That was easy.
}

void DELIMITED_STRING::operator = (const CString& str)
{
  m_string = str;
}


bool DELIMITED_STRING::IsLegal(void) const
{
  const char *str, *end;
  int len, sublen;
  len=m_string.GetLength();
  if (len==0) return true;
  str = LPCTSTR(m_string);
  end = str + len; //Last character + 1 
  while (str < end)
  {
    sublen = 0;
    while (*str != '.')
    {
      if ( (*str<'0') || (*str>'9')) return false;
      sublen = 10*sublen + (*str - '0');
      str++;
      if (str >= end) return false;
    };
    str += sublen+1; //Bypass the period, too.
  };
  if (str > end) return false;
  return true;
}

int DELIMITED_STRING::Count(void) const
{
  const char *str, *end;
  int len, sublen, count;
  len=m_string.GetLength();
  if (len==0) return 0;
  str = LPCTSTR(m_string);
  end = str + len; //Last character + 1 
  count = 0;
  while (str < end)
  {
    sublen = 0;
    while (*str != '.')
    {
      if ( (*str<'0') || (*str>'9')) return count;
      sublen = 10*sublen + (*str - '0');
      str++;
      if (str >= end) return count;
    };
    str += sublen+1; //Bypass the period, too.
    count++;
  };
  if (str > end) return count-1;
  return count;
}

bool DELIMITED_STRING::Next(int *col, CString *result) const
{
  int len, start;
  const char *str;
  char delim;
  str = LPCTSTR(m_string);
  len = m_string.GetLength();
  if (*col < 0) *col = 0;
  result->Empty();
  if (*col >= len)
  {
    return false;
  };
  delim = str[*col];
  (*col)++;
  start = *col;
  while ((*col < len) && (str[*col] != delim))
  {
    (*col)++;
  };
  *result = CString(str+start, *col-start);
  return true;
}


CString DELIMITED_STRING::GetAt(int index) const
{
  const char *str, *end;
  int len, sublen;
  len=m_string.GetLength();
  if (len == 0) return CString("");
  str = LPCTSTR(m_string);
  end = str + len; //Last character + 1 
  while (str < end)
  {
    sublen = 0;
    while (*str != '.')
    {
      if ( (*str<'0') || (*str>'9')) return CString("");
      sublen = 10*sublen + (*str - '0');
      str++;
      if (str >= end) return CString("");
    };
    if (index==0) return CString(str+1, sublen);
    str += sublen+1; //Bypass the period, too.
    index--;
  };
  return CString("");
}



bool DELIMITED_STRING::Contains(const CString& cstr) const
{
  const char *str, *end;
  int len, stringlen, sublen;
  len=m_string.GetLength();
  if (len==0) return false;
  stringlen = cstr.GetLength();
  str = LPCTSTR(m_string);
  end = str + len; //Last character + 1 
  while (str < end)
  {
    sublen = 0;
    while (*str != '.')
    {
      if ( (*str<'0') || (*str>'9')) return false;
      sublen = 10*sublen + (*str - '0');
      str++;
      if (str >= end) return false;
    };
    if (   (sublen == stringlen)
         &&(memcmp(LPCTSTR(cstr),str+1,sublen)==0) ) return true;
    str += sublen+1; //Bypass the period, too.
  };
  return false;
}

CString DELIMITED_STRING::Result(void)
{
  return m_string;
}

void DELIMITED_STRING::Insert(const CString& str)
{
  CString newstr;
  newstr.Format("%d.%s",str.GetLength(),str);
  m_string = m_string + newstr;
}

bool SUBSTRINGS::HeadAndTail(CString *head, CString *tail) const
{
  int len;
  const char *start, *str, *end;
  char delim;
  len = GetLength();
  if (len < 2) return false;
  start = LPCSTR(*this);
  str = start;
  end = start + len;
  delim = *str; str++;
  while (str < end)
  {
    if (*str == delim) break;
    str++;
  };
  *head = CString(start+1, str-start-1);
  *tail = CString(str, end-str);
  return true;
}


bool SUBSTRINGS::NextField(int *col, CString *result) const
{
  // col = where to start.
  //        col is updated for next call
  int len;
  char delim;
  const char *start, *begin, *str, *end;
  if (*col < 0) *col = 0;
  len = GetLength();
  if (*col >= len-1) return false;
  start = LPCSTR(*this);
  begin = start + *col;
  str = begin;
  end = start + len;
  delim = *str;  str++;
  while (str < end)
  {
    if (*str == delim) break;
    str++;
  };
  *result = CString(begin+1, str-begin-1); 
  *col = str - start;
  return true;
}

//========================================================================

#define BALANCE_RIGHT(p, e) if((p)->m_cntL > (p)->m_cntR+2+(p)->m_cntL/8)PushRight(p,e);
#define BALANCE_LEFT(p, e)  if((p)->m_cntR > (p)->m_cntL+2+(p)->m_cntR/8)PushLeft(p,e);


BTree::~BTree(void)
{
  if (m_root != NULL)
  {
    die ("Not Needed?"); //Not Implemented(0x5dadbc, FALSE);  // Should never happen
  };
}

void BTree::Swap(BTree& tree)
{
  BTreeNode *temp;
  temp = m_root;
  m_root = tree.m_root;
  tree.m_root = temp;
}

void BTree::PushLeft(BTreeNode *pTop, BTREE_ENVIRONMENT *pEnv)
{
  int temp;
  BTreeNode *C, *D;
  ASSERT(pTop->m_cntR > 1);
  C = pTop->m_right.pNode;
  D = C->m_left.pNode;
  if (D == NULL)
  {
    temp = C->m_data;
    C->m_data = pTop->m_data;
    pTop->m_data = temp;

    pTop->m_cntR = C->m_cntR;
    pTop->m_right.pNode = C->m_right.pNode;
    C->m_cntL = pTop->m_cntL;
    C->m_left.pNode = pTop->m_left.pNode;
    C->m_cntR = 0;
    C->m_right.pNode = NULL;
    if (pTop->m_cntL == 0)
    {
      pTop->m_left.data = C->m_data;
      delete C;
    }
    else
    {
      pTop->m_left.pNode = C;
    };
    pTop->m_cntL++;
    return;
  }
  else
  {
    if (C->m_cntL == 1)
    {
      temp = C->m_data;
      C->m_data = pTop->m_data;
      pTop->m_data = temp;

      pTop->m_cntR = C->m_cntR;
      pTop->m_right.pNode = C->m_right.pNode;

      C->m_cntR = 1;
      C->m_right.data = C->m_left.data;

      C->m_cntL = pTop->m_cntL;
      C->m_left.pNode = pTop->m_left.pNode;

      pTop->m_cntL += 2;
      pTop->m_left.pNode = C;

      BALANCE_RIGHT(C, pEnv);
      return;
    }
    else
    {
      temp = D->m_data;
      D->m_data = pTop->m_data;
      pTop->m_data = temp;

      C->m_cntL = D->m_cntR;
      C->m_left.pNode = D->m_right.pNode;

      D->m_cntR = D->m_cntL;
      D->m_right.pNode = D->m_left.pNode;

      D->m_cntL = pTop->m_cntL;
      D->m_left.pNode = pTop->m_left.pNode;

      pTop->m_cntL += 1 + D->m_cntR;
      pTop->m_left.pNode = D;
      pTop->m_cntR -= 1 + D->m_cntR;

      if (pTop->m_cntR == 1)
      {
        BTreeNode *rightNode;
        rightNode = pTop->m_right.pNode;
        pTop->m_right.data = rightNode->m_data;
        delete rightNode;
      };
      
      if (pTop->m_cntL == 1)
      {
        BTreeNode *leftNode;
        leftNode = pTop->m_left.pNode;
        pTop->m_left.data = leftNode->m_data;
        delete leftNode;
      };
      
      BALANCE_LEFT(C, pEnv)
      BALANCE_RIGHT(D, pEnv)
      return;
    };
  };
}

void BTree::PushRight(BTreeNode *pTop, BTREE_ENVIRONMENT *pEnv)
{
  int temp;
  BTreeNode *C, *D;
  ASSERT(pTop->m_cntL > 1);
  C = pTop->m_left.pNode;
  D = C->m_right.pNode;
  if (D == NULL)
  {
    temp = C->m_data;
    C->m_data = pTop->m_data;
    pTop->m_data = temp;

    pTop->m_cntL = C->m_cntL;
    pTop->m_left.pNode = C->m_left.pNode;
    C->m_cntR = pTop->m_cntR;
    C->m_right.pNode = pTop->m_right.pNode;
    C->m_cntL = 0;
    C->m_left.pNode = NULL;
    if (pTop->m_cntR == 0)
    {
      pTop->m_right.data = C->m_data;
      delete C;
    }
    else
    {
      pTop->m_right.pNode = C;
    };
    pTop->m_cntR++;
    return;
  }
  else
  {
    if (C->m_cntR == 1)
    {
      temp = C->m_data;
      C->m_data = pTop->m_data;
      pTop->m_data = temp;

      pTop->m_cntL = C->m_cntL;
      pTop->m_left.pNode = C->m_left.pNode;

      C->m_cntL = 1;
      C->m_left.data = C->m_right.data;

      C->m_cntR = pTop->m_cntR;
      C->m_right.pNode = pTop->m_right.pNode;

      pTop->m_cntR += 2;
      pTop->m_right.pNode = C;

      BALANCE_LEFT(C, pEnv);
      return;
    }
    else
    {
      temp = D->m_data;
      D->m_data = pTop->m_data;
      pTop->m_data = temp;

      C->m_cntR = D->m_cntL;
      C->m_right.pNode = D->m_left.pNode;

      D->m_cntL = D->m_cntR;
      D->m_left.pNode = D->m_right.pNode;

      D->m_cntR = pTop->m_cntR;
      D->m_right.pNode = pTop->m_right.pNode;

      pTop->m_cntR += 1 + D->m_cntL;
      pTop->m_right.pNode = D;
      pTop->m_cntL -= 1 + D->m_cntL;

      if (pTop->m_cntL == 1)
      {
        BTreeNode *leftNode;
        leftNode = pTop->m_left.pNode;
        pTop->m_left.data = leftNode->m_data;
        delete leftNode;
      };

      if (pTop->m_cntR == 1)
      {
        BTreeNode *rightNode;
        rightNode = pTop->m_right.pNode;
        pTop->m_right.data = rightNode->m_data;
        delete rightNode;
      };

      BALANCE_RIGHT(C, pEnv)
      BALANCE_LEFT(D, pEnv)
      return;
    };
  };
}

struct BTREE_ENVIRONMENT
{
  BOOL(*bTreeKeyCompare)(int a);          // Compare bTreeKey to entry a.
  BOOL(*bTreeEntryCompare)(int a, int b); // Compare entry a to entry b.
  void(*bTreeDeleteData)(int a);
  int     bTreeNoData;
  LPCTSTR bTreeKey;
  int     bTreeData;
};


int BTree::AddNode(BTreeNode *pTop, BTREE_ENVIRONMENT *pEnv)
{ // *ppTop != NULL
  // Update *ppTop if tree is adjusted.
  // If (matching key already exists)
  // {
  //     return data from existing node;
  // }
  // else
  // {
  //     return NULL;
  // };
  int diff;
  diff = (*(pEnv->bTreeEntryCompare))(pTop->m_data, pEnv->bTreeData);
  //diff = pTop->m_data->Compare(bTreeData);
  if (diff == 0)
  {
    return pTop->m_data;
  };
  if (diff < 0)
  {
    // Add to right side
    if (pTop->m_cntR == 0)
    {
      pTop->m_right.data = pEnv->bTreeData;
      pTop->m_cntR = 1;
    }
    else
    {
      int foundData;
      if (pTop->m_cntR == 1)
      {
        diff = (*pEnv->bTreeEntryCompare)(pTop->m_right.data, pEnv->bTreeData);
        //diff = pTop->m_right.pData->Compare(bTreeData);
        if (diff == 0)
        {
          return pTop->m_right.data;
        };
        BTreeNode *pNewNode;
        pNewNode = new BTreeNode;
        pNewNode->m_data = pTop->m_right.data;
        pTop->m_right.pNode = pNewNode;
        if (diff < 0)
        {
          // Put on right side
          pNewNode->m_cntR = 1;
          pNewNode->m_cntL = 0;
          pNewNode->m_left.data = pEnv->bTreeNoData;
          pNewNode->m_right.data = pEnv->bTreeData;
        }
        else
        {
          // Put on left side
          pNewNode->m_cntR = 0;
          pNewNode->m_cntL = 1;
          pNewNode->m_left.data = pEnv->bTreeData;
          pNewNode->m_right.data = pEnv->bTreeNoData;
        };
        pTop->m_cntR++;
        return pEnv->bTreeNoData;
      };
      foundData = AddNode(pTop->m_right.pNode, pEnv);
      if (foundData != pEnv->bTreeNoData) return foundData;
      pTop->m_cntR++;
      BALANCE_LEFT(pTop, pEnv);
    };
  }
  else
  {
    // Add to left side
    if (pTop->m_cntL == 0)
    {
      pTop->m_left.data = pEnv->bTreeData;
      pTop->m_cntL = 1;
    }
    else
    {
      int foundData;
      if (pTop->m_cntL == 1)
      {
        diff = (*pEnv->bTreeEntryCompare)(pTop->m_left.data, pEnv->bTreeData);
        //diff = pTop->m_left.pData->Compare(bTreeData);
        if (diff == 0)
        {
          return pTop->m_left.data;
        };
        BTreeNode *pNewNode;
        pNewNode = new BTreeNode;
        pNewNode->m_data = pTop->m_left.data;
        pTop->m_left.pNode = pNewNode;
        if (diff < 0)
        {
          // Put on right side
          pNewNode->m_cntR = 1;
          pNewNode->m_cntL = 0;
          pNewNode->m_left.data = pEnv->bTreeNoData;
          pNewNode->m_right.data = pEnv->bTreeData;
        }
        else
        {
          // Put on left side
          pNewNode->m_cntR = 0;
          pNewNode->m_cntL = 1;
          pNewNode->m_left.data = pEnv->bTreeData;
          pNewNode->m_right.data = pEnv->bTreeNoData;
        };
        pTop->m_cntL++;
        return pEnv->bTreeNoData;
      };
      foundData = AddNode(pTop->m_left.pNode, pEnv);
      if (foundData != pEnv->bTreeNoData) return foundData;
      pTop->m_cntL++;
      BALANCE_RIGHT(pTop, pEnv);
    };
  };
  return pEnv->bTreeNoData;
}

//*****************************************************************
//   Remove pTop->m_data.  Replace m_data from left or right side.
//   Return old value of m_data.
//*****************************************************************
int BTree::RemoveData(BTreeNode *pTop, BTREE_ENVIRONMENT *pEnv)
{
  int result;
//  ASSERT((pTop->m_cntL | pTop->m_cntR) != 0);
  result = pTop->m_data;
  if (pTop->m_cntL > pTop->m_cntR)
  {
    if (pTop->m_cntL == 1)
    {
      pTop->m_cntL = 0;
      pTop->m_data = pTop->m_left.data;
      pTop->m_left.data = pEnv->bTreeNoData;
      return result;
    };
    pTop->m_data = RemoveLargest(pTop->m_left.pNode, pEnv);
    pTop->m_cntL--;
    if (pTop->m_cntL == 1)
    {
      BTreeNode *leftNode;
      leftNode = pTop->m_left.pNode;
      pTop->m_left.data = leftNode->m_data;
      delete leftNode;
    };
    BALANCE_RIGHT(pTop, pEnv);
    return result;
  };
  //ASSERT(pTop->m_cntR != 0);
  if (pTop->m_cntR == 0)
  {
    pTop->m_data = pEnv->bTreeNoData;
    return result;
  };
  {
    if (pTop->m_cntR == 1)
    {
      pTop->m_cntR = 0;
      pTop->m_data = pTop->m_right.data;
      pTop->m_right.data = pEnv->bTreeNoData;
      return result;
    };
    pTop->m_data = RemoveSmallest(pTop->m_right.pNode, pEnv);
    pTop->m_cntR--;
    if (pTop->m_cntR == 1)
    {
      BTreeNode *rightNode;
      rightNode = pTop->m_right.pNode;
      pTop->m_right.data = rightNode->m_data;
      delete rightNode;
    };
    BALANCE_RIGHT(pTop, pEnv);
    return result;
  };
};

int BTree::DeleteNode(BTreeNode *pTop, BTREE_ENVIRONMENT *pEnv)
{
  int diff;
  diff = (*pEnv->bTreeKeyCompare)(pTop->m_data);
  //diff = pTop->m_data->Compare(bTreeKey);
  if (diff == 0)
  {
    return RemoveData(pTop, pEnv);
  };
  if (diff < 0)
  {
    // Delete from right side
    int result;
    if (pTop->m_cntR == 0) return pEnv->bTreeNoData;
    if (pTop->m_cntR == 1)
    {
      if ((*pEnv->bTreeKeyCompare)(pTop->m_right.data) != 0) return pEnv->bTreeNoData;
      //if (pTop->m_right.data->Compare(bTreeKey) != 0) return pEnv->bTreeNoData;
      pTop->m_cntR = 0;
      result = pTop->m_right.data;
      pTop->m_right.data = pEnv->bTreeNoData;
      BALANCE_RIGHT(pTop, pEnv)
      return result;
    };
    result = DeleteNode(pTop->m_right.pNode, pEnv);
    if (result != pEnv->bTreeNoData)
    {
      pTop->m_cntR--;
      if (pTop->m_cntR == 1)
      {
        BTreeNode *rightNode;
        rightNode = pTop->m_right.pNode;
        pTop->m_right.data = rightNode->m_data;
        delete rightNode;
      };
      BALANCE_RIGHT(pTop, pEnv);
    };
    return result;
  };
  {
    // Delete from left side
    int result;
    if (pTop->m_cntL == 0) return pEnv->bTreeNoData;
    if (pTop->m_cntL == 1)
    {
      if ((*pEnv->bTreeKeyCompare)(pTop->m_left.data) != 0) return pEnv->bTreeNoData;
      //if (pTop->m_left.pData->Compare(bTreeKey) != 0) return pEnv->bTreeNoData;
      pTop->m_cntL = 0;
      result = pTop->m_left.data;
      pTop->m_left.data = pEnv->bTreeNoData;
      BALANCE_LEFT(pTop, pEnv)
      return result;
    };
    result = DeleteNode(pTop->m_left.pNode, pEnv);
    if (result != pEnv->bTreeNoData)
    {
      pTop->m_cntL--;
      if (pTop->m_cntL == 1)
      {
        BTreeNode *leftNode;
        leftNode = pTop->m_left.pNode;
        pTop->m_left.data = leftNode->m_data;
        delete leftNode;
      };
      BALANCE_LEFT(pTop, pEnv);
    };
    return result;
  };
}

int BTree::Delete(BTREE_ENVIRONMENT *pEnv)
{
  int result;
  if (m_root == NULL) return pEnv->bTreeNoData;
  result = DeleteNode(m_root, pEnv);
  if (m_root->m_data == pEnv->bTreeNoData)
  {
    delete m_root;
    m_root = NULL;
  };
  return result;
}


#ifdef _DEBUG
int NodeID = 1;
#endif

int BTree::Insert(BTREE_ENVIRONMENT *pEnv)
{
  // Return matching data if matching key already exists; else zero.
  if (m_root == NULL)
  {
    m_root = new BTreeNode;
//    m_root->id = NodeID++;
    m_root->m_cntL = 0;
    m_root->m_cntR = 0;
    m_root->m_left.pNode = NULL;
    m_root->m_right.pNode = NULL;
    m_root->m_data = pEnv->bTreeData;
    return pEnv->bTreeNoData;
  }
  else
  {
    return AddNode(m_root, pEnv);
  };
}


void BTree::ClearNode(BTreeNode *pNode, BTREE_ENVIRONMENT *pEnv)
{
  if (pNode->m_cntL != 0)
  {
    if (pNode->m_cntL == 1)
    {
      (*pEnv->bTreeDeleteData)(pNode->m_left.data);
    }
    else
    {
      ClearNode(pNode->m_left.pNode, pEnv);
    };
  };
  if (pNode->m_cntR != 0)
  {
    if (pNode->m_cntR == 1)
    {
      (*pEnv->bTreeDeleteData)(pNode->m_right.data);
    }
    else
    {
      ClearNode(pNode->m_right.pNode, pEnv);
    };
  };
  (*pEnv->bTreeDeleteData)(pNode->m_data);
  delete pNode;
}


void BTree::Clear(BTREE_ENVIRONMENT *pEnv)
{
  if (m_root!=NULL) ClearNode(m_root, pEnv);
  m_root = NULL;
}
/*
void BTree::MoveEntryLeft(BTreeNode **ppTop)
{
  BTreeNode *pNewTop;
  int temp;
  // Remove smallest from right side.
  pNewTop = RemoveSmallest(&(*ppTop)->m_right);
  (*ppTop)->m_cntR--;
  // Replace top with smallest.
  temp = pNewTop->m_data;
  pNewTop->m_data = (*ppTop)->m_data;
  (*ppTop)->m_data = temp;
  // Then add the old top to ourselves
  AddNode(ppTop, pNewTop);
}

void BTree::MoveEntryRight(BTreeNode **ppTop)
{
  BTreeNode *pNewTop;
  int temp;
  // Remove smallest from right side.
  pNewTop = RemoveLargest(&(*ppTop)->m_left);
  (*ppTop)->m_cntL--;
  // Replace top with smallest.
  temp = pNewTop->m_data;
  pNewTop->m_data = (*ppTop)->m_data;
  (*ppTop)->m_data = temp;
  // Then add the old top to ourselves
  AddNode(ppTop, pNewTop);
}
*/

int BTree::RemoveSmallest(BTreeNode *pTop, BTREE_ENVIRONMENT *pEnv)
{
  int pSmall;
  if (pTop->m_cntL == 0)
  {
    // The top entry is the smallest.
    return RemoveData(pTop, pEnv);
  };
  pTop->m_cntL--;
  if (pTop->m_cntL == 0)
  {
    pSmall = pTop->m_left.data;
    pTop->m_left.data = pEnv->bTreeNoData;
  }
  else
  {
    pSmall = RemoveSmallest(pTop->m_left.pNode, pEnv);
    if (pTop->m_cntL == 1)
    {
      BTreeNode *pLeft;
      pLeft = pTop->m_left.pNode;
      pTop->m_left.data = pLeft->m_data;
      delete pLeft;
    };
  };
  BALANCE_LEFT(pTop, pEnv);
  return pSmall;
}

int BTree::RemoveLargest(BTreeNode *pTop, BTREE_ENVIRONMENT *pEnv)
{
  int pLarge;
  if (pTop->m_cntR == 0)
  {
    // The top entry is the smallest.
    return RemoveData(pTop, pEnv);
  };
  pTop->m_cntR--;
  if (pTop->m_cntR == 0)
  {
    pLarge = pTop->m_right.data;
    pTop->m_right.data = pEnv->bTreeNoData;
  }
  else
  {
    pLarge = RemoveLargest(pTop->m_right.pNode, pEnv);
    if (pTop->m_cntR == 1)
    {
      BTreeNode *pRight;
      pRight = pTop->m_right.pNode;
      pTop->m_right.data = pRight->m_data;
      delete pRight;
    };
  };
  BALANCE_RIGHT(pTop, pEnv)
  return pLarge;
}

int BTree::GetNextAssoc(POSITION& pos) const
{
  int n;
  WORD more;
  more = 0;
  BTreeNode *pNode;
  pNode = m_root;
  n = (int)pos-1;
  for (;;)
  {
    if (n == pNode->m_cntL)
    {
      pos++;
      more += pNode->m_cntR;
      if (!more) pos = NULL;
      return pNode->m_data;
    };
    if (n < pNode->m_cntL)
    {
      if (pNode->m_cntL == 1)
      {
        pos++;
        return pNode->m_left.data;
      };
      more++;
      pNode = pNode->m_left.pNode;
    }
    else
    {
      if (pNode->m_cntR == 1)
      {
        pos++;
        if (more == 0) pos = NULL;
        return pNode->m_right.data;
      };
      n -= pNode->m_cntL + 1;
      pNode = pNode->m_right.pNode;
    };
  };
}

/*
BOOL BTree::WalkNode(int id, BTreeNode *pNode) const
{
  if (pNode->m_left != NULL)
  {
    if (WalkNode(id, pNode->m_left)) return TRUE;
  };
  if (Walker(id, pNode->m_data)) return TRUE;
  if (pNode->m_right != NULL)
  {
    if (WalkNode(id, pNode->m_right)) return TRUE;
  };
  return FALSE;
}
*/
int BTree::FindNode(BTreeNode *pNode, BTREE_ENVIRONMENT *pEnv) const
{
  int diff;
  diff = (*pEnv->bTreeKeyCompare)(pNode->m_data);
  //diff = pNode->m_data->Compare(bTreeKey);
  if (diff == 0)
  {
    return pNode->m_data;
  };
  if (diff < 0)
  {
    if (pNode->m_cntR == 1)
    {
      if ((*pEnv->bTreeKeyCompare)(pNode->m_right.data) != 0)
      //if (pNode->m_right.pData->Compare(bTreeKey) != 0)
      {
        return pEnv->bTreeNoData;
      };
      return pNode->m_right.data;
    };
    if (pNode->m_cntR != 0) return FindNode(pNode->m_right.pNode, pEnv);
    return pEnv->bTreeNoData;
  }
  else
  {
    if (pNode->m_cntL == 1)
    {
      if ((pEnv->bTreeKeyCompare)(pNode->m_left.data) != 0)
      //if (pNode->m_left.pData->Compare(bTreeKey) != 0)
      {
        return pEnv->bTreeNoData;
      };
      return pNode->m_left.data;
    };
    if (pNode->m_cntL != 0) return FindNode(pNode->m_left.pNode, pEnv);
    return pEnv->bTreeNoData;
  };
  //return pEnv->bTreeNoData;
}


int BTree::Find(BTREE_ENVIRONMENT *pEnv) const
{
  if (m_root == NULL) return pEnv->bTreeNoData;
  return FindNode(m_root, pEnv);
}

/*
void BTree::Walk(int id) const
{
  if (m_root != NULL)
  {
    WalkNode(id, m_root);
  };
}
*/
/*
int CompareASLEntries(int keyData, int data)
{
  // return (int)(key - data.key)
  return ((ASLENTRY *)data)->Compare((ASLENTRY *)keyData);
}
*/



// ********************************************************
//
//  A_ASLENTRY_L
//
// ********************************************************



void ASLENTRY_DELETE(int a);

BTREE_ENVIRONMENT ASLEntryEnvironment =
{
  ASLENTRY_COMPARE_KEY,
  ASLENTRY_COMPARE_ENTRY,
  ASLENTRY_DELETE,
  0
};


void ASLENTRY_DELETE(int a)
{
  delete (ASLENTRY *)a;
}

int ASLENTRY_COMPARE_KEY(int a)  // sign(a-key)
{
  return (((ASLENTRY *)a)->m_key).Compare(ASLEntryEnvironment.bTreeKey);
}

int ASLENTRY_COMPARE_ENTRY(int a, int b)  // sign(a-b)
{
  return (((ASLENTRY *)a)->m_key).Compare(((ASLENTRY *)b)->m_key);
}


const CString& A_ASLENTRY_L::Lookup(LPCTSTR key) const
{
  ASLENTRY *foundData;

  ASLEntryEnvironment.bTreeKey = key;
  foundData = (ASLENTRY *)BTree::Find(&ASLEntryEnvironment);
  if (foundData != NULL) 
  {
    return foundData->m_value;
  };
	return emptyString;
}

ASLENTRY *A_ASLENTRY_L::Find(LPCTSTR key) const
{
  ASLEntryEnvironment.bTreeKey = key;
  return (ASLENTRY *)BTree::Find(&ASLEntryEnvironment);
}



//ASLENTRY::ASLENTRY(void) {
//	m_flags=0;
//}

ASLENTRY::ASLENTRY(const CString& key, const CString& value, unsigned char flags) {
  m_key = key;
	m_value= value;
	m_flags=flags;
}

ASLENTRY::~ASLENTRY(void) {
}


A_ASLENTRY_L& A_ASLENTRY_L::operator =(const A_ASLENTRY_L& asl) 
{
	POSITION p;
	//CString key;
	const ASLENTRY *pEntry;
  if (&asl==this) return *this;
  BTree::Clear(&ASLEntryEnvironment);
	p=asl.GetStartPosition();
	while (p!=NULL) 
  {
		asl.GetNextAssoc(p, &pEntry);
		Insert(pEntry);
	};
  return *this;
}

void A_ASLENTRY_L::Copy(CArray<CStringPAIR, CStringPAIR&> *pArray) 
{
	int i, n;
	//CString key;
  BTree::Clear(&ASLEntryEnvironment);
  n = pArray->GetSize();
	for (i=0; i<n; i++)
  {
    ASLENTRY ent((*pArray)[i].Key(), (*pArray)[i].Value(), 0);
		Insert(&ent);
	};
}

void A_ASLENTRY_L::Copy(const A_CStringPAIR_L& list) 
{
	POSITION p;
	//CString key;
	const CStringPAIR *pEntry;
  BTree::Clear(&ASLEntryEnvironment);
	p=list.GetStartPosition();
	while (p!=NULL) 
  {
		list.GetNextAssoc(p, &pEntry);
    ASLENTRY ent(pEntry->Key(), pEntry->Value(), 0);
		Insert(&ent);
	};
}

void A_ASLENTRY_L::Copy(const A_ASLENTRY_L& list)
{
  *this = list;
}

void A_ASLENTRY_L::Swap(A_ASLENTRY_L& list)
{
  BTree::Swap(list);
}


A_ASLENTRY_L::~A_ASLENTRY_L(void)
{

  BTree::Clear(&ASLEntryEnvironment);
}

BOOL A_ASLENTRY_L::operator ==(const A_ASLENTRY_L& asl) const
{
  POSITION pos1, pos2;
  const ASLENTRY *pEntry1, *pEntry2;
  pos1 = GetStartPosition();
  pos2 = asl.GetStartPosition();
  while ((pos1!=NULL) && (pos2!=NULL))
  {
    GetNextAssoc(pos1, &pEntry1);
    asl.GetNextAssoc(pos2, &pEntry2);
    if (!(*pEntry1 == *pEntry2)) return FALSE;
  };
  return (pos1 == pos2);
}

BOOL ASLENTRY::operator ==(const ASLENTRY& atv) const
{
  if (m_flags!=atv.m_flags) return false;
  if (m_value!=atv.m_value) return false;
  return true;
}

void ASLENTRY::Serialize(CAR *pCar) const
{
  (*pCar) << *(CString *)&m_key;
  (*pCar) << m_flags;
  (*pCar) << *(CString *)&m_value;
}

void ASLENTRY::Serialize(CArchive *pAr) const
{
  (*pAr) << m_key;
  (*pAr) << m_flags;
  (*pAr) << m_value;  
}

#ifdef UAFEDITOR
const char *JKEY_KEY = "key";
const char *JKEY_ORIGKEY = "origKey";
extern const char *JKEY_VALUE;
const char *JKEY_FLAGS = "flags";
const char *JKEY_ENTRY = "entry";

void ASLENTRY::Export(JWriter& jw) const
{
  jw.StartList();
  jw.Linefeed(false);
  jw.NameAndQuotedValue(JKEY_KEY, m_key); 
  jw.NameAndQuotedValue(JKEY_VALUE, m_value); 
  jw.NameAndValue(JKEY_FLAGS, m_flags); 
  jw.EndList();
  jw.Linefeed(true);
}
#endif

void ASLENTRY::DeSerialize(CAR *pCar)
{
  int i, n;
  (*pCar) >> m_key;
  n = m_key.GetLength();
  for (i=0; i<n; i++)
  {
    if (m_key.GetAt(i) < 0x20)
    {
      m_key.SetAt(i, m_key.GetAt(i) + 0x20);
    };
  };

  (*pCar) >> m_flags;
  (*pCar) >> m_value;
}

void ASLENTRY::DeSerialize(CArchive *pAr)
{
  (*pAr) >> m_key;
  (*pAr) >> m_flags;
  (*pAr) >> m_value;
}

int ASLENTRY::GetDataSize(void) const
{
  return sizeof(ASLENTRY) + m_value.GetLength();
}


int A_ASLENTRY_L::GetDataSize(void) const
{
  int size;
  POSITION pos;
  const ASLENTRY *pEntry;
  size = sizeof (A_ASLENTRY_L);
  pos = GetStartPosition();
  while (pos != NULL)
  {
    GetNextAssoc(pos,&pEntry);
    size += pEntry->GetDataSize();
  };
  size += BTree::GetCount() * sizeof(BTreeNode);
  return size;
}

/*
int A_ASLENTRY_L::Walker(int id, int data) const
{
  int ID, FUNCID;
  ID     = ((int *)id)[0];
  FUNCID = ((int *)id)[1];
  return ((int (*)(int, ASLENTRY *))FUNCID)(ID, (ASLENTRY *)data);
}
*/
BOOL A_ASLENTRY_L::Insert(const CString& key, const CString& data, unsigned char flags)
{
  ASLENTRY *pASLEntry;
  ASLENTRY *foundData;
  pASLEntry = new ASLENTRY(key, data, flags);
//#ifdef _DEBUG
//  pASLEntry->id = NodeID++;
//#endif
  ASLEntryEnvironment.bTreeData = (int)pASLEntry;
  foundData = (ASLENTRY *)(BTree::Insert(&ASLEntryEnvironment));
  if (foundData != NULL)
  {
    foundData->m_value = pASLEntry->m_value;
    foundData->m_flags = pASLEntry->m_flags;
    delete pASLEntry;
    return TRUE;
  };
  return FALSE;
}

#ifdef UAFEDITOR
void A_ASLENTRY_L::Update(LPCSTR key, LPCSTR value, unsigned char flags)
{
  ASLENTRY *pOldEntry;
  ASSERT(strlen(key)>0);
  if (strlen(key)==0)
  {
    WriteDebugString("Warning: Attempt to update asl with empty key name ignored\n");
    //errorcount++;
    return;
  }
  if (strlen(value)==0) 
    WriteDebugString("Warning: Updating asl key \'%s\' with empty value\n", key);

  ASLENTRY *pNewAV = new ASLENTRY(key, value, flags);
  ASLEntryEnvironment.bTreeData = (int)pNewAV;
  pOldEntry = (ASLENTRY *)BTree::Insert(&ASLEntryEnvironment);
  if (pOldEntry != NULL)
  {
    pOldEntry->Value(value);
    pOldEntry->Flags(flags);
    delete pNewAV;
  };
}
#endif



BOOL A_ASLENTRY_L::Insert(const ASLENTRY *pEntry)
{
  return Insert(pEntry->Key(), pEntry->Value(), pEntry->Flags());
}

void A_ASLENTRY_L::GetNextAssoc(POSITION& pos, const ASLENTRY **ppEntry) const
{
  *ppEntry = (ASLENTRY *)BTree::GetNextAssoc(pos);
}


CArchive& A_ASLENTRY_L::Serialize(CArchive& ar, LPCSTR mapName)
{
	if (GetGameVersion(globalData) >= _ASL_LEVEL_) 
  {
		if (ar.IsStoring()) 
    {
      die("We should not be using CArchive to store ASL data");
		} 
    else 
    {
			CString temp;
      WORD count;
			ar >> temp;
			if (temp!=mapName) {
				CString msg;
				msg.Format("Expected:\n%s\nEncountered:\n%s\nwhile reading file.",
					mapName,temp);
        WriteDebugString(msg);

        MsgBoxError(msg);
        msg = ar.GetFile()->GetFileName();
        throw 7;
        //   I replaced the AfsThrowArchiveExecption because I could not
        //   figure out how to catch it properly.  It caused a memory leak.
 				//AfxThrowArchiveException(CArchiveException::badIndex,
        //          ar.GetFile()->GetFileName());
			};
      // 16-bit count; {CString key; char flags; CString value}
      ar >> count;
      for (;count>0; count--)
      {
        ASLENTRY tempASL("","",0);
        tempASL.DeSerialize(&ar);
        Insert(&tempASL);
      };
		};

	};
	return ar;
}


CAR& A_ASLENTRY_L::Serialize(CAR& ar, LPCSTR mapName)
{
	if (GetGameVersion(globalData) >= _ASL_LEVEL_) 
  {
		if (ar.IsStoring()) 
    {
      WORD count;
      POSITION p;
      const ASLENTRY *pEntry;
			ar << mapName;
      count = GetCount();
      ar << count;
      p = GetStartPosition();
      while (p != NULL)
      {
        GetNextAssoc(p, &pEntry);
        pEntry->Serialize(&ar);
      };
		} 
    else 
    {
			CString temp;
      WORD count;
			ar >> temp;
			if (temp!=mapName) {
				CString msg;
				msg.Format("Expected:\n%s\nEncountered:\n%s\nwhile reading file.",
					mapName,temp);
        WriteDebugString(msg);

        MsgBoxError(msg);
        msg = ar.GetFile()->GetFileName();
        throw 7;
        //   I replaced the AfsThrowArchiveExecption because I could not
        //   figure out how catch it properly.  It caused a memory leak.
 				//AfxThrowArchiveException(CArchiveException::badIndex,
        //          ar.GetFile()->GetFileName());
			};
      // 16-bit count; {CString key; char flags; CString value}
      ar >> count;
      for (;count>0; count--)
      {
        ASLENTRY tempASL("","",0);
        tempASL.DeSerialize(&ar);
        Insert(&tempASL);
      };
		};

	};
	return ar;
}

#ifdef UAFEDITOR
const char *JKEY_ASL = "asl";

void A_ASLENTRY_L::Export(JWriter& jw)
{
  POSITION p;
  const ASLENTRY *pEntry;
  if (GetCount() == 0) return;
  p = GetStartPosition();
  jw.StartArray(JKEY_ASL);
  while (p != NULL)
  {
    GetNextAssoc(p, &pEntry);
    jw.NextEntry();
    pEntry->Export(jw);
  };
  jw.EndArray();
}

bool A_ASLENTRY_L::Import(JReader& jr)
{
  if (jr.Optional(),jr.StartArray(JKEY_ASL))
  {
    CString name, value;
    int flags;
    while (jr.Optional(),jr.ListEntry())
    {
      jr.NameAndQuotedValue(JKEY_KEY, name); 
      jr.NameAndQuotedValue(JKEY_VALUE, value); 
      jr.NameAndValue(JKEY_FLAGS, flags); 
      Insert(name, value, flags);
      jr.EndList();
    };
    jr.EndArray();
    return true;
  };
  return false;
}

#endif
CAR& A_ASLENTRY_L::Save(CAR& car, LPCSTR mapName) const
{
	WORD count=0;
	POSITION pos;
	CString tempkey;
	const ASLENTRY *pEntry;
	car << mapName;
	pos=GetStartPosition();
	while (pos!=NULL)
	{ 
		GetNextAssoc(pos, &pEntry);
		if ((pEntry->m_flags & ASLF_READONLY)==0) count++;
	};
	car << count;
  if (count!=0) // Some ASLs contain only read-only entries.
  {             // This saves the overhead of searching through the 
                // list twice.  It probably ain't a big deal either way.
	  pos=GetStartPosition();
	  while (pos!=NULL) 
	  {
		  GetNextAssoc(pos,&pEntry);
		  if ((pEntry->m_flags & ASLF_READONLY)==0)
		  {
        // I'm not going to check for empty string.  I don't believe
        // that it can cause a problem.  Mr. Turner has yet to 
        // convince me.  Besides, if empty strings are a problem then
        // we have to rewrite ASL::Serialize(CArchive& ar)
        // because it uses the CMap::Serialize which does not check
        // for empty strings.  So there!  PRS
			  pEntry->Serialize(&car);
			  //SerializeElements(ar, &pEntry, 1);
        count--;
		  };
	  };
  };
  ASSERT(count==0);
	return car;
}

void A_ASLENTRY_L::CommitRestore(const A_ASLENTRY_L *pList)
{
	// Delete current non-READONLY attributes and add all
	// non-READONLY attributes from src.
	POSITION pos;
	const ASLENTRY *pEntry;
	CString tempkey;
	pos=GetStartPosition();
	// Discard all current non-READONLY entries before adding
	// the entries from src.
	while (pos!=NULL) 
	{
		GetNextAssoc(pos, &pEntry);
		if ((pEntry->m_flags & ASLF_READONLY)==0)
		{  // Retain only the READONLY entries
			this->Delete(pEntry->m_key);
		};
	};
	// Now copy all non-READONLY attributes from src.
	pos=pList->GetStartPosition();
	while (pos!=NULL) 
	{
    const ASLENTRY *pASL;
		pList->GetNextAssoc(pos,&pASL);
		if ((pASL->m_flags & ASLF_READONLY)==0)
		{
			Insert(pASL);
		};
	};
}


CString A_ASLENTRY_L::Delete(LPCSTR key)
{
  ASLENTRY *result;
  CString value; 
  ASLEntryEnvironment.bTreeKey = key;
  result = (ASLENTRY *)BTree::Delete(&ASLEntryEnvironment);
  if (result != NULL) 
  {
    value = result->m_value;
    delete result;
    return value;
  }
  return CString("");
}


void A_ASLENTRY_L::Clear(void)
{
  BTree::Clear(&ASLEntryEnvironment);
}

// ****************************************************************
//
//
//        A_CString_L
//
// ****************************************************************

void CString_DELETE(int a);
int  CString_COMPARE_KEY(int a);  // sign(a-key)
int  CString_COMPARE_ENTRY(int a, int b);  // sign(a-b)

BTREE_ENVIRONMENT CStringEnvironment =
{
  CString_COMPARE_KEY,
  CString_COMPARE_ENTRY,
  CString_DELETE
};

A_CString_L::~A_CString_L(void)
{

  BTree::Clear(&CStringEnvironment);
}



BOOL A_CString_L::Insert(const CString& key)
{
  CString *pEntry, *foundData;
  pEntry = new CString;
  *pEntry = key;
  CStringEnvironment.bTreeData = (int)pEntry;
  foundData = (CString *)(BTree::Insert(&CStringEnvironment));
  if (foundData != NULL)
  {
    *foundData = *pEntry;
    delete pEntry;
    return TRUE;
  };
  return FALSE;
}





void CStringPAIR_DELETE(int a);
int CStringPAIR_COMPARE_KEY(int a);  // sign(a-key)
int CStringPAIR_COMPARE_ENTRY(int a, int b);  // sign(a-b)

BTREE_ENVIRONMENT CStringPAIREnvironment =
{
  CStringPAIR_COMPARE_KEY,
  CStringPAIR_COMPARE_ENTRY,
  CStringPAIR_DELETE,
  0
};



int CStringPAIR::Compare(LPCSTR str) const
{
  return m_key.Compare(str);
}

int CStringPAIR::Compare(const CStringPAIR *p) const
{
  return m_key.Compare(p->m_key);
}

void CStringPAIR_DELETE(int a)
{
  delete (CStringPAIR *)a;
}

int CStringPAIR_COMPARE_KEY(int a)  // sign(a-key)
{
  return ((CStringPAIR *)a)->Compare(CStringPAIREnvironment.bTreeKey);
}

int CStringPAIR_COMPARE_ENTRY(int a, int b)  // sign(a-b)
{
  return ((CStringPAIR *)a)->Compare((CStringPAIR *)b);
}

bool CStringPAIR::operator ==(const CStringPAIR& p) const
{
  return (m_key == p.m_key) && (m_value == p.m_value);
}

CStringPAIR::CStringPAIR(void)
{
}

CStringPAIR::CStringPAIR(const CString& key, const CString& value)
{
  m_key = key;
  m_value = value;
};

CStringPAIR& CStringPAIR::operator =(const CStringPAIR& p)
{
  m_key = p.m_key; m_value = p.m_value;
  return *this;
}

CStringPAIR& CStringPAIR::operator =(const ASLENTRY& p)
{
  m_key = p.Key(); m_value = p.Value();
  return *this;
}


A_CStringPAIR_L::~A_CStringPAIR_L(void)
{

  BTree::Clear(&CStringPAIREnvironment);
}

void A_CStringPAIR_L::Copy(const A_CStringPAIR_L& src)
{
  *this = src;
}

void A_CStringPAIR_L::Clear(void)
{
  BTree::Clear(&CStringPAIREnvironment);
}

CStringPAIR *A_CStringPAIR_L::Find(LPCTSTR key) const
{
  CStringPAIR *foundData;

  CStringPAIREnvironment.bTreeKey = key;
  foundData = (CStringPAIR *)BTree::Find(&CStringPAIREnvironment);
  if (foundData != NULL) 
  {
    return foundData;
  };
	return NULL;
}




const CString& A_CStringPAIR_L::Lookup(LPCTSTR key) const
{
  const CStringPAIR *foundData;

  CStringPAIREnvironment.bTreeKey = key;
  foundData = (CStringPAIR *)BTree::Find(&CStringPAIREnvironment);
  if (foundData != NULL) 
  {
    return foundData->m_value;
  };
	return emptyString;
}



void CString_DELETE(int a)
{
  delete (CString *)a;
}

int CString_COMPARE_KEY(int a)  // sign(a-key)
{
  return (*(CString *)a).Compare(CStringEnvironment.bTreeKey);
}

int CString_COMPARE_ENTRY(int a, int b)  // sign(a-b)
{
  return (*(CString *)a).Compare(*(CString *)b);
}


BOOL A_CStringPAIR_L::Insert(const CString& key, const CString& value)
{

  CStringPAIR *pEntry, *foundData;
  pEntry = new CStringPAIR;
  pEntry->m_key = key;
  pEntry->m_value = value;
  CStringPAIREnvironment.bTreeData = (int)pEntry;
  foundData = (CStringPAIR *)(BTree::Insert(&CStringPAIREnvironment));
  if (foundData != NULL)
  {
    *foundData = *pEntry;
    delete pEntry;
    return TRUE;
  };
  return FALSE;
}

BOOL A_CStringPAIR_L::Insert(const CStringPAIR& pEntry)
{
  return Insert(pEntry.Key(), pEntry.Value());
}


CString A_CStringPAIR_L::Delete(LPCSTR key)
{
  CStringPAIR *result;
  CStringPAIREnvironment.bTreeKey = key;
  CString value; 
  result = (CStringPAIR *)BTree::Delete(&CStringPAIREnvironment);
  if (result != NULL) 
  {
    value = result->m_value;
    delete result;
    return value;
  }
  else
  {
    return NO_SUCH_SA;
  };
}


BOOL A_CStringPAIR_L::operator ==(const A_CStringPAIR_L& list) const
{
  POSITION pos1, pos2;
  const CStringPAIR *pEntry1, *pEntry2;
  pos1 = GetStartPosition();
  pos2 = list.GetStartPosition();
  while ((pos1!=NULL) && (pos2!=NULL))
  {
    GetNextAssoc(pos1, &pEntry1);
    GetNextAssoc(pos2, &pEntry2);
    if (!(*pEntry1 == *pEntry2)) return FALSE;
  };
  return (pos1 == pos2);
}


void A_CStringPAIR_L::GetNextAssoc(POSITION& pos, const CStringPAIR **ppEntry) const
{
  *ppEntry = (CStringPAIR *)BTree::GetNextAssoc(pos);
}

CArchive& A_CStringPAIR_L::Serialize(CArchive& ar)
{
  int count;
  POSITION pos;
  const CStringPAIR *pEntry;
  if (ar.IsStoring())
  {
    count = GetCount();
    ar << count;
    pos = GetStartPosition();
    while (pos != NULL)
    {
      count--;
      GetNextAssoc(pos, &pEntry);
      ar << pEntry->m_key;
      ar << pEntry->m_value;
    };
    if (count != 0)
    {
      MsgBoxInfo("Special Abilities inconsistent.  Bad save", "Error");
    };
  }
  else
  {
    Clear();
    
    for (ar >> count; count!=0; count--)
    {
      CString key, value;
      ar >> key;
      ar >> value;
      Insert(key, value);
    };
  };
  return ar;
}


CAR& A_CStringPAIR_L::Serialize(CAR& car)
{
  int count;
  POSITION pos;
  const CStringPAIR *pEntry;
  if (car.IsStoring())
  {
    count = GetCount();
    car << count;
    pos = GetStartPosition();
    while (pos != NULL)
    {
      count--;
      GetNextAssoc(pos, &pEntry);
      car << *(CString *)&pEntry->m_key;
      car << *(CString *)&pEntry->m_value;
    };
    if (count != 0)
    {
      MsgBoxInfo("Special Abilities inconsistent.  Bad save", "Error");
    };
    return car;
  }
  else
  {
    Clear();
    
    for (car >> count; count!=0; count--)
    {
      CString key, value;
      car >> key;
      car >> value;
      Insert(key, value);
    };
    return car;
  };
}



#ifdef UAFEDITOR

extern const char *JKEY_NAME;
extern const char *JKEY_SPECIALABILITIES = "specialAbilities";
void A_CStringPAIR_L::Export(JWriter& jw)
{
  POSITION pos;
  const CStringPAIR *pEntry;
  jw.StartArray(JKEY_SPECIALABILITIES);
  jw.Linefeed(false);
  pos = GetStartPosition();
  while (pos != NULL)
  {
    jw.NextEntry();
    jw.StartList();
    GetNextAssoc(pos, &pEntry);
    jw.NameAndValue(JKEY_NAME, pEntry->m_key);
    jw.NameAndValue(JKEY_VALUE, pEntry->m_value);
    jw.EndList();
  };
  jw.EndArray();
  jw.Linefeed(true);
}
void A_CStringPAIR_L::Import(JReader& jr)
{
  CStringPAIR entry;
  jr.StartArray(JKEY_SPECIALABILITIES);
  jr.Linefeed(false);
  while (jr.Optional(), jr.NextEntry())
  {
    jr.StartList();
    jr.NameAndValue(JKEY_NAME, entry.m_key);
    jr.NameAndValue(JKEY_VALUE, entry.m_value);
    Insert(entry);
    jr.EndList();
  };
  jr.EndArray();
  jr.Linefeed(true);
}
#endif


A_CStringPAIR_L& A_CStringPAIR_L::operator =(const A_CStringPAIR_L& list) 
{
	POSITION p;
	CString key;
	const CStringPAIR *pEntry;
  if (&list==this) return *this;
  BTree::Clear(&CStringPAIREnvironment);
	p=list.GetStartPosition();
	while (p!=NULL) 
  {
		list.GetNextAssoc(p, &pEntry);
		Insert(*pEntry);
	};
  return *this;
}

void A_CStringPAIR_L::Copy(const A_ASLENTRY_L& list) 
{
	POSITION p;
	CString key;
	const ASLENTRY *pEntry;
  BTree::Clear(&CStringPAIREnvironment);
	p=list.GetStartPosition();
	while (p!=NULL) 
  {
		list.GetNextAssoc(p, &pEntry);
		Insert(pEntry->Key(), pEntry->Value());
	};
}

int A_CStringPAIR_L::GetDataSize(void) const
{
  POSITION pos;
  int size;
  const CStringPAIR *pEntry;
  pos = GetStartPosition();
  size = sizeof (*this);
  while (pos != NULL)
  {
    GetNextAssoc(pos, &pEntry);
    size += sizeof(*pEntry);
    size += pEntry->Key().GetLength();
    size += pEntry->Value().GetLength();
  };
  return size;
}



void CStringInteger_DELETE(int a);
int CStringInteger_COMPARE_KEY(int a);  // sign(a-key)
int CStringInteger_COMPARE_ENTRY(int a, int b);  // sign(a-b)

BTREE_ENVIRONMENT CStringIntegerEnvironment =
{
  CStringInteger_COMPARE_KEY,
  CStringInteger_COMPARE_ENTRY,
  CStringInteger_DELETE,
  0
};


CStringInteger::CStringInteger(void)
{
  m_value = 0x80000000;
}

CStringInteger& CStringInteger::operator =(const CStringInteger& p)
{
  m_key = p.m_key; m_value = p.m_value;
  return *this;
}

A_CStringInteger_L::~A_CStringInteger_L(void)
{

  BTree::Clear(&CStringIntegerEnvironment);
}



void CStringInteger_DELETE(int a)
{
  delete (CStringInteger *)a;
}

int CStringInteger::Compare(LPCSTR str) const
{
  return m_key.Compare(str);
}

int CStringInteger::Compare(const CStringInteger *p) const
{
  return m_key.Compare(p->m_key);
}



int CStringInteger_COMPARE_KEY(int a)  // sign(a-key)
{
  return ((CStringInteger *)a)->Compare(CStringIntegerEnvironment.bTreeKey);
}

int CStringInteger_COMPARE_ENTRY(int a, int b)  // sign(a-b)
{
  return ((CStringInteger *)a)->Compare((CStringInteger *)b);
}


int *A_CStringInteger_L::Find(LPCTSTR key) const
{
  CStringInteger *foundEntry;

  CStringIntegerEnvironment.bTreeKey = key;
  foundEntry = (CStringInteger *)BTree::Find(&CStringIntegerEnvironment);
  if (foundEntry != NULL) 
  {
    return &foundEntry->m_value;
  };
	return NULL;
}


BOOL A_CStringInteger_L::Insert(const CString& key, int value)
{

  CStringInteger *pEntry, *foundData;
  pEntry = new CStringInteger;
  pEntry->m_key = key;
  pEntry->m_value = value;
  CStringIntegerEnvironment.bTreeData = (int)pEntry;
  foundData = (CStringInteger *)(BTree::Insert(&CStringIntegerEnvironment));
  if (foundData != NULL)
  {
    *foundData = *pEntry;
    delete pEntry;
    return TRUE;
  };
  return FALSE;
}




// ****************************************************************
//
//        A_SPECABILITY_DEF_L
//
// ****************************************************************


int SPECABILITY_DEF_COMPARE_KEY(int a);
int SPECABILITY_DEF_COMPARE_ENTRY(int a, int b);
void SPECABILITY_DEF_DELETE(int a);


BTREE_ENVIRONMENT specAbilityDefEnvironment =
{
  SPECABILITY_DEF_COMPARE_KEY,
  SPECABILITY_DEF_COMPARE_ENTRY,
  SPECABILITY_DEF_DELETE,
  0
};

void SPECABILITY_DEF_DELETE(int a)
{
  delete (SPECABILITY_DEF *)a;
}

int SPECABILITY_DEF_COMPARE_ENTRY(int a, int b)
{
  return ((SPECABILITY_DEF *)a)->Compare(*(SPECABILITY_DEF *)b);
}

int SPECABILITY_DEF_COMPARE_KEY(int a)
{
  return ((SPECABILITY_DEF *)a)->Compare(specAbilityDefEnvironment.bTreeKey);
}

A_SPECABILITY_DEF_L::A_SPECABILITY_DEF_L(void)
{
}

int A_SPECABILITY_DEF_L::GetCount(void) const
{
  return BTree::GetCount();
}

void A_SPECABILITY_DEF_L::Copy(const A_SPECABILITY_DEF_L& list)
{
  *this = list;
}

A_SPECABILITY_DEF_L::~A_SPECABILITY_DEF_L(void)
{
  Clear();
}
/*
void A_SPECABILITY_DEF_L::SwapList(const CString& name, SPECABILITY_DEF& specAb)
{
  SPECABILITY_DEF *pEntry;
  BTreeNode *pTemp;
  pEntry = FindAbility(name);
  pTemp = specAb.m_root;
  specAb.m_root = m_root;
  m_root = pTemp;
}
*/
void A_SPECABILITY_DEF_L::Clear(void)
{
  BTree::Clear(&specAbilityDefEnvironment);
}

SPECABILITY_DEF *A_SPECABILITY_DEF_L::GetNext(POSITION& pos) const
{
  return (SPECABILITY_DEF *)BTree::GetNextAssoc(pos);
}

void A_SPECABILITY_DEF_L::InsertString(LPCSTR abilityName, 
                                       const CString& key,
                                       const CString& value,
                                       unsigned char flags)
{
  SPECABILITY_DEF *pSpecAb;
  specAbilityDefEnvironment.bTreeKey = abilityName;
  pSpecAb = (SPECABILITY_DEF *)BTree::Find(&specAbilityDefEnvironment);
  if (pSpecAb == NULL) return;
  pSpecAb->Insert(key, value, flags);
}

void A_SPECABILITY_DEF_L::Delete(const CString& abilityName)
{
  specAbilityDefEnvironment.bTreeKey = abilityName;
  BTree::Delete(&specAbilityDefEnvironment);
}

A_SPECABILITY_DEF_L& A_SPECABILITY_DEF_L::operator=(const A_SPECABILITY_DEF_L& specDefList)
{
	POSITION p;
	const SPECABILITY_DEF *pEntry;
  SPECABILITY_DEF *newEntry;
  if (&specDefList==this) return *this;
  BTree::Clear(&specAbilityDefEnvironment);
	p = specDefList.GetStartPosition();
	while (p!=NULL) 
  {
		pEntry = specDefList.GetNext(p);
    newEntry = new SPECABILITY_DEF;
    newEntry->Copy(*pEntry);
    specAbilityDefEnvironment. bTreeData = (int)newEntry;
    BTree::Insert(&specAbilityDefEnvironment);
	};
  return *this;
}

  


POSITION A_SPECABILITY_DEF_L::GetStartPosition(void) const
{
  return BTree::GetStartPosition();
}

SPECABILITY_DEF *A_SPECABILITY_DEF_L::FindAbility(specialAbilitiesType sa) const
{
  specAbilityDefEnvironment.bTreeKey = sa;
  return (SPECABILITY_DEF *)BTree::Find(&specAbilityDefEnvironment);
}

const CString& A_SPECABILITY_DEF_L::FindString(specialAbilitiesType sa, LPCSTR name)
{
  const SPECABILITY_DEF *pEntry;
  pEntry = FindAbility(sa);
  if (pEntry == NULL) return emptyString;
  return pEntry->Lookup(name);
}

void A_SPECABILITY_DEF_L::DeleteString(specialAbilitiesType sa, LPCSTR name)
{SPECABILITY_DEF *pEntry;
  pEntry = FindAbility(sa);
  if (pEntry == NULL) return;
  pEntry->Delete(name);
}

const CString& A_SPECABILITY_DEF_L::spellAbilitiesText(specialAbilitiesType sa) const
{
  die ("Not Needed?"); //Not Implemented(0x991af, FALSE);
  return *(CString *)NULL;
}

SPECABILITY_DEF *A_SPECABILITY_DEF_L::InsertAbility(const CString& specAbilityName)
{
  SPECABILITY_DEF *pSpecAbilityDef;
  SPECABILITY_DEF *foundData;
  pSpecAbilityDef = new SPECABILITY_DEF;
  pSpecAbilityDef->m_specAbName = specAbilityName;
//#ifdef _DEBUG
//  pASLEntry->id = NodeID++;
//#endif
  specAbilityDefEnvironment.bTreeData = (int)pSpecAbilityDef;
  foundData = (SPECABILITY_DEF *)(BTree::Insert(&specAbilityDefEnvironment));
  if (foundData != NULL)
  {
    foundData->Swap(*pSpecAbilityDef); // Update contents
    delete pSpecAbilityDef;
    return foundData;
  };
  return pSpecAbilityDef;
}

bool A_SPECABILITY_DEF_L::operator == (const A_SPECABILITY_DEF_L& src) const
{

  POSITION pos1, pos2;
  const SPECABILITY_DEF *pEntry1, *pEntry2;
  pos1 = GetStartPosition();
  pos2 = src.GetStartPosition();
  while ((pos1!=NULL) && (pos2!=NULL))
  {
    pEntry1 = GetNext(pos1);
    pEntry2 = src.GetNext(pos2);
    if (!(*pEntry1 == *pEntry2)) return FALSE;
  };
  return (pos1 == pos2);
}

void A_SPECABILITY_DEF_L::Serialize(CAR& car)
{
  if (car.IsStoring())
  {
    int count;
    POSITION pos;
    SPECABILITY_DEF *pSpecAb;
    car << "SpecAbVer01";
    car.Compress(true);
    count = GetCount();
    car << count;
    pos = GetStartPosition();
    while (pos != NULL)
    {
      pSpecAb = GetNext(pos);
      pSpecAb->Serialize(car);
      count--;
    };
    if (count != 0)
    {
      MsgBoxInfo("Error writing special abilities","error");
    };
  }
  else
  {
    int count;
    SPECABILITY_DEF *pSpecAb;
    Clear();
    CString version;
    CString name;
    car >> version;
    car.Compress(true);
    car >> count;
    while (count != 0)
    {
      int i, n;
      car >> name;
      n = name.GetLength();
      for (i=0; i<n; i++)
      {
        if (name.GetAt(i) < 0x20)
        {
          name.SetAt(i, name.GetAt(i) + 0x20);
        };
      }
      pSpecAb = InsertAbility(name);
      pSpecAb->m_abilityStrings.Serialize(car, "SPECIAL_ABILITIES_DB");
      count--;
    };
  };
}

/*
// *****************************************************************
//
//
//             A_SPECABILITY_L
//
// *****************************************************************

void SPECABILITY_DELETE(int a);
int SPECABILITY_COMPARE_ENTRY(int a, int b);
int SPECABILITY_COMPARE_KEY(int a);


BTREE_ENVIRONMENT specAbilityEnvironment =
{
  SPECABILITY_COMPARE_KEY,
  SPECABILITY_COMPARE_ENTRY,
  SPECABILITY_DELETE,
  0
};

void SPECABILITY_DELETE(int a)
{
  delete (SPECABILITY *)a;
}

int SPECABILITY_COMPARE_ENTRY(int a, int b)
{
  return ((SPECABILITY *)a)->Compare(*(SPECABILITY *)b);
}

int SPECABILITY_COMPARE_KEY(int a)
{
  return ((SPECABILITY *)a)->Name().Compare(specAbilityEnvironment.bTreeKey);
}

A_SPECABILITY_L &A_SPECABILITY_L::operator=(const A_SPECABILITY_L& specAb)
{
	POSITION p;
	CString key;
	const SPECABILITY *pEntry;
  if (&specAb==this) return *this;
  BTree::Clear(&specAbilityEnvironment);
	p=specAb.GetStartPosition();
	while (p!=NULL) 
  {
		specAb.GetNextAssoc(p, &pEntry);
		Insert(pEntry->m_specAbName);
	};
  return *this;
}

void A_SPECABILITY_L::GetNextAssoc(POSITION& pos, const SPECABILITY **ppEntry) const
{
  *ppEntry = (SPECABILITY *)BTree::GetNextAssoc(pos);
}

const SPECABILITY *A_SPECABILITY_L::Find(specialAbilitiesType sa) const
{
  specAbilityEnvironment.bTreeKey = sa;
  return (SPECABILITY *)BTree::Find(&specAbilityEnvironment);
}


A_SPECABILITY_L::~A_SPECABILITY_L(void)
{
  Clear();
}

void A_SPECABILITY_L::Clear(void)
{
  BTree::Clear(&specAbilityEnvironment);
}

const SPECABILITY *A_SPECABILITY_L::Insert(LPCSTR specAbilityName)
{
  SPECABILITY *pSpecAbility;
  SPECABILITY *foundData;
  pSpecAbility = new SPECABILITY;
  pSpecAbility->m_specAbName = specAbilityName;
//#ifdef _DEBUG
//  pASLEntry->id = NodeID++;
//#endif
  specAbilityEnvironment.bTreeData = (int)pSpecAbility;
  foundData = (SPECABILITY *)(BTree::Insert(&specAbilityEnvironment));
  if (foundData != NULL)
  {
    foundData->m_parameters = pSpecAbility->m_parameters;
    delete pSpecAbility;
    return foundData;
  };
  return pSpecAbility;
}


POSITION A_SPECABILITY_L::GetStartPosition(void) const
{
  return BTree::GetStartPosition();
}

bool A_SPECABILITY_L::operator == (const A_SPECABILITY_L& src) const
{

  POSITION pos1, pos2;
  const SPECABILITY *pEntry1, *pEntry2;
  pos1 = GetStartPosition();
  pos2 = src.GetStartPosition();
  while ((pos1!=NULL) && (pos2!=NULL))
  {
    GetNextAssoc(pos1, &pEntry1);
    GetNextAssoc(pos2, &pEntry2);
    if (!(*pEntry1 == *pEntry2)) return FALSE;
  };
  return (pos1 == pos2);
}

*/

DEBUG_STRINGS debugStrings;

BOOL DEBUG_STRINGS::AlreadyNoted(LPCSTR string)
{
  return m_debugStrings.Insert(string);
}


//class A_ASLENTRY_L;
//#define TestingASL
#ifdef TestingASL
A_ASLENTRY_L list;
A_ASLENTRY_L list1;
int integer1;
int integer2;

class testing
{
public:
  testing(void);
};

void CheckNodes(BTreeNode *pNode)
{
  if (pNode == NULL) return;
  if (pNode->m_cntL == 0)
  {
    if (pNode->m_left.data != NULL)
    {
      MsgBoxInfo("Non-Null left pointer","Error");
    };
    return;
  };
  if (pNode->m_cntR == 0)
  {
    if (pNode->m_right.data != NULL)
    {
      MsgBoxInfo("Non-Null right pointer","Error");
    };
    return;
  };
  if (pNode->m_cntL > 1) CheckNodes(pNode->m_left.pNode);
  if (pNode->m_cntR > 1) CheckNodes(pNode->m_right.pNode);
}

void CheckList(bool print)
{
  CString prev = "0";
  CheckNodes(list.m_root);
  int count = list.GetCount();
  {
    POSITION pos;
    const ASLENTRY *pEntry;
    pos = list.GetStartPosition();
    while (pos!=NULL)
    {
      list.GetNextAssoc(pos, &pEntry);
      if (pEntry->Key() <= prev)
      {
        MsgBoxInfo("Error3","Error3");
      };
      prev = pEntry->Key();
      if (print) MsgBoxInfo(prev, "Entry");
      count--;
    };
  };
  if (count != 0)
  {
    MsgBoxInfo("Error","error");
  };
};


void TryDelete(CString key, CString data)
{
  CString result;
  result = list.Delete(key);
  if (result != data)
  {
    CString msg;
    msg.Format("Tried to delete %s=%s\nReceived %s",key,data,result);
    MsgBoxInfo(msg, "Error");
  };
  CheckList();
}

testing::testing(void)
{
  CString result;


  list.Insert("test","x",0);
  TryDelete("test", "x");
  if (list.GetCount() != 0)
  {
    MsgBoxInfo("Could not delete test","Error");
  };


  list.Insert("123", "abc", 7);
  CheckList();
  list.Insert("124", "abd", 8);
  CheckList();
  list.Insert("125", "abe", 9);
  CheckList();
  list.Insert("126", "abf", 10);
  CheckList();
  list.Insert("127", "abg", 11);
  CheckList();
  list.Insert("128", "abh", 12);
  CheckList();
  list.Insert("129", "abi", 13);
  CheckList();
  list.Insert("130", "abj", 14);
  CheckList();
  list.Insert("099", "aaz", 9);
  CheckList();
  list.Insert("098", "aay", 9);
  CheckList();
  list.Insert("097", "aax", 9);
  CheckList();
  list.Insert("096", "aaw", 9);
  CheckList();
  list.Insert("095", "aav", 9);
  CheckList();
  list.Insert("094", "aau", 9);
  CheckList();
  list.Insert("093", "aat", 9);
  CheckList();
  list.Insert("092", "aas", 9);
  CheckList();
  list.Insert("091", "aar", 9);
  CheckList();
  list.Insert("090", "aaq", 9);
  CheckList();

  {
    POSITION pos;
    const ASLENTRY *pEntry;
    pos = list.GetStartPosition();
    while (pos!=NULL)
    {
      list.GetNextAssoc(pos, &pEntry);
    };
  };

  TryDelete("130", "abj");
  TryDelete("098", "aay");
  TryDelete("129", "abi");
  TryDelete("097", "aax");
  TryDelete("128", "abh");
  TryDelete("091", "aar");
  TryDelete("096", "aaw");
  TryDelete("126", "abf");
  TryDelete("099", "aaz");
  TryDelete("095", "aav");
  TryDelete("127", "abg");
  TryDelete("124", "abd");
  TryDelete("123", "abc");
  TryDelete("092", "aas");
  TryDelete("093", "aat");
  TryDelete("090", "aaq");
  TryDelete("094", "aau");
  TryDelete("125", "abe");
  if (list.GetCount() != 0)
  {
    MsgBoxInfo("Not all deleted","Error");
  };
#define nument 10
  {
    int i, j, k, seed;
    bool present[nument];
    for (i=0; i<nument; i++) present[i] = false;
  
    srand(seed = (unsigned)time( NULL ));
    for (i=0; i<1000000; i++)
    {
      CString key,data;

      j = rand()%nument;
      key.Format("%03d", j);
      data.Format("data%03d", j);
      k = rand()%2;
      if (k == 0)
      {
        //Attempt an insert
        if (present[j])
        {
          CString msg;
          if (!list.Insert(key, data, j&0xff))
          {
            MsgBoxInfo("Error2","Error2");
          };
        }
        else
        {
          CString msg;
          if (list.Insert(key, data, j&0xff))
          {
            CString msg;
            msg.Format("Error1 i=%d j=%d", i, j);
            MsgBoxInfo(msg,"Error1");
          };
          present[j] = true;
        };
      }
      else
      {
        // Attempt a delete
        if (present[j])
        {
          if (list.Delete(key) != data)
          {
            MsgBoxInfo("Bad Delete","Error");
          };
          present[j] =false;
        }
        else
        {
        };
      };
      CheckList();
    };
  };

}
testing a;
#endif TestingASL
