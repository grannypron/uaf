/******************************************************************************
* Filename: Money.cpp
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
#include "stdafx.h"
//#include "..\Shared\Version.h"
#include <math.h>

#ifdef UAFEDITOR
#include "..\UAFWinEd\UAFWinEd.h"
#include "class.h"
#elif UAFEngine
#include "externs.h"
#include "..\UAFWin\Dungeon.h"
#include "class.h"
#endif

#ifdef MONEYTEST
float RollDice( int sides, int numTimes, float bonus);
#endif

#include "GlobalData.h"
#include "Money.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//*****************************************************************************
//    NAME: COIN_TYPE::COIN_TYPE
//
// PURPOSE: 
//
//*****************************************************************************
COIN_TYPE::COIN_TYPE() 
{ 
  Clear(); 
}

//*****************************************************************************
//    NAME: COIN_TYPE::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void COIN_TYPE::Clear() 
{ 
  memset(Name,0,sizeof(Name));
  rate=0.0;
  IsBase=FALSE; 
} 
 
//*****************************************************************************
//    NAME: COIN_TYPE::GetName
//
// PURPOSE: 
//
//*****************************************************************************
LPCSTR COIN_TYPE::GetName() 
{ 
  return Name; 
}

//*****************************************************************************
//    NAME: COIN_TYPE::GetRate
//
// PURPOSE: 
//
//*****************************************************************************
double COIN_TYPE::GetRate() 
{ 
  return rate; 
}

//*****************************************************************************
//    NAME: COIN_TYPE::GetBase
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COIN_TYPE::GetBase() 
{ 
  return IsBase; 
}

//*****************************************************************************
//    NAME: COIN_TYPE::SetName
//
// PURPOSE: 
//
//*****************************************************************************
void COIN_TYPE::SetName(LPCSTR name) 
{ 
  strncpy(Name, name, MAX_COIN_NAME); 
  Name[MAX_COIN_NAME]='\0'; 
}

//*****************************************************************************
//    NAME: COIN_TYPE::SetRate
//
// PURPOSE: 
//
//*****************************************************************************
void COIN_TYPE::SetRate(double r) 
{ 
  if (r >= 0.0) 
    rate=r; 
}

//*****************************************************************************
//    NAME: COIN_TYPE::SetBase
//
// PURPOSE: 
//
//*****************************************************************************
void COIN_TYPE::SetBase(BOOL b) 
{ 
  IsBase=b; 
}

//*****************************************************************************
//    NAME: COIN_TYPE::operator=
//
// PURPOSE: 
//
//*****************************************************************************
void COIN_TYPE::operator=(const COIN_TYPE &rhs)
{
  if (&rhs==this) return;
  rate=rhs.rate;
  IsBase=rhs.IsBase;
  strcpy(Name, rhs.Name);
}

//*****************************************************************************
//    NAME: COIN_TYPE::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
#if defined(UAFEngine) || defined(UAFEDITOR)
void COIN_TYPE::Serialize(CArchive &ar)
{
  if (ar.IsStoring())
  {
    ar << rate;
    ar << IsBase;
    for (int i=0;i<MAX_COIN_NAME;i++)
      ar << Name[i];
  }
  else
  {
    ar >> rate;
    ar >> IsBase;
    for (int i=0;i<MAX_COIN_NAME;i++)
      ar >> Name[i];
    if (strlen(Name) > 0)
      rate = max(rate, 1);
    else
      rate = 0;
  }
}
//*****************************************************************************
//    NAME: COIN_TYPE::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void COIN_TYPE::Serialize(CAR &ar)
{
  if (ar.IsStoring())
  {
    ar << rate;
    ar << IsBase;
    for (int i=0;i<MAX_COIN_NAME;i++)
      ar << Name[i];
  }
  else
  {
    ar >> rate;
    ar >> IsBase;
    for (int i=0;i<MAX_COIN_NAME;i++)
      ar >> Name[i];
    if (strlen(Name) > 0)
      rate = max(rate, 1);
    else
      rate = 0;
  }
}
#endif


#ifdef UAFEDITOR

const char *JKEY_COINTYPES = "coinTypes";
const char *JKEY_RATE = "rate";
const char *JKEY_ISBASE = "isBase";
const char *JKEY_NAME = "name";
void COIN_TYPE::Export(JWriter& jw)
{
  jw.StartList();
  jw.Linefeed(false);
  jw.NameAndValue(JKEY_NAME, Name);
  jw.NameAndValue(JKEY_RATE, rate);
  jw.NameAndValue(JKEY_ISBASE, IsBase);
  jw.EndList();
  jw.Linefeed(true);
}
void COIN_TYPE::Import(JReader& jr)
{
  CString name;
  int n;
  jr.StartList();
  jr.Linefeed(false);
  jr.NameAndValue(JKEY_NAME, name);
  n = name.GetLength();
  if (n > MAX_COIN_NAME) n = MAX_COIN_NAME;
  strcpy(Name, name);
  jr.NameAndValue(JKEY_RATE, rate);
  jr.NameAndValue(JKEY_ISBASE, IsBase);
  jr.EndList();
  jr.Linefeed(true);
}
#endif

//*****************************************************************************
//    NAME: GEM_CONFIG::GEM_CONFIG
//
// PURPOSE: 
//
//*****************************************************************************
GEM_CONFIG::GEM_CONFIG() 
{ 
  Clear(); 
}

//*****************************************************************************
//    NAME: GEM_CONFIG::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void GEM_CONFIG::Clear() 
{ 
  minValue=0;
  maxValue=0; 
  name[0]='\0'; 
}

//*****************************************************************************
//    NAME: GEM_CONFIG::operator=
//
// PURPOSE: 
//
//*****************************************************************************
void GEM_CONFIG::operator=(const GEM_CONFIG &rhs)
{
  if (this==&rhs) return;
  minValue=rhs.minValue;
  maxValue=rhs.maxValue;
  strcpy(name, rhs.name);
}

//*****************************************************************************
//    NAME: GEM_CONFIG::GetName
//
// PURPOSE: 
//
//*****************************************************************************
LPCSTR GEM_CONFIG::GetName() 
{ 
  return name; 
}

//*****************************************************************************
//    NAME: GEM_CONFIG::SetName
//
// PURPOSE: 
//
//*****************************************************************************
void GEM_CONFIG::SetName(LPCSTR n) 
{ 
  strncpy(name, n, MAX_NAME); 
  name[MAX_NAME]='\0'; 
}

//*****************************************************************************
//    NAME: GEM_CONFIG::GetAValue
//
// PURPOSE: 
//
//*****************************************************************************
int GEM_CONFIG::GetAValue() 
{
  int sides = abs(maxValue-minValue);
  if (sides <= 0) return maxValue; // ie: min = max = 5
  int val = (int)RollDice(sides, 1, 0.0);
  val += (minValue-1);
  return val;
}

//*****************************************************************************
//    NAME: GEM_CONFIG::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
#if defined(UAFEngine) || defined(UAFEDITOR)
void GEM_CONFIG::Serialize(CArchive &ar)
{
  if (ar.IsStoring())
  {
    ar << minValue;
    ar << maxValue;
    for (int i=0;i<MAX_NAME;i++)
      ar << name[i];
  }
  else
  {
    ar >> minValue;
    ar >> maxValue;
    for (int i=0;i<MAX_NAME;i++)
      ar >> name[i];
  }
}

//*****************************************************************************
//    NAME: GEM_CONFIG::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void GEM_CONFIG::Serialize(CAR &ar)
{
  if (ar.IsStoring())
  {
    ar << minValue;
    ar << maxValue;
    for (int i=0;i<MAX_NAME;i++)
      ar << name[i];
  }
  else
  {
    ar >> minValue;
    ar >> maxValue;
    for (int i=0;i<MAX_NAME;i++)
      ar >> name[i];
  }
}
#endif


#ifdef UAFEDITOR

const char *JKEY_MINVALUE = "minValue";
const char *JKEY_MAXVALUE = "maxValue";
extern const char *JKEY_NAME;
const char *JKEY_GEMCONFIG = "gemConfig";
extern const char *JKEY_NAMES = "names";
void GEM_CONFIG::Export(const CString& gemType, JWriter& jw)
{
  jw.StartList(gemType);
  jw.NameAndValue(JKEY_MINVALUE, minValue);
  jw.NameAndValue(JKEY_MAXVALUE, maxValue);
  {
    jw.NameAndValue(JKEY_NAMES, name);
  };
  jw.EndList();
}
void GEM_CONFIG::Import(const CString& gemType, JReader& jr)
{
  CString temp;
  int n;
  jr.StartList(gemType);
  jr.NameAndValue(JKEY_MINVALUE, minValue);
  jr.NameAndValue(JKEY_MAXVALUE, maxValue);
  {
    jr.NameAndValue(JKEY_NAMES, temp);
    n = temp.GetLength();
    if (n > MAX_NAME) n = MAX_NAME;
    strcpy(name, temp);
  };
  jr.EndList();
}
#endif

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetIndex
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_DATA_TYPE::GetIndex(itemClassType type) 
{
  switch (type)
  {
  case PlatinumType: return 0;
  case ElectrumType: return 1;
  case GoldType: return 2;
  case SilverType: return 3;
  case CopperType: return 4;
  case CoinType6: return 5;
  case CoinType7: return 6;
  case CoinType8: return 7;
  case CoinType9: return 8;
  case CoinType10: return 9;
  }
  die(0xab526);
  return 0;
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetItemClass
//
// PURPOSE: 
//
//*****************************************************************************
itemClassType MONEY_DATA_TYPE::GetItemClass(int index) 
{
  switch (index)
  {
  case 0: return PlatinumType;
  case 1: return ElectrumType;
  case 2: return GoldType;
  case 3: return SilverType;
  case 4: return CopperType;
  case 5: return CoinType6;
  case 6: return CoinType7;
  case 7: return CoinType8;
  case 8: return CoinType9;
  case 9: return CoinType10;
  }
  die(0xab527);
  return PlatinumType;
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::MONEY_DATA_TYPE
//
// PURPOSE: 
//
//*****************************************************************************
MONEY_DATA_TYPE::MONEY_DATA_TYPE() 
{ 
  Clear(); 
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_DATA_TYPE::Clear() 
{ 
  for (int i=0;i<MAX_COIN_TYPES;i++) 
    Coins[i].Clear();
  HighestRate=0;
  weight=10;
  HRType=BogusItemType;
  DefType=PlatinumType;
  Gems.Clear(); 
  Gems.minValue=1;
  Gems.maxValue=100;
  Gems.SetName("Gem");
  Jewelry.Clear();
  Jewelry.minValue=10;
  Jewelry.maxValue=1000;
  Jewelry.SetName("Jewelry");
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::operator=
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_DATA_TYPE::operator=(const MONEY_DATA_TYPE &rhs)
{
  if (&rhs==this) return;
  Clear();
  weight=rhs.weight;
  HighestRate=rhs.HighestRate;
  HRType=rhs.HRType;
  DefType=rhs.DefType;
  Gems=rhs.Gems;
  Jewelry=rhs.Jewelry;
  for (int i=0;i<MAX_COIN_TYPES;i++)
    Coins[i] = rhs.Coins[i];
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::SetUADefaults
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_DATA_TYPE::SetUADefaults()
{
  // 1000 copper = 100 silver = 10 electrum = 5 gold = 1 platinum
  //
  // 1 pp = 5 gp
  // 1 pp = 10 ep
  // 1 pp = 100 sp
  // 1 pp = 1000 cp
  //
  // 1 gp = 2 ep
  // 1 gp = 20 sp
  // 1 gp = 200 cp
  //
  // 1 ep = 10 sp
  // 1 ep = 100 cp
  //
  // 1 sp = 10 cp
  //
  Clear();
  COIN_TYPE data;
  weight=10;
  data.SetName("Platinum");
  data.SetRate(1.0);
  data.SetBase(TRUE);
  Coins[GetIndex(PlatinumType)] = data;

  data.SetName("Gold");
  data.SetRate(5.0);
  Coins[GetIndex(GoldType)] = data;

  data.SetName("Electrum");
  data.SetRate(10.0);
  Coins[GetIndex(ElectrumType)] = data;

  data.SetName("Silver");
  data.SetRate(100.0);
  Coins[GetIndex(SilverType)] = data;

  data.SetName("Copper");
  data.SetRate(1000.0);
  Coins[GetIndex(CopperType)] = data;

  Gems.Clear(); 
  Gems.minValue=1;
  Gems.maxValue=100;
  Gems.SetName("Gem");
  Jewelry.Clear();
  Jewelry.minValue=10;
  Jewelry.maxValue=1000;
  Jewelry.SetName("Jewelry");

  ComputeHighestRate();
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::ComputeHighestRate
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_DATA_TYPE::ComputeHighestRate()
{
  HighestRate=0;
  HRType=BogusItemType;
  for (int i=0;i<MAX_COIN_TYPES;i++) 
  {
    if (Coins[i].GetRate() > HighestRate)
    {
      HighestRate = (int)Coins[i].GetRate();
      HRType = GetItemClass(i);
    }
  }
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetWeight
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_DATA_TYPE::GetWeight() 
{
  return weight; 
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::SetWeight
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_DATA_TYPE::SetWeight(int w) 
{ 
  weight = max(0, w); 
}
//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::SetGemRate
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_DATA_TYPE::SetGemRate(int min, int max) 
{ 
  Gems.minValue=min; 
  Gems.maxValue=max; 
}
//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::SetJewelryRate
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_DATA_TYPE::SetJewelryRate(int min, int max) 
{ 
  Jewelry.minValue=min; 
  Jewelry.maxValue=max; 
}
//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetGemRate
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_DATA_TYPE::GetGemRate(int &min, int &max) 
{ 
  min=Gems.minValue; 
  max=Gems.maxValue; 
}
//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetJewelryRate
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_DATA_TYPE::GetJewelryRate(int &min, int &max) 
{ 
  min=Jewelry.minValue; 
  max=Jewelry.maxValue; 
}
//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetGemValue
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_DATA_TYPE::GetGemValue() 
{ 
  return Gems.GetAValue(); 
}
//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetJewelryValue
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_DATA_TYPE::GetJewelryValue() 
{
  return Jewelry.GetAValue(); 
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetDefaultType
//
// PURPOSE: 
//
//*****************************************************************************
itemClassType MONEY_DATA_TYPE::GetDefaultType() 
{ 
  return DefType; 
}
//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::SetDefaultType
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_DATA_TYPE::SetDefaultType(itemClassType type) 
{ 
  DefType=type;
}
//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetDefaultRate
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_DATA_TYPE::GetDefaultRate() 
{ 
  return (int)GetRate(DefType); 
}
//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::ConvertToDefault
//
// PURPOSE: 
//
//*****************************************************************************
double MONEY_DATA_TYPE::ConvertToDefault(double amount, itemClassType srcType, double *overflow)
{
  return Convert(amount, srcType, DefType, overflow);
}
// Highest Rate Type, coin with the highest conversion rate
//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetBaseType
//
// PURPOSE: 
//
//*****************************************************************************
itemClassType MONEY_DATA_TYPE::GetBaseType() 
{ 
  return HRType; 
}
//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetBaseRate
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_DATA_TYPE::GetBaseRate() 
{ 
  return HighestRate; 
}
//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::ConvertToBase
//
// PURPOSE: 
//
//*****************************************************************************
double MONEY_DATA_TYPE::ConvertToBase(double amount, itemClassType srcType, double *overflow)
{
  return Convert(amount, srcType, HRType, overflow);
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::Convert
//
// PURPOSE: 
//
//*****************************************************************************
double MONEY_DATA_TYPE::Convert(double amount, itemClassType srcType, itemClassType dstType, double *overflow)
{
  if (overflow!=NULL) *overflow=0.0;
  if (amount == 0) return 0.0;
  if (srcType == dstType) return amount;
  double srate = Coins[GetIndex(srcType)].GetRate();
  double drate = Coins[GetIndex(dstType)].GetRate();
  if (srate == 0.0) return 0.0;
  if (drate == 0.0) return 0.0;
  if (drate == srate) return amount;
  double total;
  double extracoins;
  double dv;
  if (drate < srate)
  {
    dv = srate/drate;
    dv = max(1.0, dv);
    total = amount/dv;
  }
  else // (drate > srate)
  {
    dv = drate/srate;
    dv = max(1.0, dv);
    total = amount*dv;
  }
  extracoins = modf(total, &total);
  extracoins *= dv;
  //extracoins = ceil(extracoins);
  extracoins = floor(extracoins+0.5);
  if (overflow!=NULL) *overflow=extracoins;
  return total;
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetConvertRate
//
// PURPOSE: 
//
//*****************************************************************************
double MONEY_DATA_TYPE::GetConvertRate(itemClassType srcType, itemClassType dstType)
{
  if (srcType == dstType) return Coins[GetIndex(srcType)].GetRate();
  double srate = Coins[GetIndex(srcType)].GetRate();
  double drate = Coins[GetIndex(dstType)].GetRate();
  if (drate == srate) return drate;
  return (srate/drate);
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::Set
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_DATA_TYPE::Set(itemClassType index, COIN_TYPE &data) 
{
  int i=GetIndex(index); 
  ASSERT(i>=0);  
  Coins[i]=data; 
}
//*****************************************************************************
//    NAME: &MONEY_DATA_TYPE::Get
//
// PURPOSE: 
//
//*****************************************************************************
COIN_TYPE &MONEY_DATA_TYPE::Get(itemClassType index) 
{ 
  int i=GetIndex(index); 
  ASSERT(i>=0); 
  return Coins[i]; 
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::IsActive
//
// PURPOSE: 
//
//*****************************************************************************
BOOL MONEY_DATA_TYPE::IsActive(itemClassType index) 
{ 
  int i=GetIndex(index); 
  ASSERT(i>=0); 
  return ((Coins[i].GetRate()>0.0) && (strlen(Coins[i].GetName()) > 0)); 
}
//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetName
//
// PURPOSE: 
//
//*****************************************************************************
LPCSTR MONEY_DATA_TYPE::GetName(itemClassType index) 
{ 
  if (index == GemType) return Gems.GetName();
  else if (index == JewelryType) return Jewelry.GetName();
  else { int i=GetIndex(index); ASSERT(i>=0); return Coins[i].GetName(); }
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetRate
//
// PURPOSE: 
//
//*****************************************************************************
double MONEY_DATA_TYPE::GetRate(itemClassType index) 
{
  int i=GetIndex(index); 
  ASSERT(i>=0);  
  return Coins[i].GetRate(); 
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetCoinName
//
// PURPOSE: 
//
//*****************************************************************************
LPCSTR MONEY_DATA_TYPE::GetCoinName(int i)
{
  return (i<0)||(i>=MAX_COIN_TYPES)?"":Coins[i].GetName();
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetCoinRate
//
// PURPOSE: 
//
//*****************************************************************************
double MONEY_DATA_TYPE::GetCoinRate(int i)
{
  return (i<0)||(i>=MAX_COIN_TYPES)?0:Coins[i].GetRate();
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::GetBase
//
// PURPOSE: 
//
//*****************************************************************************
BOOL MONEY_DATA_TYPE::GetBase(itemClassType index) 
{ 
  int i=GetIndex(index); 
  ASSERT(i>=0); 
  return Coins[i].GetBase(); 
} 
//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::SetName
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_DATA_TYPE::SetName(itemClassType index, LPCSTR name) 
{ 
  if (index == GemType) Gems.SetName(name);
  else if (index == JewelryType) Jewelry.SetName(name);
  else { int i=GetIndex(index); ASSERT(i>=0); Coins[i].SetName(name); }
}
//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::SetRate
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_DATA_TYPE::SetRate(itemClassType index, double r) 
{ 
  int i=GetIndex(index); 
  ASSERT(i>=0); 
  Coins[i].SetRate(r); 
  ComputeHighestRate(); 
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
#if defined(UAFEngine) || defined(UAFEDITOR)
void MONEY_DATA_TYPE::Serialize(CArchive &ar, double version)
{
  if (ar.IsStoring())
  {
    ar << weight;
    ar << HighestRate;
    ar << (int)HRType;
    ar << (int)DefType;
    Gems.Serialize(ar);
    Jewelry.Serialize(ar);    
  }
  else
  {
    if (version >= _VERSION_0662_)
    {
      ar >> weight;
    }
    ar >> HighestRate;
    if (version >= _VERSION_0661_)
    {
      int temp;
      ar >> temp;
      HRType = (itemClassType)temp;
      ar >> temp;
      DefType = (itemClassType)temp;
      Gems.Serialize(ar);
      Jewelry.Serialize(ar);
    }
  }
  for (int i=0;i<MAX_COIN_TYPES;i++)
    Coins[i].Serialize(ar);
}

//*****************************************************************************
//    NAME: MONEY_DATA_TYPE::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_DATA_TYPE::Serialize(CAR &ar, double version)
{
  if (ar.IsStoring())
  {
    ar << weight;
    ar << HighestRate;
    ar << (int)HRType;
    ar << (int)DefType;
    Gems.Serialize(ar);
    Jewelry.Serialize(ar);
  }
  else
  {
    if (version >= _VERSION_0662_)
    {
      ar >> weight;
    }
    ar >> HighestRate;
    if (version >= _VERSION_0661_)
    {
      int temp;
      ar >> temp;
      HRType = (itemClassType)temp;
      ar >> temp;
      DefType = (itemClassType)temp;
      Gems.Serialize(ar);
      Jewelry.Serialize(ar);
    }
  }
  for (int i=0;i<MAX_COIN_TYPES;i++)
    Coins[i].Serialize(ar);
}
#endif

#ifdef UAFEDITOR
const char *JKEY_WEIGHT = "weight";
const char *JKEY_HIGHESTRATE = "highestRate";
const char *JKEY_HRTYPE = "hrType";
const char *JKEY_DEFTYPE = "defType";
const char *JKEY_COINS = "coins";
const char *JKEY_GEMS = "gems";
const char *JKEY_JEWELS = "jewels";

void MONEY_DATA_TYPE::Export(JWriter& jw)
{
  jw.NameAndValue(JKEY_WEIGHT, weight);
  jw.NameAndValue(JKEY_HIGHESTRATE, HighestRate);
  jw.NameAndEnum(JKEY_HRTYPE, HRType);
  jw.NameAndEnum(JKEY_DEFTYPE, DefType);
  Gems.Export(JKEY_GEMS, jw);
  Jewelry.Export(JKEY_JEWELS, jw);
  jw.StartArray(JKEY_COINS);
  for (int i=0;i<MAX_COIN_TYPES;i++)
  {
    jw.NextEntry();
    Coins[i].Export(jw);
  };
  jw.EndArray();
}
void MONEY_DATA_TYPE::Import(JReader& jr)
{
  jr.NameAndValue(JKEY_WEIGHT, weight);
  jr.NameAndValue(JKEY_HIGHESTRATE, HighestRate);
  jr.NameAndEnum(JKEY_HRTYPE, HRType);
  jr.NameAndEnum(JKEY_DEFTYPE, DefType);
  Gems.Import(JKEY_GEMS, jr);
  Jewelry.Import(JKEY_JEWELS, jr);
  jr.StartArray(JKEY_COINS);
  for (int i=0;i<MAX_COIN_TYPES;i++)
  {
    jr.NextEntry();
    Coins[i].Import(jr);
  };
  jr.EndArray();
}
#endif


//*****************************************************************************
//    NAME: GEM_TYPE::GEM_TYPE
//
// PURPOSE: 
//
//*****************************************************************************
GEM_TYPE::GEM_TYPE() 
{ 
  Clear(); 
}
//*****************************************************************************
//    NAME: GEM_TYPE::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void GEM_TYPE::Clear() 
{ 
  value=0; 
  id=-1; 
}
//*****************************************************************************
//    NAME: GEM_TYPE::operator=
//
// PURPOSE: 
//
//*****************************************************************************
void GEM_TYPE::operator=(const GEM_TYPE &rhs)
{
  if (this==&rhs) return;
  value=rhs.value;
  id=rhs.id;
}
//*****************************************************************************
//    NAME: GEM_TYPE::operator==
//
// PURPOSE: 
//
//*****************************************************************************
BOOL GEM_TYPE::operator==(const GEM_TYPE &rhs)
{
  if (value != rhs.value) return FALSE;
  if (id != rhs.id) return FALSE;
  return TRUE;
}

//*****************************************************************************
//    NAME: GEM_TYPE::GetValue
//
// PURPOSE: 
//
//*****************************************************************************
int GEM_TYPE::GetValue() 
{ 
  return value; 
}

//*****************************************************************************
//    NAME: GEM_TYPE::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
#if defined(UAFEngine) || defined(UAFEDITOR)
void GEM_TYPE::Serialize(CArchive &ar)
{
  if (ar.IsStoring())
  {
    ar << id;
    ar << value;
  }
  else
  {
    ar >> id;
    ar >> value;
  }
}
//*****************************************************************************
//    NAME: GEM_TYPE::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void GEM_TYPE::Serialize(CAR &ar)
{
  if (ar.IsStoring())
  {
    ar << id;
    ar << value;
  }
  else
  {
    ar >> id;
    ar >> value;
  }
}
#endif

#ifdef UAFEDITOR
extern const char *JKEY_ID;
const char *JKEY_VALUE = "value";
const char *JKEY_GEM = "gem";
const char *JKEY_JEWEL = "jewel";

void GEM_TYPE::Export(JWriter& jw)
{
  jw.StartList();
  jw.Linefeed(false);
  jw.NameAndValue(JKEY_ID, id);
  jw.NameAndValue(JKEY_VALUE, value);
  jw.EndList();
  jw.Linefeed(true);
}
bool GEM_TYPE::Import(JReader& jr)
{
  jr.Optional();
  if (!jr.NextEntry())
  {
    return false;
  };
  jr.StartList();
  jr.Linefeed(false);
  jr.NameAndValue(JKEY_ID, id);
  jr.NameAndValue(JKEY_VALUE, value);
  jr.EndList();
  jr.Linefeed(true);
  return true;
}
#endif

//*****************************************************************************
//    NAME: MONEY_SACK::MONEY_SACK
//
// PURPOSE: 
//
//*****************************************************************************
MONEY_SACK::MONEY_SACK() 
{ 
  Clear(); 
}
//*****************************************************************************
//    NAME: MONEY_SACK::MONEY_SACK
//
// PURPOSE: 
//
//*****************************************************************************
MONEY_SACK::MONEY_SACK(const MONEY_SACK &data) 
{ 
  *this = data; 
}
//*****************************************************************************
//    NAME: MONEY_SACK::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_SACK::Clear() 
{ 
  memset(Coins,0,sizeof(Coins));
  Jewelry.RemoveAll();
  Gems.RemoveAll(); 
}

//*****************************************************************************
//    NAME: MONEY_SACK::operator[]
//
// PURPOSE: 
//
//*****************************************************************************
int& MONEY_SACK::operator[](itemClassType nIndex) 
{ 
  int i=MONEY_DATA_TYPE::GetIndex(nIndex); 
  ASSERT(i>=0); 
  return Coins[i]; 
}
//*****************************************************************************
//    NAME: MONEY_SACK::operator[]
//
// PURPOSE: 
//
//*****************************************************************************
int& MONEY_SACK::operator[](DWORD nIndex) 
{ 
  ASSERT(((int)nIndex>=0)&&((int)nIndex<NumCoinTypes())); 

  return Coins[nIndex]; 
}

//*****************************************************************************
//    NAME: MONEY_SACK::operator=
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_SACK::operator=(const MONEY_SACK &rhs)
{
  if (&rhs==this) return;
  Clear();
  //Gems.SetSize(rhs.Gems.GetSize());
  //for (int g=0;g<NumGems();g++)
  //  Gems[g]=rhs.Gems[g];

  //Jewelry.SetSize(rhs.Jewelry.GetSize());
  //for (int j=0;j<NumJewelry();j++)
  //  Jewelry[j]=rhs.Jewelry[j];
  
  POSITION pos = const_cast<MONEY_SACK&>(rhs).Gems.GetHeadPosition();
  while (pos != NULL)
    AddGemWithCurrKey(const_cast<MONEY_SACK&>(rhs).Gems.GetNext(pos));

  pos = const_cast<MONEY_SACK&>(rhs).Jewelry.GetHeadPosition();
  while (pos != NULL)  
    AddJewelryWithCurrKey(const_cast<MONEY_SACK&>(rhs).Jewelry.GetNext(pos));

  for (int i=0;i<NumCoinTypes();i++)
    Coins[i] = rhs.Coins[i];
}
#ifdef UAFEDITOR
//*****************************************************************************
//    NAME: MONEY_SACK::operator==
//
// PURPOSE: 
//
//*****************************************************************************
BOOL MONEY_SACK::operator==(const MONEY_SACK &rhs) const
{
  for (int i=0;i<NumCoinTypes();i++)
  {
    if (Coins[i] != rhs.Coins[i]) 
    {
      return FALSE;
    };
  };

  if (NumGems() != rhs.NumGems()) return FALSE;
  if (NumJewelry() != rhs.NumJewelry()) return FALSE;
  
  //for (int g=0;g<NumGems();g++)
  //  if (!(Gems[g] == rhs.Gems[g])) return FALSE;
  POSITION p1=Gems.GetHeadPosition();
  POSITION p2=const_cast<MONEY_SACK&>(rhs).Gems.GetHeadPosition();
  while (p1!=NULL)
  {
    if (p2==NULL) return false;
    if (!(const_cast<MONEY_SACK*>(this)->Gems.GetNext(p1) 
            == const_cast<MONEY_SACK&>(rhs).Gems.GetNext(p2))) return false;
  }
  if (p2!=NULL) return false;

  //for (int j=0;g<NumJewelry();j++)
  //  if (!(Jewelry[j] == rhs.Jewelry[j])) return FALSE;

  p1=Jewelry.GetHeadPosition();
  p2=rhs.Jewelry.GetHeadPosition();
  while (p1!=NULL)
  {
    if (p2==NULL) return false;
    if (!(const_cast<MONEY_SACK*>(this)->Jewelry.GetNext(p1)==const_cast<MONEY_SACK&>(rhs).Jewelry.GetNext(p2))) return false;
  }
  if (p2!=NULL) return false;

  return TRUE;
}
#endif
//*****************************************************************************
//    NAME: MONEY_SACK::GetCoinAsBaseAmount
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::GetCoinAsBaseAmount(itemClassType type)
{
  if (!IsActive(type)) return 0;
  int i = MONEY_DATA_TYPE::GetIndex(type);
  return (globalData.moneyData.ConvertToBase(Coins[i], type));
}

//*****************************************************************************
//    NAME: MONEY_SACK::GetCoinAmount
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::GetCoinAmount(itemClassType type)
{
  if (!IsActive(type)) return 0;
  int i = MONEY_DATA_TYPE::GetIndex(type);
  return (Coins[i]);
}

//*****************************************************************************
//    NAME: MONEY_SACK::IsActive
//
// PURPOSE: 
//
//*****************************************************************************
BOOL MONEY_SACK::IsActive(itemClassType nIndex)
{
  return (globalData.moneyData.IsActive(nIndex));
}

//*****************************************************************************
//    NAME: MONEY_SACK::IsEmpty
//
// PURPOSE: 
//
//*****************************************************************************
BOOL MONEY_SACK::IsEmpty()
{
  for (int i=0;i<NumCoinTypes();i++)
    if (Coins[i] > 0) return FALSE;
  return TRUE;
}

//*****************************************************************************
//    NAME: MONEY_SACK::Total
//
// PURPOSE: 
//
//*****************************************************************************
double MONEY_SACK::Total()
{
  double total=0;
  for (int i=0;i<NumCoinTypes();i++)
  {
    if (Coins[i] > 0)
      total += globalData.moneyData.ConvertToBase(Coins[i], MONEY_DATA_TYPE::GetItemClass(i));
  }
  return total;
}

//*****************************************************************************
//    NAME: MONEY_SACK::AutoUpConvert
//
// PURPOSE: 
//
// convert the existing funds to the highest value coin type. ie: convert
// 1000 copper to 1 platinum
//
//*****************************************************************************
struct AUTOUPCONVERT_DATA
{
  double rate;
  int    index;
};

int CompareAutoUpConvertData( const void *arg1, const void *arg2 )
{
  /*
    to sort in decreasing order, must return
  < 0 elem1 less than elem2 
  = 0 elem1 equivalent to elem2 
  > 0 elem1 greater than elem2 
  */
  ASSERT( (arg1!=NULL) && (arg2!=NULL) );
  if ((arg1==NULL)||(arg2==NULL)) return 0;
  AUTOUPCONVERT_DATA *pElem1 = (AUTOUPCONVERT_DATA*)arg1;
  AUTOUPCONVERT_DATA *pElem2 = (AUTOUPCONVERT_DATA*)arg2;

  if (pElem1->rate < pElem2->rate) return -1;
  if (pElem1->rate > pElem2->rate) return  1;
  return 0;
}

