//******************************************
// copyright 1998	Mickey Kawick
// This file may be used anywhere for free, but this
// comment info must be left here.

// If you use this code in any shape or form or even
// as a reference for profit send me a dollar. 
// Not a dollar per copy, just a single US dollar. 

// If you are not in the US, send me
// a few different coins or the equivalent of a dollar.
// I love foreign currency, and I have a little collection.
// MKawick@sprintmail.com
//******************************************

//---------------------------------------------------------------------------
#include "..\Shared\stdafx.h"
//#include "memory.h"
#ifndef __TAG_LIST_H_
#include "taglist.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------

//**************************************************
//**************************************************
//**************************************************

TAG_LIST:: TAG_LIST (int num) 
{
	// init the BitMask list
		BitMask = new BYTE[8];
//	BYTE	bits[8] = { 1,2,4,8,16,32,64,128 };
	for (int i=0; i<8; i++)
		BitMask [i] = 1<<i;

	if (num == 0) ListLength = MAX_LIST;
	else ListLength = num;
	Tag = new BYTE [(ListLength>>BIT_SHIFT)+1];// 8bits
	Clear ();
}
//***********************
TAG_LIST:: TAG_LIST (TAG_LIST & TL) 
{
	// init the BitMask list
		BitMask = new BYTE[8];
	for (int i=0; i<8; i++)
		BitMask [i] = 1<<i;

	ListLength = TL.ListLength;

	Tag = new BYTE [(ListLength>>BIT_SHIFT)+1];// 8bits
	Clear ();
	Current = TL.Current;
	CountOn = TL.CountOn;
	memcpy (Tag, TL.Tag, (ListLength>>BIT_SHIFT)+1);
}
//***********************
TAG_LIST:: ~TAG_LIST () 
{
	if (Tag) delete [] Tag; 
	if (BitMask) delete [] BitMask; 
	BitMask = 0L;
}
//***********************
void TAG_LIST:: Copy (TAG_LIST& copy)
{
	// make sure that we have the space for copying
	if (ListLength > copy.ListLength) 
	{
		if (copy.Tag) delete [] copy.Tag;
		copy.Tag = new BYTE [(ListLength>>BIT_SHIFT)+1];
		copy.ListLength = ListLength;
		//Clear ();
	}
	copy.Current = Current;
	copy.CountOn = CountOn;
	memcpy (copy.Tag, Tag, (ListLength>>BIT_SHIFT)+1);
}
//***********************
void TAG_LIST:: Clear ()
{
	memset ( Tag, 0, (ListLength>>BIT_SHIFT) + 1);
	Current = 0, Last = 0, CountOn = 0;
}

//***********************
//***********************
int	 TAG_LIST:: GetTag (const int i) const
{
	if (i>=0 && i<ListLength) 
		return  (Tag[i / DIVIDE] & BitMask[i & WHICH_BIT]) ? 1:0;

	return 0;
}
//***********************
void TAG_LIST:: SetTag (const int i) 
{
	if (i>=0 && i<ListLength) 
	{	// set on
		if (!(Tag[i / DIVIDE] & BitMask[i & WHICH_BIT])) IncCountOn ();
		Tag[i / DIVIDE] |= BitMask[i & WHICH_BIT];
	}
}
//***********************
void TAG_LIST:: ClearTag (const int i)
{
	if (i>=0 && i<ListLength) 
	{	// set off
		if (Tag[i / DIVIDE] & BitMask[i & WHICH_BIT]) DecCountOn ();
		Tag[i / DIVIDE] &= ~(BitMask[i & WHICH_BIT]);
		Current = i>>BIT_SHIFT;
	}
}
//***********************
int	 TAG_LIST:: OnOffTag (const int i) const// simple test
{
	return (Tag[i / DIVIDE] & BitMask[i & WHICH_BIT]) ? 1:0;
}
//***********************
int	 TAG_LIST:: FlipTag (const int i)
{
	if (OnOffTag (i))
		ClearTag (i);
	else SetTag (i);
	return Tag[i / DIVIDE] & BitMask[i & WHICH_BIT];
}

