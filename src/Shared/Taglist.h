#ifndef __TAG_LIST_H_
#define __TAG_LIST_H_

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

#include "externs.h"

#define FC

//---------------------------------------------------------------------------

//***************************************************
//***************************************************
//***************************************************

typedef class  TAG_LIST
{

private:

	enum {MAX_LIST = 64, BIT_SHIFT = 3, DIVIDE = 8, WHICH_BIT = 7, FULL = (1<<DIVIDE)-1};

	int				ListLength;// number of items in list
	int				Current;// the current byte we are using
	int				Last;// stores last find position
	int				CountOn;
	LPBYTE			Tag;
	LPBYTE			BitMask;

	//***********************
	void IncCountOn () {CountOn ++;}
	void DecCountOn () {CountOn --;}

public:

	//***********************

	TAG_LIST (int num = MAX_LIST);
	TAG_LIST (TAG_LIST & TL);
	~TAG_LIST () ;
	void 	FC  Copy (TAG_LIST& copy);// copy this into "copy"
	void	FC  Clear ();

	//***********************

	int		FC  GetTag (const int i) const;
	void	FC  SetTag (const int i); 
	void	FC  ClearTag (const int i);
	int		FC  OnOffTag (const int i) const;// simple test
	int		FC  FlipTag (const int i);

	//***********************
	//*********************** simpler interfaces
	int		FC  Get (const int i) const
	{
		return GetTag (i);
	}
	//***********************
	void	FC  Set (const int i) 
	{
		SetTag (i);
	}
	//***********************
	void	FC  Clear (const int i)
	{
		ClearTag (i);
	}
	//***********************
	int		FC  Test (const int i)
	{
		return OnOffTag (i);
	}
	//***********************
	int		FC  Flip (const int i)
	{
		return FlipTag (i);
	}
	//***********************
	//***********************
	int		FC  FindOpenTag ();// uses the Current pointer to start, remeber to 
	//call FindOpenTag in continuous succession if you need this functionality.
	//***********************

	int		FC  FindFirstSet ();// find first tag on
	int		FC  FindNextSet ();// find next tag on

	//***********************

	int		FC  CountSet () const;

	//***********************
	TAG_LIST&	FC	operator =		(const TAG_LIST& TL);
	bool		FC	operator ==		(const TAG_LIST& TL) const;
    bool		FC  operator !=		(const TAG_LIST& TL) const;
	int			FC  operator []		(const int which) const;

	TAG_LIST&	FC  operator <<=	(const int num);// num must be less than DIVIDE
	TAG_LIST	FC  operator <<		(const int num);
	TAG_LIST&	FC  operator >>=	(const int num);// must be less than DIVIDE
    TAG_LIST	FC  operator >>		(const int num);

    TAG_LIST&	FC  operator |=		(const TAG_LIST& TL);
	TAG_LIST	FC  operator |		(const TAG_LIST& TL);
	TAG_LIST&	FC  operator &=		(const TAG_LIST& TL);
	TAG_LIST	FC  operator &		(const TAG_LIST& TL);
    TAG_LIST&	FC  operator ^=		(const TAG_LIST& TL);
	TAG_LIST	FC  operator ^		(const TAG_LIST& TL);

	TAG_LIST	FC  operator ~ ();
	//***********************
}*TAGptr, **TAGlist;



//***************************************************
//***************************************************
//***************************************************
// designed to manage bits like the TAG_LIST but with
// 4 bytes at a time, much faster with large numbers of items
// much larger overhead for small lists of 100 items or less

typedef class  TAG_LIST_32
{

private:

	enum 
	{
		MAX_LIST = 256, 
		BIT_SHIFT = 5, 
		DIVIDE = 32, 
		WHICH_BIT = 31, 
		//FULL = (1<<DIVIDE)-1 
		FULL = 0xFFFFFFFF 
	};

	int				ListLength;// number of items in list
	int				Current;// the current byte we are using
	int				Last;// stores last find position
	int				CountOn;
	DWORD*			Tag;
	DWORD*			BitMask;

	//***********************
	void IncCountOn () {CountOn ++;}
	void DecCountOn () {CountOn --;}

public:

	//***********************

	TAG_LIST_32 (int num = MAX_LIST);
	TAG_LIST_32 (const TAG_LIST_32 & TL);
	~TAG_LIST_32 ();
	void 	FC  Copy (TAG_LIST_32& copy);
	void	FC  Clear ();

	//***********************

	int		FC  GetTag (const int i) const;
	void	FC  SetTag (const int i); 
	void	FC  ClearTag (const int i);
	int		FC  OnOffTag (const int i) const;// simple test
	int		FC  FlipTag (const int i);

	//***********************
	int		FC  Get (int i) 
	{
		return GetTag (i);
	}
	//***********************
	void	FC  Set (int i) 
	{
		SetTag (i);
	}
	//***********************
	void	FC  Clear (int i)
	{
		ClearTag (i);
	}
	//***********************
	int		FC  Test (int i)
	{
		return OnOffTag (i);
	}
	//***********************
	int		FC  Flip (int i)
	{
		return FlipTag (i);
	}

	//***********************

	int		FC  FindOpenTag ();
	
	//***********************

	int		FC  FindFirstSet ();
	int		FC  FindNextSet ();

	//***********************

	int		FC  CountSet () const;

	//***********************
	TAG_LIST_32&	FC  operator =		(const TAG_LIST_32& TL);
	bool			FC  operator ==		(const TAG_LIST_32& TL) const;
    bool			FC  operator !=		(const TAG_LIST_32& TL) const;
	int				FC  operator []		(const int which) const; 
	TAG_LIST_32&	FC  operator <<=	(const int num);// num must be less than DIVIDE
	TAG_LIST_32		FC  operator <<		(const int num); 
	TAG_LIST_32&	FC  operator >>=	(const int num);// must be less than DIVIDE
    TAG_LIST_32		FC  operator>>		(const int num); 
    TAG_LIST_32&	FC  operator |=		(const TAG_LIST_32& TL);
	TAG_LIST_32		FC	operator |		(const TAG_LIST_32& TL);
	TAG_LIST_32&	FC  operator &=		(const TAG_LIST_32& TL);
	TAG_LIST_32		FC  operator &		(const TAG_LIST_32& TL);
    TAG_LIST_32&	FC  operator ^=		(const TAG_LIST_32& TL);
	TAG_LIST_32		FC	operator ^		(const TAG_LIST_32& TL);
	TAG_LIST_32		FC  operator ~	();

	//***********************
}*TAG32ptr, **TAG32list;