void MONEY_SACK::AutoUpConvert(void)
{
  double total = 0.0;
  double leftover = 0.0;
  int i;
  int count = 0;
  
  AUTOUPCONVERT_DATA ConvertOrder[MONEY_DATA_TYPE::MAX_COIN_TYPES];
  memset(ConvertOrder, 0, sizeof(ConvertOrder));

  for (i=0; i<NumCoinTypes(); i++)
  {
    double rate = globalData.moneyData.GetCoinRate(i);

    // only include the non-zero coin rates
    if (rate <= 0.0)
      break;

    ConvertOrder[i].rate = rate;
    ConvertOrder[i].index = i;
  }
  
  count = i;

  qsort( ConvertOrder, 
         count, 
         sizeof(AUTOUPCONVERT_DATA), 
         CompareAutoUpConvertData);

  for (i=count-1; i>=1; i--)
  {
    int coinindex1 = ConvertOrder[i].index;
    int coinindex2 = ConvertOrder[i-1].index;
    total = globalData.moneyData.Convert(Coins[coinindex1], 
                                         MONEY_DATA_TYPE::GetItemClass(coinindex1), // src
                                         MONEY_DATA_TYPE::GetItemClass(coinindex2), // dst
                                         &leftover);
    Coins[coinindex2] += total;
    Coins[coinindex1] = leftover;
  }
}

