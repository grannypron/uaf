/******************************************************************************
* Filename: SurfaceMgr.h
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
#ifndef __SURFACEMGR_H__
#define __SURFACEMGR_H__

#include "externs.h"

const int NUM_SURFACE_TYPES = 17;
enum SurfaceType { BogusDib=0, CommonDib=1, CombatDib=2, WallDib=4, DoorDib=8, 
                   BackGndDib=16, OverlayDib=32, IconDib=64, OutdoorCombatDib=128,
                   BigPicDib=256, MapDib=512, SmallPicDib=1024, SpriteDib=2048,
                   TitleDib=4096, BufferDib=8192, FontDib=16384, MouseDib=32768,
                   TransBufferDib=65536, SpecialGraphicsOpaqueDib = 0x20000, 
                   SpecialGraphicsTransparentDib = 0x40000};

const DWORD AllSurfTypes = 0xFFFFFFFF;


template <class T> struct SurfaceNode
{
  SurfaceNode() { key=-1; pSurf=NULL; type = BogusDib; }
  T GetSurfacePtr() { return pSurf; }

  long key;
  SurfaceType type;
  T pSurf;
};

template <class T> class SurfaceMgr
{
public:
  enum { MAX_SURFACES = 1000 };

  SurfaceMgr() { Clear(); m_name=""; }
  ~SurfaceMgr() { Clear(); }

  void SetName(CString name) { m_name = name; }
  void Clear() { m_Surfaces.RemoveAll(); m_SurfaceKey=1; }
  BOOL ValidSurface(long key) { return (GetSurfacePtr(key) != NULL); }

  long AddSurface(T pSurf, SurfaceType type);
  BOOL ChangeSurfaceType(long surfaceNum, SurfaceType type);
  BOOL RemoveSurface(long& surfaceNum);
  void RemoveSurfaceTypes(DWORD types);
  long GetSurfaceCount() { return m_Surfaces.GetCount(); }

  T GetSurfacePtr(long key);
  T GetSurfacePtr(long key, SurfaceType &type);

  POSITION GetHeadPosition() { return m_Surfaces.GetStartPosition(); }
  T GetNext(POSITION &pos) 
  { 
    SurfaceNode<T> node;   
    long key;
    m_Surfaces.GetNextAssoc(pos,key,node);
    return node.pSurf; 
  }
  T GetAt(POSITION pos) 
  { 
    SurfaceNode<T> node;   
    long key;
    m_Surfaces.GetNextAssoc(pos,key,node);
    return node.pSurf; 
  }
  long GetKeyAt(POSITION pos) 
  { 
    SurfaceNode<T> node;   
    long key;
    m_Surfaces.GetNextAssoc(pos,key,node);
    return node.key; 
  }

  typedef VOID (CALLBACK *PSURFACEMGRCALLBACK)(T pSurface);
  void IterateAllSurfaces(PSURFACEMGRCALLBACK Callback);

private:
  CString m_name;
  long m_SurfaceKey;

  long GetNextSurfaceKey() 
  { 
    long key=m_SurfaceKey;
    if (GetSurfaceCount() > 0)
    {
      if (key >= (INT_MAX-1))
      {
        // Need to wrap key.
        // Search for non-sequential gap between current keys.
        // A gap is guaranteed since the key range is greater than
        // MAX_SURFACES.
        // Gaps occur when deletions are made to the list
        long newKey = 1;
        SurfaceNode<T> node;
        while (m_Surfaces.Lookup(newKey, node))
          newKey++;
        key=newKey;
      }
      else
        key++;
    }
    else
      key = 1;
    m_SurfaceKey=key;
    return m_SurfaceKey;
  }

  BOOL GetSurface(long surfaceNum, SurfaceNode<T> &node);

  CMap<long, long, SurfaceNode<T>, SurfaceNode<T>&> m_Surfaces;
  //CList<SurfaceNode<T>, SurfaceNode<T>&> m_Surfaces;
};

template <class T> 
void SurfaceMgr<T>::IterateAllSurfaces(PSURFACEMGRCALLBACK Callback)
{
  SurfaceNode<T> tmp;
  long key;
  POSITION pos = m_Surfaces.GetStartPosition();
  while (pos != NULL)
  {
    m_Surfaces.GetNextAssoc(pos,key,tmp);
    Callback(tmp.pSurf);
  }
}

template <class T> 
long SurfaceMgr<T>::AddSurface(T pSurf, SurfaceType type)
{
  long key = -1;  
  SurfaceNode<T> node;
  
  if (GetSurfaceCount() >= MAX_SURFACES)
  {
    CString msg;
    msg.Format("%s:Reached max surface count, cannot load resource",m_name);
    MsgBoxError(msg);
    return key;
  }

  node.pSurf = pSurf;
  node.key = GetNextSurfaceKey();
  node.type = type;
  key = node.key;
  if (key > 0)
  {
    //m_Surfaces.AddTail(node);
    m_Surfaces.SetAt(key, node);
  }
   
  if (key < 0)
  {
    WriteDebugString("%s: ****** Failed to add surface !!!\n", m_name);
  }
  else
  {
    /*
#ifdef _DEBUG
    ASSERT( m_name != "" );
    WriteDebugString("SurfMgr(%s): add surf, total %u\n", 
      m_name,m_Surfaces.GetCount());
#endif
    */
  }
  
  return key;
}

