#include "pch.h"
#include "MemoryReader.h"

MemoryReader::MemoryReader(BSTR fullPath, int startPointerAt) {
	CFile myFile;

	// read the whole hecking file into a byte[], and move the pointer ahead to some start position where the decompress is supposed to start
	CFileException e;

	if (!myFile.Open(fullPath, CFile::modeRead, &e))
	{
		TRACE("Unable to open race db file %s, error %i\n", fullPath, e.m_cause);
		//return 0;
	}
	ULONGLONG len = myFile.GetLength();
	fullFileData = new char[len];
	fullFileDataLen = len;
	UINT bytesRead = myFile.Read(fullFileData, len);
	myFile.Close();
	pointer = startPointerAt;

}


int MemoryReader::read(unsigned int data[], int count) {
	// 2) Make a CARWrapper::readFromMemory() that will read from that buffer at a given start position and keep track of the start position
	int i = 0;
	int arrayIndex = 0;
	int totalCount = 0;
		for (i = 0; i < count && pointer < fullFileDataLen && totalCount < count; i++) {
		unsigned char* temp = new unsigned char[4];
		// Here I have to read in 4 bytes and then turn that into a 32 bit integer
		for (int j = 0; j < 4; j++) {
			temp[j] = fullFileData[pointer++];
			totalCount++;
		}
		data[arrayIndex] = (temp[3] * 16777216) + (temp[2]* 65536) + (temp[1] * 256) + temp[0];
		arrayIndex++;
	}
	return totalCount;
}