//***********************
//***********************
int	 TAG_LIST:: FindOpenTag ()// uses the Current pointer to start, remember to 
//call FindOpenTag in continuous succession if you need this functionality.
{
	int NumBytes = (ListLength>>BIT_SHIFT)+1;
	int StartByte = Current;
	while (Tag[Current] == FULL && Current < NumBytes)// full
		{Current++;}
	if (Current >= NumBytes) 
	{
		Current = 0;// none available
		while (Tag[Current] == FULL && Current < StartByte)// full
			{Current++;}
		if (Current >= StartByte) return -1;// none available
	}

	// go bit by bit to find an available one
	int WhichBit = Current<<BIT_SHIFT;
	for (int i=0; i<DIVIDE; i++, WhichBit++)
	{
		if (GetTag (WhichBit) == 0) return WhichBit;
	}

	return -1;// nothing available
}
//***********************
int	 TAG_LIST:: FindFirstSet ()
{
	Last = 0;
	while (Last<ListLength)
	{
		if (GetTag (Last))break;
	}

	if (Last >= ListLength) return -1;
	return Last++;
}
//***********************
int	 TAG_LIST:: FindNextSet ()
{
	while (Last<ListLength)
	{
		if (GetTag (Last))break;
	}

	if (Last >= ListLength) return -1;
	return Last++;
}
//***********************
//***********************
int	 TAG_LIST:: CountSet () const
{
	return CountOn;
}
//***********************
//***********************
TAG_LIST&  TAG_LIST:: operator = (const TAG_LIST& TL)
{
  if (&TL == this)
    return *this;
	if (TL.ListLength != ListLength)
	{
		if (Tag) delete [] Tag; 
		ListLength = TL.ListLength;
		Tag = new BYTE [(ListLength>>BIT_SHIFT)+1];// 8bits
	}
	Clear ();
	Current = TL.Current;
	CountOn = TL.CountOn;
	memcpy (Tag, TL.Tag, (ListLength>>BIT_SHIFT)+1);
	return *this;
}
//***********************
bool TAG_LIST:: operator == (const TAG_LIST& TL) const
{
	if (ListLength != TL.ListLength) return false;
	int t = (ListLength>>BIT_SHIFT)+1;
	for (int i=0; i<t; i++)
	{
		if (Tag[i] == TL.Tag[i]) return false;
	}
	return true;
}
//***********************
bool TAG_LIST:: operator != (const TAG_LIST& TL) const
{
	return (! (*this == TL));
}
//***********************
int	 TAG_LIST:: operator [] (const int which) const
{
	if (which<0 || which>=ListLength) return 0;
	int val = GetTag (which);
	return val;
}
//***********************
//***********************
TAG_LIST&  TAG_LIST:: operator <<= (const int num)// num must be less than DIVIDE
{
	if (num<0) return (*this >>= num);

	int n = ListLength;
	
	while (n--)
	{
		if (GetTag(n-num)) SetTag (n);
		else ClearTag (n);
	}
	return *this;
}
//******
TAG_LIST  TAG_LIST:: operator << (const int num) 
{
	return (TAG_LIST (*this) <<= num);
}
//***********************
TAG_LIST& TAG_LIST:: operator >>= (const int num)// must be less than DIVIDE
{
	if (num<0) return (*this <<= num);

	int n = ListLength;
	
	for (int i=0; i<n; i++)
	{
		if (GetTag (i+num)) SetTag (i);
		else ClearTag (i);
	}
	return *this;
}
//******
TAG_LIST  TAG_LIST:: operator >> (const int num) 
{
	return (TAG_LIST (*this) >>= num);
}
//***********************
TAG_LIST& TAG_LIST:: operator |= (const TAG_LIST& TL)
{
	int len = (ListLength>>BIT_SHIFT)+1;
	int testlen = (TL.ListLength>>BIT_SHIFT)+1;
	if (testlen < len) len = testlen;// take the shorter of the 2

	for (int i=0; i<len; i++)
	{
		Tag[i] |= TL.Tag[i];
	}
	return *this;
}
//******
TAG_LIST  TAG_LIST:: operator | (const TAG_LIST& TL)
{
	return (TAG_LIST (*this) |= TL);
}
//***********************
TAG_LIST& TAG_LIST:: operator &= (const TAG_LIST& TL)
{
	int len = (ListLength>>BIT_SHIFT)+1;
	int testlen = (TL.ListLength>>BIT_SHIFT)+1;
	if (testlen < len) len = testlen;// take the shorter of the 2

	for (int i=0; i<len; i++)
	{
		Tag[i] &= TL.Tag[i];
	}
	return *this;
}
//******
TAG_LIST  TAG_LIST:: operator & (const TAG_LIST& TL)
{
	return (TAG_LIST (*this) &= TL);
}
//***********************
TAG_LIST& TAG_LIST:: operator ^= (const TAG_LIST& TL)
{
	int len = (ListLength>>BIT_SHIFT)+1;
	int testlen = (TL.ListLength>>BIT_SHIFT)+1;
	if (testlen < len) len = testlen;// take the shorter of the 2

	for (int i=0; i<len; i++)
	{
		Tag[i] ^= TL.Tag[i];
	}
	return *this;
}
//******
TAG_LIST TAG_LIST:: operator ^ (const TAG_LIST& TL)
{
	return (TAG_LIST (*this) ^= TL);
}
//***********************
//***********************
TAG_LIST TAG_LIST:: operator ~ ()
{
	TAG_LIST t (*this);
	int len = (ListLength>>BIT_SHIFT)+1;

	for (int i=0; i<len; i++)
	{
		t.Tag[i] = ~t.Tag[i];
	}
	return t;
}