//*****************************************************************************
//    NAME: MONEY_SACK::HaveEnough
//
// PURPOSE: 
//
//*****************************************************************************
BOOL MONEY_SACK::HaveEnough(itemClassType nIndex, int amount)
{
  return (globalData.moneyData.ConvertToBase(amount, nIndex) <= Total());
}

//*****************************************************************************
//    NAME: MONEY_SACK::Add
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_SACK::Add(itemClassType nIndex, int amount)
{
  if (!IsActive(nIndex)) return;
  int i = MONEY_DATA_TYPE::GetIndex(nIndex);
  Coins[i] += amount;
}

//*****************************************************************************
//    NAME: MONEY_SACK::Add
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_SACK::Add(const MONEY_SACK &src)
{
  for (int i=0;i<NumCoinTypes();i++)
    Coins[i] += src.Coins[i];

  AddGems(src.Gems);
  AddJewelry(src.Jewelry);
}

//*****************************************************************************
//    NAME: MONEY_SACK::Subtract
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_SACK::Subtract(itemClassType nIndex, int amount)
{
  if (!IsActive(nIndex)) return;
  if (!HaveEnough(nIndex, amount)) return;

  int i = MONEY_DATA_TYPE::GetIndex(nIndex);
  if (amount <= Coins[i])
  {
    Coins[i] -= amount;
    Coins[i] = max(Coins[i],0);
    return;
  }

  // sub from index coin first
  int diff = amount-Coins[i];
  Coins[i]=0;
  bool done=false;
  double extracoins=0.0;

  // take remainder from other coin types
  itemClassType srcType = MONEY_DATA_TYPE::GetItemClass(i);  
  double leftover=0.0;

  for (int c=NumCoinTypes()-1; c>=0 && !done; c--)
  {
    if (Coins[c] > 0)
    {
      extracoins=0.0;
      // convert this coin type into the src coin type
      int avail = globalData.moneyData.Convert(Coins[c], MONEY_DATA_TYPE::GetItemClass(c), srcType, &extracoins);

      if (avail > 0)
      {
        int temp = avail - diff;
        if (temp >= 0)
        {
          done=true;
          avail -= diff;
          diff = 0;
          Coins[c] = floor(extracoins+0.5);
          extracoins=0.0;
          // 
          Coins[c] += globalData.moneyData.Convert(avail, srcType, MONEY_DATA_TYPE::GetItemClass(c), &extracoins);
          /*Coins[i]*/leftover = floor(extracoins+0.5);
        }
        else
        {
          Coins[c] = floor(extracoins+0.5);
          diff = abs(temp);
        }
      }
    }
  }

  if (!done)
  {
    double total = Total();
    diff = globalData.moneyData.ConvertToBase(diff, srcType);
    total -= diff;
    for (i=0;i<NumCoinTypes(); i++)
      Coins[i]=0;
    if (total > 0)
    {
      srcType = globalData.moneyData.GetBaseType();
      Coins[MONEY_DATA_TYPE::GetIndex(srcType)] = total;
    }
  }
  else if (leftover > 0.0)
  {
    done=false;
    // convert the extra coins to higher coin types if possible
    for (int c=0; c<NumCoinTypes() && !done; c++)
    {
      // convert leftover coin type into the dest coin type
      int result = globalData.moneyData.Convert(leftover, srcType, MONEY_DATA_TYPE::GetItemClass(c), &extracoins);
      if (result > 0.0)
      {
        leftover = extracoins;
        Coins[c]=result;
      }
      if (leftover <= 0.0)
        done=true;
    }

    if (!done)
    {
      Coins[i] = leftover;
    }
  }
}