template <class T> 
void SurfaceMgr<T>::RemoveSurfaceTypes(DWORD types)
{
  SurfaceNode<T> node;   
  long key;
  POSITION pos = m_Surfaces.GetStartPosition();
  //POSITION prevPos = NULL;
  
  while (pos != NULL)
  {
    //prevPos = pos;
    //node = m_Surfaces.GetNext(pos);
    m_Surfaces.GetNextAssoc(pos,key,node);
    
    if (node.type & types)
    {
      if (node.pSurf != NULL)
        delete node.pSurf;      
      m_Surfaces.RemoveKey(key);
    }
  }
  // don't do this
  //if (m_Surfaces.GetCount()==0) m_SurfaceKey=1;
  /*
#ifdef _DEBUG
  WriteDebugString("SurfMgr(%s): RemoveSurfaceTypes(), total %u\n", 
    m_name,m_Surfaces.GetCount());
#endif
  */
}

template <class T> 
BOOL SurfaceMgr<T>::RemoveSurface(long& surfaceNum)
{ 
  BOOL found = FALSE;
  
  SurfaceNode<T> node;
  if (GetSurface(surfaceNum, node))
  {
    if (node.pSurf != NULL)
      delete node.pSurf;      
    m_Surfaces.RemoveKey(surfaceNum);
    found=TRUE;
  }
  // don't do this
  //if (m_Surfaces.GetCount()==0) m_SurfaceKey=1;
  /*
#ifdef _DEBUG
  WriteDebugString("SurfMgr(%s): RemoveSurface(), total %u\n", 
    m_name,m_Surfaces.GetCount());
#endif
  */
  surfaceNum = -1;
  return found;
}

template <class T> 
T SurfaceMgr<T>::GetSurfacePtr(long key)
{
  SurfaceNode<T> node;   
  if (GetSurface(key, node))
    return node.pSurf;
  else
    return NULL;
}

template <class T> 
T SurfaceMgr<T>::GetSurfacePtr(long key, SurfaceType &type)
{
  SurfaceNode<T> node;   
  if (GetSurface(key, node))
  {
    type = node.type;
    return node.pSurf;
  }
  else
    return NULL;
}

template <class T> 
BOOL SurfaceMgr<T>::ChangeSurfaceType(long surfaceNum, SurfaceType type)
{
  if (surfaceNum < 0) return FALSE;
  if (GetSurfaceCount()<=0) return FALSE;
  SurfaceNode<T> node;
  if (m_Surfaces.Lookup(surfaceNum, node))
  {
    node.type=type;
    m_Surfaces[surfaceNum]=node;
    return TRUE;
  }
  return FALSE;
}

template <class T> 
BOOL SurfaceMgr<T>::GetSurface(long surfaceNum, SurfaceNode<T> &node)
{ 
  if (surfaceNum < 0) return FALSE;
  if (GetSurfaceCount()<=0) return FALSE;
  BOOL found = FALSE;
  found = m_Surfaces.Lookup(surfaceNum, node);  
  return found;
}

#endif