//**************************************************
//**************************************************
//**************************************************

TAG_LIST_32:: TAG_LIST_32 (const int num) 
{
	// init the BitMask list
	BitMask = new DWORD[32];
/*	DWORD	bits[DIVIDE] = { 0x1, 0x2,0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 
	0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000,
	0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000,
	0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000};
*/
  for (int i=0; i<DIVIDE; i++)
		BitMask [i] = 1<<i;

	if (num == 0) ListLength = MAX_LIST;
	else ListLength = num;
	Tag = new DWORD [(ListLength>>BIT_SHIFT)+1];// 8bits
	Clear ();
}

//***********************

TAG_LIST_32:: TAG_LIST_32 (const TAG_LIST_32 & TL) 
{
	// init the BitMask list
		BitMask = new DWORD[8];
	for (int i=0; i<DIVIDE; i++)
		BitMask [i] = 1<<i;

	ListLength = TL.ListLength;

	Tag = new DWORD [(ListLength>>BIT_SHIFT)+1];// 8bits
	Clear ();
	Current = TL.Current;
	CountOn = TL.CountOn;
	memcpy (Tag, TL.Tag, ((ListLength>>BIT_SHIFT)+1)*4);
}

//***********************

TAG_LIST_32:: ~TAG_LIST_32 () 
{
	delete [] Tag; 
	if (BitMask) delete [] BitMask; 
	BitMask = 0L;
}

//***********************
//***********************

void TAG_LIST_32:: Copy (TAG_LIST_32& copy)
{
	// make sure that we have the space for copying
	if (ListLength > copy.ListLength) 
	{
		if (copy.Tag) delete [] copy.Tag;
		copy.Tag = new DWORD [(ListLength>>BIT_SHIFT)+1];
		copy.ListLength = ListLength;
		//Clear ();
	}
	copy.Current = Current;
	memcpy (copy.Tag, Tag, ((ListLength>>BIT_SHIFT)+1)*4);
}
//***********************
void TAG_LIST_32:: Clear ()
{
	memset ( Tag, 0, (ListLength>>BIT_SHIFT) + 1);// num is the number of vertices
	Current = 0, Last = 0, CountOn = 0;
}