//*****************************************************************************
//    NAME: MONEY_SACK::Subtract
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_SACK::Subtract(const MONEY_SACK &src)
{
  for (int i=0;i<NumCoinTypes();i++)
    Subtract(MONEY_DATA_TYPE::GetItemClass(i), src.Coins[i]);
}

//*****************************************************************************
//    NAME: MONEY_SACK::Transfer
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_SACK::Transfer(MONEY_SACK &src)
{
  Add(src);
  src.Clear();
}

//*****************************************************************************
//    NAME: MONEY_SACK::Name
//
// PURPOSE: 
//
//*****************************************************************************
LPCSTR MONEY_SACK::Name(itemClassType nIndex)
{
  return globalData.moneyData.GetName(nIndex);
}

//*****************************************************************************
//    NAME: MONEY_SACK::Rate
//
// PURPOSE: 
//
//*****************************************************************************
double MONEY_SACK::Rate(itemClassType nIndex)
{
  return globalData.moneyData.GetRate(nIndex);
}


//*****************************************************************************
//    NAME: MONEY_SACK::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
#if defined(UAFEngine) || defined(UAFEDITOR)
void MONEY_SACK::Serialize(CArchive &ar, double version)
{
  int i;
  for (i=0;i<NumCoinTypes();i++)
  {
    if (ar.IsStoring())
      ar << Coins[i];
    else
      ar >> Coins[i];
  }
  //Gems.Serialize(ar);
  //Jewelry.Serialize(ar);
  
  POSITION pos;
  if (ar.IsStoring())
  {
    ar << Gems.GetCount();
    pos = Gems.GetHeadPosition();
    while (pos != NULL)
      Gems.GetNext(pos).Serialize(ar);

    ar << Jewelry.GetCount();
    pos = Jewelry.GetHeadPosition();
    while (pos != NULL)
      Jewelry.GetNext(pos).Serialize(ar);
  }
  else
  {
    if (version < _VERSION_0911_)
    {
      // old style for loading gems and jewelry
      CArray<GEM_TYPE, GEM_TYPE&> tempGEMS;
      tempGEMS.Serialize(ar); 

      for (i=0;i<tempGEMS.GetSize(); i++)
        AddGemWithCurrKey(tempGEMS[i]);

      // now load the jewelry
      tempGEMS.RemoveAll();
      tempGEMS.Serialize(ar);
      
      for (i=0;i<tempGEMS.GetSize(); i++)
        AddJewelryWithCurrKey(tempGEMS[i]);
    }
    else
    {
      GEM_TYPE data;
      int temp;
      ar >> temp;
      ASSERT( temp < MAX_GEMS );
      
      for (i=0; i<temp; i++)
      {
        data.Clear();
        data.Serialize(ar);
        AddGemWithCurrKey(data);
      }

      ar >> temp;
      ASSERT( temp < MAX_JEWELRY );
      
      for (i=0; i<temp; i++)
      {
        data.Clear();
        data.Serialize(ar);
        AddJewelryWithCurrKey(data);
      }
    }
  }
}

