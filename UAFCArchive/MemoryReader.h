#pragma once
#include <afx.h>
public class MemoryReader
{
public:
	MemoryReader(BSTR fullPath, int startPointerAt);
	int read(unsigned int data[], int count);
private:
	char* fullFileData;
	int fullFileDataLen;
	unsigned int pointer;
};