//***********************
//***********************
//***********************
int	 TAG_LIST_32:: GetTag (const int i) const
{
	if (i>=0 && i<ListLength) 
	return  Tag[i / DIVIDE] & BitMask[i & WHICH_BIT];

	return 0;
}
//***********************
void TAG_LIST_32:: SetTag (const int i) 
{
	if (i>=0 && i<ListLength) 
	{
		if (!(Tag[i / DIVIDE] & BitMask[i & WHICH_BIT])) IncCountOn ();// set on
		Tag[i / DIVIDE] |= BitMask[i & WHICH_BIT];
	}
}
//***********************
void TAG_LIST_32:: ClearTag (const int i)
{
	if (i>=0 && i<ListLength) 
	{
		if (Tag[i / DIVIDE] & BitMask[i & WHICH_BIT]) DecCountOn ();// set off
		Tag[i / DIVIDE] &= ~(BitMask[i & WHICH_BIT]);
		Current = i>>BIT_SHIFT;
	}
}
//***********************
int	 TAG_LIST_32:: OnOffTag (const int i) const// simple test
{
	return (Tag[i / DIVIDE] & BitMask[i & WHICH_BIT]);
}
//***********************
int	 TAG_LIST_32:: FlipTag (const int i)
{
	if (OnOffTag (i))
		ClearTag (i);
	else SetTag (i);
	return Tag[i / DIVIDE] & BitMask[i & WHICH_BIT];
}
//***********************
//***********************
int	 TAG_LIST_32:: FindOpenTag ()// uses the Current pointer to start, remeber to 
	//call FindOpenTag in continuous succession if you need this functionality.
{
	int NumBytes = (ListLength>>BIT_SHIFT)+1;
	int StartByte = Current;
	while (Tag[Current] == FULL && Current < NumBytes)// full
		{Current++;}
	if (Current >= NumBytes) 
	{
		Current = 0;// none available
		while (Tag[Current] == FULL && Current < StartByte)// full
			{Current++;}
		if (Current >= StartByte) return -1;// none available
	}

	// go bit by bit to find an available one
	int WhichBit = Current<<BIT_SHIFT;
	for (int i=0; i<DIVIDE; i++, WhichBit++)
	{
		if (GetTag (WhichBit) == 0) return WhichBit;
	}

	return -1;// nothing available
}
//***********************
int	 TAG_LIST_32:: FindFirstSet ()
{
	Last = 0;
	while (Last<ListLength)
	{
		if (GetTag (Last))break;
	}

	if (Last >= ListLength) return -1;
	return Last++;
}
//***********************
int	 TAG_LIST_32:: FindNextSet ()
{
	while (Last<ListLength)
	{
		if (GetTag (Last))break;
	}

	if (Last >= ListLength) return -1;
	return Last++;
}
//***********************
//***********************
int	 TAG_LIST_32:: CountSet () const
{
	return CountOn;
}
//***********************
//***********************
TAG_LIST_32&  TAG_LIST_32:: operator = (const TAG_LIST_32& TL)
{
  if (&TL == this)
    return *this;
	if (TL.ListLength != ListLength)
	{
		if (Tag) delete [] Tag; 
		ListLength = TL.ListLength;
		Tag = new DWORD [(ListLength>>BIT_SHIFT)+1];// 8bits
	}
	Clear ();
	Current = TL.Current;
	CountOn = TL.CountOn;
	memcpy (Tag, TL.Tag, ((ListLength>>BIT_SHIFT)+1)*4);
	return *this;
}
//***********************
bool TAG_LIST_32:: operator == (const TAG_LIST_32& TL) const
{
	if (ListLength != TL.ListLength) return false;
	int t = (ListLength>>BIT_SHIFT)+1;
	for (int i=0; i<t; i++)
	{
		if (Tag[i] == TL.Tag[i]) return false;
	}
	return true;
}
//***********************
bool TAG_LIST_32:: operator != (const TAG_LIST_32& TL) const
{
	return (! (*this == TL));
}
//***********************
int  TAG_LIST_32:: operator [] (const int which) const
{
	if (which<0 || which>=ListLength) return 0;
	int val = GetTag(which);
	return val;
}
//***********************
//***********************
TAG_LIST_32& TAG_LIST_32:: operator <<= (const int num)// num must be less than DIVIDE
{
	if (num<0) return (*this >>= num);

	int n = ListLength;
	
	while (n--)
	{
		if (GetTag (n-num)) SetTag (n);
		else ClearTag (n);
	}
	return *this;
}
//******
TAG_LIST_32 TAG_LIST_32:: operator << (const int num) 
{
	return (TAG_LIST_32 (*this) <<= num);
}
//***********************
TAG_LIST_32& TAG_LIST_32:: operator >>= (const int num)// must be less than DIVIDE
{
	if (num<0) return (*this <<= num);

	int n = ListLength;
	
	for (int i=0; i<n; i++)
	{
		if (GetTag (i+num)) SetTag (i);
		else ClearTag (i);
	}
	return *this;
}
//******
TAG_LIST_32 TAG_LIST_32:: operator >> (const int num) 
{
	return (TAG_LIST_32 (*this) >>= num);
}
//***********************
TAG_LIST_32& TAG_LIST_32:: operator |= (const TAG_LIST_32& TL)
{
	int len = (ListLength>>BIT_SHIFT)+1;
	int testlen = (TL.ListLength>>BIT_SHIFT)+1;
	if (testlen < len) len = testlen;// take the shorter of the 2

	for (int i=0; i<len; i++)
	{
		Tag[i] |= TL.Tag[i];
	}
	return *this;
}
//******
TAG_LIST_32 TAG_LIST_32:: operator | (const TAG_LIST_32& TL)
{
	return (TAG_LIST_32 (*this) |= TL);
}
//***********************
TAG_LIST_32& TAG_LIST_32:: operator &= (const TAG_LIST_32& TL)
{
	int len = (ListLength>>BIT_SHIFT)+1;
	int testlen = (TL.ListLength>>BIT_SHIFT)+1;
	if (testlen < len) len = testlen;// take the shorter of the 2

	for (int i=0; i<len; i++)
	{
		Tag[i] &= TL.Tag[i];
	}
	return *this;
}
//******
TAG_LIST_32 TAG_LIST_32:: operator & (const TAG_LIST_32& TL)
{
	return (TAG_LIST_32 (*this) &= TL);
}
//***********************
TAG_LIST_32& TAG_LIST_32:: operator ^= (const TAG_LIST_32& TL)
{
	int len = (ListLength>>BIT_SHIFT)+1;
	int testlen = (TL.ListLength>>BIT_SHIFT)+1;
	if (testlen < len) len = testlen;// take the shorter of the 2

	for (int i=0; i<len; i++)
	{
		Tag[i] ^= TL.Tag[i];
	}
	return *this;
}
//******
TAG_LIST_32 TAG_LIST_32:: operator ^ (const TAG_LIST_32& TL)
{
	return (TAG_LIST_32 (*this) ^= TL);
}
//***********************
//***********************
TAG_LIST_32 TAG_LIST_32:: operator ~ ()
{
	TAG_LIST_32 t (*this);
	int len = (ListLength>>BIT_SHIFT)+1;

	for (int i=0; i<len; i++)
	{
		t.Tag[i] = ~t.Tag[i];
	}
	return t;
}

