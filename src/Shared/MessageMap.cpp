/******************************************************************************
* Filename: ConfigFile.cpp
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
#include "MessageMap.h"
#include <fstream>
#include <json.hpp>
#include <string>

// for convenience
using json = nlohmann::json;

std::unordered_map<std::string, std::string>* idToValueMap;

MESSAGEMAP::MESSAGEMAP(void)
{
}

MESSAGEMAP::~MESSAGEMAP(void)
{
    delete &idToValueMap;
}

void MESSAGEMAP::LoadFile(CString path) {

    CStdioFile file;

    // read a JSON file
    std::ifstream i(path);
    json jsonData;
    i >> jsonData;
    if (file.m_hFile != CFile::hFileNull) {
        file.Close();
    }

    idToValueMap = new std::unordered_map<std::string, std::string>;

    for (json::iterator it = jsonData.begin(); it != jsonData.end(); ++it) {
        CString key = it.key().c_str();
        std::string strValue;
        it.value().get_to(strValue);
        CString value = strValue.c_str();

        // This does NOT support Unicode
        char* cKey = key.GetBuffer();
        char* cValue = value.GetBuffer();

        idToValueMap->insert({ it.key(), strValue });
    }
}

const char* MESSAGEMAP::getTextFromId(const char* id)
{
    std::unordered_map<std::string, std::string>::const_iterator mapFind = idToValueMap->find(id);
    if (mapFind == idToValueMap->end()) {
        return id;
    }
    else {
        return &(mapFind->second[0]);
    }
}