//***************************************************
//***************************************************
//***************************************************


typedef class  TAG_LIST_2D
{

private:

	enum {MAX_LIST = 64, BIT_SHIFT = 3, DIVIDE = 8, WHICH_BIT = 7, FULL = (1<<DIVIDE)-1};

	int				ListLength;// number of items in list
	int				Current;// the current byte we are using
	int				Last;// stores last find position
	int				CountOn;
	LPBYTE			Tag;
	LPBYTE			BitMask;
	int				Width, Height;

	//***********************
	void IncCountOn () {CountOn ++;}
	void DecCountOn () {CountOn --;}

public:

	//***********************

	TAG_LIST_2D (const int w, const int h);
	TAG_LIST_2D (const TAG_LIST_2D & TL);
	~TAG_LIST_2D ();
	void 	FC  Copy (TAG_LIST_2D& copy) const;
	void	FC  Clear ();

  int GetWidth() { return Width; }
  int GetHeight() { return Height; }
  int GetByteSize() { return ((ListLength>>BIT_SHIFT) + 1); }
  LPBYTE GetBytes() { return Tag; }

	//***********************
	//***********************

	int		FC  GetTag (const int i) const;
	void	FC  SetTag (const int i);
	void	FC  ClearTag (const int i);
	int		FC  OnOffTag (const int i) const; // simple test
	int		FC  FlipTag (const int i);

	//***********************
	//***********************

	int		FC  FindOpenTag ();// uses the Current pointer to start, remeber to 
		//call FindOpenTag in continuous succession if you need this functionality.
	void	FC	FindOpenTag (int& x, int& y);

	//***********************

	int		FC  FindFirstSet ();
	int		FC  FindNextSet ();

	//***********************

	int		FC  CountSet () const;

	//***********************

	TAG_LIST_2D&FC  operator =		(const TAG_LIST_2D& TL);
	bool		FC  operator ==		(const TAG_LIST_2D& TL) const;
    bool		FC  operator !=		(const TAG_LIST_2D& TL) const;
	int			FC  operator []		(const int which) const;

	//***********************
	// here are the main interface elements that we'll be using
	// most of the operator overloading didn't make much sense
	// so I eliminated them
	//***********************
	int		FC  Get (const int x, const int y) const 
	{
		if (x<0 || x>=Width) return 1;//outside bouondaries is tagged
		if (y<0 || y>=Height) return 1;//outside bouondaries is tagged
		return GetTag (y*Width+x);
	}
	//***********************
	void	FC  Set (const int x, const int y) 
	{
		if (x<0 || x>=Width) return;//outside bouondaries 
		if (y<0 || y>=Height) return;//outside bouondaries 
		SetTag (y*Width+x);
	}
	//***********************
	void	FC  Clear (const int x, const int y)
	{
		if (x<0 || x>=Width) return;//outside bouondaries
		if (y<0 || y>=Height) return;//outside bouondaries 
		ClearTag (y*Width+x);
	}
	//***********************
	int		FC  Test (const int x, const int y) const 
	{
		if (x<0 || x>=Width) return 1;//outside bouondaries
		if (y<0 || y>=Height) return 1;//outside bouondaries 
		return OnOffTag (y*Width+x);
	}
	//***********************
	int		FC  Flip (const int x, const int y)
	{
		if (x<0 || x>=Width) return 1;//outside bouondaries
		if (y<0 || y>=Height) return 1;//outside bouondaries 
		return FlipTag (y*Width+x);
	}
	//***********************
	//***********************

	void	FC	SetColumn (const int x);
	void	FC	ClearColumn (const int x);
	void	FC	FlipColumn (const int x);

	//***********************

	void	FC	SetRow (const int y);
	void	FC	ClearRow (const int y);
	void	FC	FlipRow (const int y);

	//***********************
	//***********************

	void	FC	SetAll ();
	void	FC	ClearAll ();
	void	FC	FlipAll ();

	//***********************
}*TAG2Dptr, **TAG2Dlist;


//***************************************************
//***************************************************
//***************************************************

#undef FC 
#endif