//**************************************************
//**************************************************
//**************************************************

TAG_LIST_2D:: TAG_LIST_2D (const int w, const int h) 
{
	Width = w, Height = h;
	int num = Width*Height;
	// init the BitMask list
		BitMask = new BYTE[8];
//	BYTE	bits[8] = { 1,2,4,8,16,32,64,128 };
	for (int i=0; i<8; i++)
		BitMask [i] = 1<<i;

	if (num == 0) ListLength = MAX_LIST;
	else ListLength = num;
	Tag = new BYTE [(ListLength>>BIT_SHIFT)+1];// 8bits
	Clear ();
}
//***********************
TAG_LIST_2D:: TAG_LIST_2D (const TAG_LIST_2D & TL) 
{
	Width = TL.Width, Height = TL.Height;
	// init the BitMask list
		BitMask = new BYTE[8];
	for (int i=0; i<8; i++)
		BitMask [i] = 1<<i;

	ListLength = TL.ListLength;

	Tag = new BYTE [(ListLength>>BIT_SHIFT)+1];// 8bits
	Clear ();
	Current = TL.Current;
	CountOn = TL.CountOn;
	memcpy (Tag, TL.Tag, (ListLength>>BIT_SHIFT)+1);
}
//***********************
TAG_LIST_2D:: ~TAG_LIST_2D () 
{
	delete [] Tag; 
	if (BitMask) delete [] BitMask; 
	BitMask = 0L;
}
//***********************
void  TAG_LIST_2D:: Copy (TAG_LIST_2D& copy) const 
{
	// make sure that we have the space for copying
	if (ListLength > copy.ListLength) 
	{
		if (copy.Tag) delete [] copy.Tag;
		copy.Tag = new BYTE [(ListLength>>BIT_SHIFT)+1];
		copy.ListLength = ListLength;
		//Clear ();
	}
	copy.Width = Width, copy.Height = Height;
	copy.Current = Current;
	copy.CountOn = CountOn;
	memcpy (copy.Tag, Tag, (ListLength>>BIT_SHIFT)+1);
}
//***********************
void TAG_LIST_2D:: Clear ()
{
	memset ( Tag, 0, (ListLength>>BIT_SHIFT) + 1);// num is the number of vertices
	Current = 0, Last = 0, CountOn = 0;
}

