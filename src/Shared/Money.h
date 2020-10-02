/******************************************************************************
* Filename: Money.h
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
#ifndef __MONEY_H__
#define __MONEY_H__

//#include "SharedQueue.h"
#include "Items.h"


#define MAX_GEMS MAX_NON_APPRAISED_GEM_JEWELRY
#define MAX_JEWELRY MAX_NON_APPRAISED_GEM_JEWELRY

class COIN_TYPE
{
public:
  enum { MAX_COIN_NAME=10 };
private:
  char Name[MAX_COIN_NAME+1];
  double rate; // zero if IsBase=true  
  BOOL IsBase; // is coin upon which all conversion is based  
public:
  COIN_TYPE();
  void Clear();
  void operator=(const COIN_TYPE &rhs);
  LPCSTR GetName();
  double GetRate();
  BOOL GetBase();
  void SetName(LPCSTR name);
  void SetRate(double r);
  void SetBase(BOOL b);
#if defined(UAFEngine) || defined(UAFEDITOR)
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);
#endif
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  void Import(JReader& jr);
#endif
};

class GEM_CONFIG
{
public:
  enum { MAX_NAME=10 };
  int minValue;
  int maxValue;
  char name[MAX_NAME+1];

  GEM_CONFIG();
  void Clear();
  void operator=(const GEM_CONFIG &rhs);
  int  GetAValue();
  void SetName(LPCSTR name);
  LPCSTR GetName();
#if defined(UAFEngine) || defined(UAFEDITOR)
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);
#endif
#ifdef UAFEDITOR
  void Export(const CString& gemType, JWriter& jw);
  void Import(const CString& gemType, JReader& jr);
#endif

};

class MONEY_DATA_TYPE
{
public:
  enum { MAX_COIN_TYPES=10, DEFAULT_COINS_PER_POUND=10 };
  static int GetIndex(itemClassType type);
  static itemClassType GetItemClass(int index);

private:
  COIN_TYPE Coins[MAX_COIN_TYPES];
  GEM_CONFIG Gems;
  GEM_CONFIG Jewelry;
  int HighestRate;
  int weight;
  itemClassType HRType;
  itemClassType DefType;

public:  
  MONEY_DATA_TYPE();
  void Clear();
  void operator=(const MONEY_DATA_TYPE &rhs);
  void SetUADefaults();
  void ComputeHighestRate();
  double Convert(double amount, itemClassType srcType, itemClassType dstType, double *overflow=NULL);
  double GetConvertRate(itemClassType srcType, itemClassType dstType);
  
  // highest rate
  itemClassType GetBaseType();
  int GetBaseRate();
  double ConvertToBase(double amount, itemClassType srcType, double *overflow=NULL);

  // default currency
  itemClassType GetDefaultType();
  void SetDefaultType(itemClassType type);
  int GetDefaultRate();
  double ConvertToDefault(double amount, itemClassType srcType, double *overflow=NULL);

#if defined(UAFEngine) || defined(UAFEDITOR)
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
#endif
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  void Import(JReader& jr);
#endif
  // index using PlatinumType, ElectrumType, etc - defined in Items.h
  void Set(itemClassType index, COIN_TYPE &data);
  COIN_TYPE &Get(itemClassType index);
  BOOL IsActive(itemClassType index);
  LPCSTR GetName(itemClassType index);
  double GetRate(itemClassType index);
  LPCSTR GetCoinName(int);
  double GetCoinRate(int);
  BOOL GetBase(itemClassType index);
  void SetName(itemClassType index, LPCSTR name);
  void SetRate(itemClassType index, double r);
  int NumCoinTypes() { return MONEY_DATA_TYPE::MAX_COIN_TYPES; }

  void SetGemRate(int min, int max);
  void SetJewelryRate(int min, int max);
  void GetGemRate(int &min, int &max);
  void GetJewelryRate(int &min, int &max);
  int GetGemValue();
  int GetJewelryValue();
  LPCSTR GetGemName() { return Gems.GetName(); }
  LPCSTR GetJewelryName() { return Jewelry.GetName(); }
  int GetWeight();
  void SetWeight(int w);
};


class GEM_TYPE
{
public:
  int  value;
  int id;

  GEM_TYPE();
  void Clear();
  void operator=(const GEM_TYPE &rhs);
  BOOL operator==(const GEM_TYPE &rhs);
  int GetValue();
#if defined(UAFEngine) || defined(UAFEDITOR)
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);
  void Export(JWriter& jw);
  bool Import(JReader& jw);
#endif
};

typedef UnorderedQueue<GEM_TYPE, MAX_GEMS> GEM_LIST;
typedef UnorderedQueue<GEM_TYPE, MAX_JEWELRY> JEWELRY_LIST;

class MONEY_SACK
{
public:

  MONEY_SACK();
  MONEY_SACK(const MONEY_SACK &data);
  void Clear();
  BOOL IsEmpty();
  BOOL IsActive(itemClassType nIndex);
  BOOL HaveEnough(itemClassType nIndex, int amount);
  double Total();
  void Add(itemClassType nIndex, int amount);
  void Add(const MONEY_SACK &src);
  void Subtract(itemClassType nIndex, int amount);
  void Subtract(const MONEY_SACK &src);  
  void Transfer(MONEY_SACK &src);
  LPCSTR Name(itemClassType nIndex);
  double Rate(itemClassType nIndex);
  int& operator[](itemClassType nIndex);
  int& operator[](DWORD nIndex);
  void operator=(const MONEY_SACK &rhs);
#ifdef UAFEDITOR
  BOOL operator==(const MONEY_SACK &rhs) const;
#endif
  // this file is used outside of the engine and editor
#if defined(UAFEngine) || defined(UAFEDITOR)
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  void Export(JWriter& jw);
  bool Import(JReader& jr);
#endif

  int NumCoinTypes() const { return MONEY_DATA_TYPE::MAX_COIN_TYPES; }

  int AddGem();
  int AddGem(const GEM_TYPE &data);
  BOOL AddGems(const GEM_LIST &data);

  int AddJewelry();
  int AddJewelry(const GEM_TYPE &data);
  BOOL AddJewelry(const JEWELRY_LIST &data);

  BOOL RemoveGem(int index);
  void RemoveMultGems(int count);
  BOOL RemoveJewelry(int index);
  void RemoveMultJewelry(int count);
  int AppraiseGem(int index);
  int AppraiseJewelry(int index);
  int NumGems() const;
  int NumJewelry() const;
  int GemValue(int index);
  int JewelryValue(int index);
  int TotalGemValue();
  int TotalJewelryValue();
  
  void AutoUpConvert(void);

  int GetCoinAsBaseAmount(itemClassType type);
  int GetCoinAmount(itemClassType type);

  itemClassType GetBaseType();
  int GetBaseRate();
  double ConvertToBase(double amount, itemClassType srcType, double *overflow=NULL);

  // default currency
  itemClassType GetDefaultType();
  int GetDefaultRate();
  double ConvertToDefault(double amount, itemClassType srcType, double *overflow=NULL);

  // weight of all coins/gems/jewelry in this container
  double GetTotalWeight();


  int Coins[MONEY_DATA_TYPE::MAX_COIN_TYPES];
  //CArray<GEM_TYPE, GEM_TYPE&> Gems;
  //CArray<GEM_TYPE, GEM_TYPE&> Jewelry;

  GEM_LIST Gems;
  JEWELRY_LIST Jewelry;

  int AddGemWithCurrKey(GEM_TYPE &data);
  int AddJewelryWithCurrKey(GEM_TYPE &data);

  int GetNextGemKey();
  int GetNextJewelryKey();
};

#endif