//*****************************************************************************
//    NAME: MONEY_SACK::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_SACK::Serialize(CAR &ar, double version)
{
  {
    int i;
    for (i = 0; i < NumCoinTypes(); i++)
    {
      if (ar.IsStoring())
        ar << Coins[i];
      else
        ar >> Coins[i];
    };
  };
  //Gems.Serialize(ar);
  //Jewelry.Serialize(ar);

  POSITION pos;
  if (ar.IsStoring())
  {
    ar << Gems.GetCount();
    pos = Gems.GetHeadPosition();
    while (pos != NULL)
      Gems.GetNext(pos).Serialize(ar);
    
    ar << Jewelry.GetCount();
    pos = Jewelry.GetHeadPosition();
    while (pos != NULL)
      Jewelry.GetNext(pos).Serialize(ar);
  }
  else
  {
    if (version < _VERSION_0911_)
    {
      // old style for loading gems and jewelry
      CArray<GEM_TYPE, GEM_TYPE&> tempGEMS;
      //tempGEMS.Serialize(ar);
      int i, n;
      //ar >> n;
      n = ar.ar.ReadCount();
      tempGEMS.SetSize(n);
      for (i=0; i<n; i++)
      {
        tempGEMS[i].Serialize(ar);
      }
      
      for (i=0;i<tempGEMS.GetSize(); i++)
        AddGemWithCurrKey(tempGEMS[i]);
      
      // now load the jewelry
      tempGEMS.RemoveAll();
      //tempGEMS.Serialize(ar.ar);
      //ar >> n; 
      n = ar.ar.ReadCount();  // Two-byte count
      tempGEMS.SetSize(n);
      for (i=0; i<n; i++)
      {
        tempGEMS[i].Serialize(ar);
      }      
      for (i=0;i<tempGEMS.GetSize(); i++)
        AddJewelryWithCurrKey(tempGEMS[i]);
    }
    else
    {
      GEM_TYPE data;
      int temp, i;
      ar >> temp;
      ASSERT( temp < MAX_GEMS );
      
      for (i=0; i<temp; i++)
      {
        data.Clear();
        data.Serialize(ar);
        AddGemWithCurrKey(data);
      }
      
      ar >> temp;
      ASSERT( temp < MAX_JEWELRY );
      
      for (i=0; i<temp; i++)
      {
        data.Clear();
        data.Serialize(ar);
        AddJewelryWithCurrKey(data);
      }
    }
  }
}
#endif

