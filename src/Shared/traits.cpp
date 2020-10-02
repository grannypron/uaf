/******************************************************************************
* Filename: traits.cpp
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

#include "externs.h"
//#ifdef UAFEDITOR
//#else
//#endif

//#include "externs.h"
//#include "..\UAFWin\Dungeon.h"

#include "class.h"
#include "traits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CT_FUNCTION::CT_FUNCTION(void)
{
  Clear();
};

CT_FUNCTION::~CT_FUNCTION(void)
{
  Clear();
}

CT_FUNCTION& CT_FUNCTION::operator =(CT_FUNCTION& src)
{
  int i,n;
  if (&src==this) return *this;
  n=src.m_adjustment.GetSize();
  m_adjustment.SetSize(n);
  for (i=0; i<n; i++)
  {
    m_adjustment[i]=src.m_adjustment[i];
  };
  return *this;
}

BOOL CT_FUNCTION::operator ==(CT_FUNCTION& src)
{
  if (&src==this) return TRUE;
  if (m_adjustment.GetSize() != src.m_adjustment.GetSize()) return FALSE;
  for (int i=0; i<m_adjustment.GetSize(); i++)
    if ((m_adjustment[i] == src.m_adjustment[i])==FALSE) return FALSE;
  return TRUE;
}

void CT_FUNCTION::Clear(void)
{
  m_adjustment.RemoveAll();
}

CT_DEPENDENTS::CT_DEPENDENTS(void)
{
  Clear();
}

CT_DEPENDENTS::~CT_DEPENDENTS(void)
{
  Clear();
}

CT_DEPENDENTS& CT_DEPENDENTS::operator =(CT_DEPENDENTS& src)
{
  int i,n;
  if (&src==this) return *this;
  n=src.m_dependents.GetSize();
  m_dependents.SetSize(n);
  for (i=0; i<n; i++)
  {
    m_dependents[i]=src.m_dependents[i];
  };
  return *this;
}

BOOL CT_DEPENDENTS::operator ==(CT_DEPENDENTS& src)
{
  if (&src==this) return TRUE;
  if (m_dependents.GetSize() != src.m_dependents.GetSize()) return FALSE;
  for (int i=0; i<m_dependents.GetSize(); i++)
    if (m_dependents[i] != src.m_dependents[i]) return FALSE;
  return TRUE;
}

void CT_DEPENDENTS::Clear(void)
{
  m_dependents.RemoveAll();
}

struct InputTraitEntry
{
  CharacterTrait m_traitIndex;
  CString        m_traitName;
  CString        m_defaultComputation;
  InputTraitEntry(CharacterTrait ct, char *name, char *def) 
  {
    m_traitIndex=ct; m_traitName=name; m_defaultComputation=def;};
};

#define TID(a,b,c) InputTraitEntry((a) , (b), (c)),
//
// Translation table from traits.dat to our fixed-entry array of traits.
/*
Trait table.
 1) Enumerated index to trait.  CT_something
 2) Name of trait in traits.dat file
 3) Default computation
      Reference op1 value1 op2 value2 op3 value3
      Reference = Tn
         T=table:
            'X' = no reference = 0
         
*/

InputTraitEntry inputTraitTable[] = 
{
TID (CT_THAC0,    "THAC0", "X+20")
};


TRAITS chartraits; // Temporary for testing

TRAITS::TRAITS(void)
{
  Clear();
}

TRAITS::~TRAITS(void)
{
  Clear();
}

TRAITS& TRAITS::operator =(TRAITS& src)
{
  int i,n;
  if (&src==this) return *this;
  n=src.m_traitValues.GetSize();
  m_traitValues.SetSize(n);
  for (i=0; i<n; i++)
  {
    m_traitValues[i]=src.m_traitValues[i];
  };
  n=src.m_functions.GetSize();
  m_functions.SetSize(n);
  for (i=0; i<n; i++)
  {
    m_functions[i]=src.m_functions[i];
  };
  n=src.m_dependents.GetSize();
  m_dependents.SetSize(n);
  for (i=0; i<n; i++)
  {
    m_dependents[i]=src.m_dependents[i];
  };
  return *this;
}

BOOL TRAITS::operator==(TRAITS& src)
{
  if (&src==this) return TRUE;
  int i;
  if (m_traitValues.GetSize() != src.m_traitValues.GetSize()) return FALSE;
  if (m_functions.GetSize() != src.m_functions.GetSize()) return FALSE;
  if (m_dependents.GetSize() != src.m_dependents.GetSize()) return FALSE;
  for (i=0; i<m_traitValues.GetSize(); i++)
    if ((m_traitValues[i] == src.m_traitValues[i])==FALSE) return FALSE;
  for (i=0; i<m_functions.GetSize(); i++)
    if ((m_functions[i] == src.m_functions[i])==FALSE) return FALSE;
  for (i=0; i<m_dependents.GetSize(); i++)
    if ((m_dependents[i] == src.m_dependents[i])==FALSE) return FALSE;
  return TRUE;
}

void TRAITS::Clear(void)
{
  m_traitValues.RemoveAll();
  m_functions.RemoveAll();
  m_dependents.RemoveAll();
}