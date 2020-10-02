/******************************************************************************
* Filename: traits.h
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
#ifndef __TRAITS_H__
#define __TRAITS_H__

/*
The TRAITS class is responsible for supplying any of the
character traits that you might need to play the game.
For example, THAC0 is an important thing to know.  Spell
immunity might be important.  Whatever it is, you get it
here.  Not from the CHAR class with its experience points
and such.  We will make those things private so that only
we can look at them.  You get your information from us.

On the other hand, things in the CHAR class need to change.
You will change them with CHAR member functions.  Those
member functions will notify us that something has changed 
so that we will know to recompute the proper traits 
according to the designer's directives.
*/

/* The TRAITS class is

    An array of 16-bit numbers that represent
    the result of the computations provided by the
    designer.  These are the things you can ask for.
    Each can be an integer represented as 3 digits
    and a power of 10.  So exact values can be 
    represented up to 999 and approximate values
    to 999000000000 (more than we can fit in the
    integer we return as a result).
    The hex value 0x8000 is reserved to mean that
    the value is outdated and needs to be recomputed.

There is also the code that computes each of these
16-bit numbers.

And there is a dependencies array for each of the 
possible types of data that can be used to compute
the 16-bit numbers.  When something changes, like
a character's ability score, we mark all the traits
that depend on that data as invalid ( 0x8000 ).
*/

enum CharacterModification
{
  CM_AbilityScore = 1,
      // Any ability score (Dexterity eg.)
  CM_BaseClass,
      // Any base class change, addition, deletion
  CM_Race,
      // The race of the character
  CM_End
      // So we know how many changeable things there are.
};

const int NumCharacterThingsThatChange = CM_End+1;

enum CharacterTrait
{
  CT_THAC0 = 1
    // Die roll (1d20) To hit armor class zero
};


class CT_FUNCTION
{
private:
  CArray <ADJUSTMENT, ADJUSTMENT&> m_adjustment;
public:
  CT_FUNCTION(void);
  ~CT_FUNCTION(void);
  CT_FUNCTION& operator =(CT_FUNCTION& src);
  BOOL operator ==(CT_FUNCTION& src);
  void Clear(void);
};


class CT_DEPENDENTS
{
private:
  CArray<WORD, WORD> m_dependents; // Index of trait dependent on this value.
public:
  CT_DEPENDENTS(void);
  ~CT_DEPENDENTS(void);
  CT_DEPENDENTS& operator =(CT_DEPENDENTS& src);
  BOOL operator ==(CT_DEPENDENTS& src);
  void Clear(void);
};

class TRAITS
{
private:
  CArray<short, short> m_traitValues;
//  CArray<CString, CString&> m_traitNames;
  CArray<CT_FUNCTION, CT_FUNCTION&> m_functions; // parallel to m_traitValues
  CArray<CT_DEPENDENTS, CT_DEPENDENTS&> m_dependents;
public:
  TRAITS (void);
  ~TRAITS (void);
  TRAITS& operator =(TRAITS& src);
  BOOL operator ==(TRAITS& src);
  void Clear(void);
  int GetTrait(CharacterTrait index);
};

#endif