#ifdef UAFEDITOR
const char *JKEY_MONEY = "money";
//const char *JKEY_COINS = "coins";

void MONEY_SACK::Export(JWriter& jw)
{
  int i;
  POSITION pos;
  jw.StartList(JKEY_MONEY);
  {
    for (i=0;i<NumCoinTypes();i++)
    {
      if (Coins[i] != 0) break;
    };
    if (i != NumCoinTypes())
    {
      jw.NameAndValue(JKEY_COINS, Coins, NumCoinTypes());
    };
    {
      pos = Gems.GetHeadPosition();
      jw.StartArray(JKEY_GEMS);
      while (pos != NULL)
      {
        GEM_TYPE *pGem;
        pGem = &Gems.GetNext(pos);
        jw.NextEntry();
        pGem->Export(jw);
      };
      jw.EndArray();
    };
    {
      jw.StartArray(JKEY_JEWELS);
      pos = Jewelry.GetHeadPosition();
      while (pos != NULL)
      {
        GEM_TYPE *pGem;
        pGem = &Jewelry.GetNext(pos);
        jw.NextEntry();
        pGem->Export(jw);
      };
      jw.EndArray();
    };
  };
  jw.EndList();
}
bool MONEY_SACK::Import(JReader& jr)
{
  jr.StartList(JKEY_MONEY);
  memset(Coins,0, sizeof(Coins));
  jr.Optional(); jr.NameAndValue(JKEY_COINS, Coins, NumCoinTypes());

  {
    GEM_TYPE gem;
    jr.StartArray(JKEY_GEMS);
    while (gem.Import(jr))
    {
      Gems.Insert(gem, gem.id);
    };
    jr.EndArray();
  };
  {
    GEM_TYPE jewel;
    jr.StartArray(JKEY_JEWELS);
    while (jewel.Import(jr))
    {
      Jewelry.Insert(jewel, jewel.id);
    };
    jr.EndArray();
  };
  jr.EndList();
  return true;
}
#endif