//***********************
//***********************
int	 TAG_LIST_2D:: GetTag (const int i) const 
{
	if (i>=0 && i<ListLength) 
		return  (Tag[i / DIVIDE] & BitMask[i & WHICH_BIT])?1:0;

	return 0;
}
//***********************
void TAG_LIST_2D:: SetTag (const int i) 
{
	if (i>=0 && i<ListLength) 
	{
		if (!(Tag[i / DIVIDE] & BitMask[i & WHICH_BIT])) IncCountOn ();// set on
		Tag[i / DIVIDE] |= BitMask[i & WHICH_BIT];
	}
}
//***********************
void TAG_LIST_2D:: ClearTag (const int i)
{
	if (i>=0 && i<ListLength) 
	{
		if (Tag[i / DIVIDE] & BitMask[i & WHICH_BIT]) DecCountOn ();// set off
		Tag[i / DIVIDE] &= ~(BitMask[i & WHICH_BIT]);
		Current = i>>BIT_SHIFT;
	}
}
//***********************
int	 TAG_LIST_2D:: OnOffTag (const int i) const // simple test
{
	return (Tag[i / DIVIDE] & BitMask[i & WHICH_BIT]);
}
//***********************
int	 TAG_LIST_2D:: FlipTag (const int i)
{
	if (OnOffTag (i))
		ClearTag (i);
	else SetTag (i);
	return Tag[i / DIVIDE] & BitMask[i & WHICH_BIT];
}

//***********************
//***********************

int	 TAG_LIST_2D:: FindOpenTag ()// uses the Current pointer to start, remeber to 
	//call FindOpenTag in continuous succession if you need this functionality.
{
	int NumBytes = (ListLength>>BIT_SHIFT)+1;
	int StartByte = Current;
	while (Tag[Current] == FULL && Current < NumBytes)// full
		{Current++;}
	if (Current >= NumBytes) 
	{
		Current = 0;// none available
		while (Tag[Current] == FULL && Current < StartByte)// full
			{Current++;}
		if (Current >= StartByte) return -1;// none available
	}

	// go bit by bit to find an available one
	int WhichBit = Current<<BIT_SHIFT;
	for (int i=0; i<DIVIDE; i++, WhichBit++)
	{
		if (GetTag (WhichBit) == 0) return WhichBit;
	}

	return -1;// nothing available
}

