#pragma once
#include <afx.h>
#include "CAR.h";
#include "MemoryReader.h";


public ref class CARWrapper
{


public:
	CARWrapper(BSTR fullPath, int startPointerAt);
	int decompressInt(int n);
	char* decompressString();
	void catchUpDecompressesFromStart(CAR* car);
	~CARWrapper() {  };
private:
	MemoryReader *memReader;
	int* decompressCalls;
	int* decompressCallTypes;
	int numDecompressCalls;
	BSTR mFullPath;
};