//*****************************************************************************
//    NAME: MONEY_SACK::GetBaseType
//
// PURPOSE: 
//
//*****************************************************************************
itemClassType MONEY_SACK::GetBaseType() 
{ 
  return (globalData.moneyData.GetBaseType()); 
}
//*****************************************************************************
//    NAME: MONEY_SACK::GetBaseRate
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::GetBaseRate() 
{ 
  return (globalData.moneyData.GetBaseRate()); 
}
//*****************************************************************************
//    NAME: MONEY_SACK::ConvertToBase
//
// PURPOSE: 
//
//*****************************************************************************
double MONEY_SACK::ConvertToBase(double amount, itemClassType srcType, double *overflow)
{ 
  return (globalData.moneyData.ConvertToBase(amount, srcType, overflow)); 
}

// default currency
//*****************************************************************************
//    NAME: MONEY_SACK::GetDefaultType
//
// PURPOSE: 
//
//*****************************************************************************
itemClassType MONEY_SACK::GetDefaultType() 
{ 
  return (globalData.moneyData.GetDefaultType()); 
}
//*****************************************************************************
//    NAME: MONEY_SACK::GetDefaultRate
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::GetDefaultRate() 
{ 
  return (globalData.moneyData.GetDefaultRate()); 
}
//*****************************************************************************
//    NAME: MONEY_SACK::ConvertToDefault
//
// PURPOSE: 
//
//*****************************************************************************
double MONEY_SACK::ConvertToDefault(double amount, itemClassType srcType, double *overflow)
{ 
  return (globalData.moneyData.ConvertToDefault(amount, srcType, overflow)); 
}

//*****************************************************************************
//    NAME: MONEY_SACK::AddGem
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::AddGem()
{  
  if (Gems.GetCount() >= MAX_GEMS)
    return 0;

  GEM_TYPE data;
  data.id = GetNextGemKey();
  Gems.Insert(data, data.id);
  return (data.id);
}

//*****************************************************************************
//    NAME: MONEY_SACK::AddGem
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::AddGem(const GEM_TYPE &data)
{
  if (Gems.GetCount() >= MAX_GEMS)
    return 0;

  // PRS May 30 2009
  //// we are about to whack your key
  //ASSERT( data.id <= 0 );

  //data.id = GetNextGemKey();
  int id = GetNextGemKey();
  Gems.Insert(data, id);
  return (id);
}

//*****************************************************************************
//    NAME: MONEY_SACK::AddGem
//
// PURPOSE: 
//
//*****************************************************************************
BOOL MONEY_SACK::AddGems(const GEM_LIST &data)
{
  POSITION pos = data.GetHeadPosition();
  while (pos != NULL)
  {
    //if (AddGem(data.GetNext(pos)) == 0)
    if (AddGem(const_cast <GEM_LIST&>(data).GetNext(pos)) == 0)
      return FALSE; // could not add
  }

  return TRUE;
}

//*****************************************************************************
//    NAME: MONEY_SACK::AddJewelry
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::AddJewelry()
{
  if (Jewelry.GetCount() >= MAX_JEWELRY)
    return 0;
  
  GEM_TYPE data;
  data.id = GetNextJewelryKey();
  Jewelry.Insert(data, data.id);
  return (data.id);
}

//*****************************************************************************
//    NAME: MONEY_SACK::AddJewelry
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::AddJewelry(const GEM_TYPE &data)
{
  if (Jewelry.GetCount() >= MAX_JEWELRY)
    return 0;
  
  // PRS May 30 2009
  //// we are about to whack your key
  //ASSERT( data.id <= 0 );

  int id = GetNextJewelryKey();
  //data.id = GetNextJewelryKey();
  //Jewelry.Insert(data, data.id);
  Jewelry.Insert(data, id);
  //return (data.id);
  return (id);
}

//*****************************************************************************
//    NAME: MONEY_SACK::AddJewelry
//
// PURPOSE: 
//
//*****************************************************************************
BOOL MONEY_SACK::AddJewelry(const JEWELRY_LIST &data)
{
  POSITION pos = data.GetHeadPosition();
  while (pos != NULL)
  {
    if (AddJewelry(const_cast<JEWELRY_LIST&>(data).GetNext(pos)) == 0)
      return FALSE; // could not add
  }
  
  return TRUE;
}