//***********************

void	 TAG_LIST_2D:: FindOpenTag (int& x, int& y)
{
	int which = FindOpenTag ();
	x = which%Width;
	y = which/Width;
}

//***********************
//***********************

int	 TAG_LIST_2D:: FindFirstSet ()
{
	Last = 0;
	while (Last<ListLength)
	{
		if (GetTag (Last))break;
	}

	if (Last >= ListLength) return -1;
	return Last++;
}

//***********************

int	 TAG_LIST_2D:: FindNextSet ()
{
	while (Last<ListLength)
	{
		if (GetTag (Last))break;
	}

	if (Last >= ListLength) return -1;
	return Last++;
}

//***********************

int	 TAG_LIST_2D:: CountSet () const
{
	return CountOn;
}

//***********************
TAG_LIST_2D& TAG_LIST_2D:: operator = (const TAG_LIST_2D& TL)
{
  if (&TL == this)
    return *this;
	if (TL.ListLength != ListLength || Width != TL.Width || Height != TL.Height)
	{
		if (Tag) delete [] Tag; 

		ListLength = TL.ListLength;
		Width = TL.Width, Height = TL.Height;

		Tag = new BYTE [(ListLength>>BIT_SHIFT)+1];// 8bits
	}
	
	Clear ();
	Current = TL.Current;
	CountOn = TL.CountOn;
	memcpy (Tag, TL.Tag, (ListLength>>BIT_SHIFT)+1);
	return *this;
}
//***********************
bool TAG_LIST_2D:: operator == (const TAG_LIST_2D& TL) const
{
	if (ListLength != TL.ListLength) return false;
	int t = (ListLength>>BIT_SHIFT)+1;
	for (int i=0; i<t; i++)
	{
		if (Tag[i] == TL.Tag[i]) return false;
	}
	return true;
}
//***********************
bool TAG_LIST_2D:: operator != (const TAG_LIST_2D& TL) const
{
	return (! (*this == TL));
}
//*********************** // only used for getting a value
int	 TAG_LIST_2D:: operator [] (const int which) const
{
	if (which<0 || which>=ListLength) return 0;
	int val = GetTag(which);
	return val;
}
//***********************
//***********************
//***********************
void TAG_LIST_2D:: SetColumn (const int x)
{
	for (int i=0, loc=x; i<Height; i++, loc+=Width)
	{
		SetTag (loc);
	}
}
//***********************
void TAG_LIST_2D:: ClearColumn (const int x)
{
	for (int i=0, loc=x; i<Height; i++, loc+=Width)
	{
		ClearTag (loc);
	}
}
//***********************
void TAG_LIST_2D:: FlipColumn (const int x)
{
	for (int i=0, loc=x; i<Height; i++, loc+=Width)
	{
		FlipTag (loc);
	}
}
//***********************
void TAG_LIST_2D:: SetRow (const int y)
{
	for (int i=0, loc=y*Width; i<Width; i++, loc++)
	{
		SetTag (loc);
	}
}
//***********************
void TAG_LIST_2D:: ClearRow (const int y)
{
	for (int i=0, loc=y*Width; i<Width; i++, loc++)
	{
		ClearTag (loc);
	}
}
//***********************
void TAG_LIST_2D:: FlipRow (const int y)
{
	for (int i=0, loc=y*Width; i<Width; i++, loc++)
	{
		FlipTag (loc);
	}
}
//***********************
//***********************
void TAG_LIST_2D:: SetAll ()
{
	int num = Width*Height;
	int t = (ListLength>>BIT_SHIFT)+1;
	for (int i=0; i<t; i++)
	{
		Tag[i] = 255;
	}
	CountOn = num;
}
//***********************
void TAG_LIST_2D:: ClearAll ()
{
	Clear ();
}
//***********************
void TAG_LIST_2D:: FlipAll ()
{
	int num = Width*Height;
	for (int i=0; i<num; i++)
	{
		FlipTag (i);
	}
}

//**************************************************
//**************************************************
//**************************************************