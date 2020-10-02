#ifndef __RUNTIMEIF__
#define __RUNTIMEIF__
/******************************************************************************
* Filename: RunTimeIF.h
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

#include "Externs.h"

extern "C" /* disable C++ name decorations */
{ 

#define MAX_KEYWORD_TEXT_LEN 50
#define MAX_TYPE_TEXT_LEN 50


#define PUBLIC extern

#define IF_TYPE_STRING   0
#define IF_TYPE_BYTE     1
#define IF_TYPE_WORD     2
#define IF_TYPE_DWORD    3
#define IF_TYPE_FLOAT    4
#define IF_TYPE_CHARTYPE 5
#define MAX_IF_TYPES     6

#define IF_TYPE_UBYTE   129
#define IF_TYPE_UWORD   130
#define IF_TYPE_UDWORD  132


/* converts "$CHAR_NAME" to enum CHAR_NAME, which
   can be used directly by the following functions */
PUBLIC unsigned long KeywordTextToNum(const char *keyword);
PUBLIC const char *NumToKeywordText(const unsigned long keyword);

PUBLIC unsigned char GetDataSize(const unsigned long keyword);
PUBLIC void GetDataSizeText(const unsigned long keyword, char *text);
PUBLIC void GetDataText(const unsigned long keyword, char *text);
PUBLIC char IsDataReadOnly(const unsigned long keyword);
PUBLIC char IsVirtualTrait(const unsigned long keyword);
PUBLIC int  GetDataAsText(const unsigned long keyword, char *buf, int blen,ActorType *pActor);

PUBLIC const char *   GetDataSTRING(const unsigned long keyword,ActorType *pActor=NULL);
PUBLIC char           GetDataBYTE(  const unsigned long keyword,ActorType *pActor, LPCSTR comment);
PUBLIC short          GetDataWORD(  const unsigned long keyword,ActorType *pActor=NULL);
PUBLIC long           GetDataDWORD( const unsigned long keyword,ActorType *pActor=NULL);
PUBLIC unsigned char  GetDataUBYTE( const unsigned long keyword,ActorType *pActor=NULL);
PUBLIC unsigned short GetDataUWORD( const unsigned long keyword,ActorType *pActor=NULL);
PUBLIC unsigned long  GetDataUDWORD(const unsigned long keyword,ActorType *pActor=NULL);
PUBLIC float          GetDataFLOAT( const unsigned long keyword,ActorType *pActor=NULL);

PUBLIC void SetDataSTRING(const unsigned long keyword, const char *data,ActorType *pActor=NULL);
PUBLIC void SetDataBYTE(  const unsigned long keyword, char data,ActorType *pActor=NULL);
PUBLIC void SetDataWORD(  const unsigned long keyword, short data,ActorType *pActor=NULL);
PUBLIC void SetDataDWORD( const unsigned long keyword, long data,ActorType *pActor=NULL);
PUBLIC void SetDataUBYTE( const unsigned long keyword, unsigned char data,ActorType *pActor=NULL);
PUBLIC void SetDataUWORD( const unsigned long keyword, unsigned short data,ActorType *pActor=NULL);
PUBLIC void SetDataUDWORD(const unsigned long keyword, unsigned long data,ActorType *pActor=NULL);
PUBLIC void SetDataFLOAT( const unsigned long keyword, float data,ActorType *pActor=NULL);

} /* extern "C" */

#endif /* ifndef __RUNTIMEIF__ */