//*****************************************************************************
//    NAME: MONEY_SACK::RemoveGem
//
// PURPOSE: 
//
//*****************************************************************************
BOOL MONEY_SACK::RemoveGem(int index)
{
  return (Gems.Remove(index));
}

//*****************************************************************************
//    NAME: MONEY_SACK::RemoveJewelry
//
// PURPOSE: 
//
//*****************************************************************************
BOOL MONEY_SACK::RemoveJewelry(int index)
{
  return (Jewelry.Remove(index));
}

//*****************************************************************************
//    NAME: MONEY_SACK::NumGems
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::NumGems() const 
{ 
  return Gems.GetCount(); 
}
//*****************************************************************************
//    NAME: MONEY_SACK::NumJewelry
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::NumJewelry() const 
{ 
  return Jewelry.GetCount(); 
}

//*****************************************************************************
//    NAME: MONEY_SACK::AppraiseGem
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::AppraiseGem(int index)
{
  POSITION pos = Gems.FindKeyPos(index);
  if (pos == NULL) return 0;

  if (Gems.GetAt(pos).value == 0)
  {
    int value = globalData.moneyData.GetGemValue();
    Gems.GetAt(pos).value = value;
    
    ASSERT( Gems.GetAt(pos).value == value );
  }

  return (Gems.GetAt(pos).GetValue());
}

//*****************************************************************************
//    NAME: MONEY_SACK::AppraiseJewelry
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::AppraiseJewelry(int index)
{
  POSITION pos = Jewelry.FindKeyPos(index);
  if (pos == NULL) return 0;
  
  if (Jewelry.GetAt(pos).value == 0)
    Jewelry.GetAt(pos).value = globalData.moneyData.GetJewelryValue();
  
  return (Jewelry.GetAt(pos).GetValue());
}

//*****************************************************************************
//    NAME: MONEY_SACK::GemValue
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::GemValue(int index)
{
  POSITION pos = Gems.FindKeyPos(index);
  if (pos == NULL) return 0;
  return (Gems.GetAt(pos).GetValue());
}

//*****************************************************************************
//    NAME: MONEY_SACK::JewelryValue
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::JewelryValue(int index)
{
  POSITION pos = Jewelry.FindKeyPos(index);
  if (pos == NULL) return 0;
  return (Jewelry.GetAt(pos).GetValue());
}

//*****************************************************************************
//    NAME: MONEY_SACK::TotalGemValue
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::TotalGemValue()
{
  int total=0;
  POSITION pos = Gems.GetHeadPosition();
  while (pos != NULL)
    total += Gems.GetNext(pos).GetValue();
  return total;
}

//*****************************************************************************
//    NAME: MONEY_SACK::TotalJewelryValue
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::TotalJewelryValue()
{
  int total=0;
  POSITION pos = Jewelry.GetHeadPosition();
  while (pos != NULL)
    total += Jewelry.GetNext(pos).GetValue();
  return total;
}

//*****************************************************************************
//    NAME: MONEY_SACK::RemoveMultGems
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_SACK::RemoveMultGems(int count)
{
  int ng = NumGems();
  int total = min(count, ng);
  for (int i=0;i<total;i++)
    Gems.RemoveHead();
}


//*****************************************************************************
//    NAME: MONEY_SACK::RemoveMultJewelry
//
// PURPOSE: 
//
//*****************************************************************************
void MONEY_SACK::RemoveMultJewelry(int count)
{
  int nj = NumJewelry();
  int total = min(count, nj);
  for (int i=0;i<total;i++)
    Jewelry.RemoveHead();
}


//*****************************************************************************
//    NAME: MONEY_SACK::GetNextGemKey
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::GetNextGemKey()
{
  int key=0;
  POSITION pos;
  if (Gems.GetCount() > 0)
  {
    key = Gems.GetTail().id;
    if (key >= INT_MAX-1)
    {
      // Need to wrap key.
      // Search for non-sequential gap between current keys.
      // A gap is guaranteed since the key range is greater than
      // MAX_ITEMS.
      // Gaps occur when deletions are made to item list
      pos = Gems.GetHeadPosition();
      BOOL found = FALSE;        
      int prevKey = 0;
      int currKey;
      while ((pos != NULL) && (!found))
      {
        currKey = Gems.GetNext(pos).id;
        if (currKey != prevKey+1)
        {
          key = prevKey+1;
          found = TRUE;
        }
        else
          prevKey = currKey; 
      }
      VERIFY(found); // should never happen
    }
    else
      key++;
  }
  else
    key = 1;
  return key;
}

//*****************************************************************************
//    NAME: MONEY_SACK::GetNextJewelryKey
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::GetNextJewelryKey()
{
  int key=0;
  POSITION pos;
  if (Jewelry.GetCount() > 0)
  {
    key = Jewelry.GetTail().id;
    if (key >= INT_MAX-1)
    {
      // Need to wrap key.
      // Search for non-sequential gap between current keys.
      // A gap is guaranteed since the key range is greater than
      // MAX_ITEMS.
      // Gaps occur when deletions are made to item list
      pos = Jewelry.GetHeadPosition();
      BOOL found = FALSE;        
      int prevKey = 0;
      int currKey;
      while ((pos != NULL) && (!found))
      {
        currKey = Jewelry.GetNext(pos).id;
        if (currKey != prevKey+1)
        {
          key = prevKey+1;
          found = TRUE;
        }
        else
          prevKey = currKey; 
      }
      VERIFY(found); // should never happen
    }
    else
      key++;
  }
  else
    key = 1;
  return key;
}

//*****************************************************************************
//    NAME: MONEY_SACK::AddGemWithCurrKey
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::AddGemWithCurrKey(GEM_TYPE &data)
{
  if (Gems.GetCount() < MAX_GEMS)
  {
    Gems.Insert(data, data.id);
    return data.id;
  }
  else
    return 0;
}

//*****************************************************************************
//    NAME: MONEY_SACK::AddJewelryWithCurrKey
//
// PURPOSE: 
//
//*****************************************************************************
int MONEY_SACK::AddJewelryWithCurrKey(GEM_TYPE &data)
{
  if (Jewelry.GetCount() < MAX_GEMS)
  {
    Jewelry.Insert(data, data.id);
    return data.id;
  }
  else
    return 0;
}

//*****************************************************************************
//    NAME: MONEY_SACK::GetTotalWeight
//
// PURPOSE: 
//
//*****************************************************************************
double MONEY_SACK::GetTotalWeight()
{
  int total = 0;
  
  // total nbr of coins
  for (int i=0;i<NumCoinTypes(); i++)
    total += Coins[i];

  total += Gems.GetCount();
  total += Jewelry.GetCount();

  // now compute weight based on qty per unit weight
  
  int CoinsPerUnitOfEncumbrance = globalData.moneyData.GetWeight();

  if (CoinsPerUnitOfEncumbrance > 0)
  {
    total = total / CoinsPerUnitOfEncumbrance;
    total = max(1, total);
  }
  else // this stuff weighs nothing
    total = 0;

  return total;  
}
