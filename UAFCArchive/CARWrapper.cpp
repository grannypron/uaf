#include "pch.h"
#include "CARWrapper.h"


CARWrapper::CARWrapper(BSTR fullPath, int startPointerAt) : memReader() {
	memReader = new MemoryReader(fullPath, startPointerAt);
	mFullPath = fullPath;
	decompressCalls = new int[10000];
	decompressCallTypes = new int[10000];
	numDecompressCalls = 0;
}


int CARWrapper::decompressInt(int n) {
	char* returnData;
	// 3) Change CAR::decompress to accept a CARWrapper and to use CARWrapper::readFromMemory instead of CArchive::Read

	// Make the CAR
	CFile myFile;

	// read the whole hecking file into a byte[], and move the pointer ahead to some start position where the decompress is supposed to start
	CFileException e;

	if (!myFile.Open(mFullPath, CFile::modeRead, &e))
	{
		TRACE("Unable to open race db file %s, error %i\n", mFullPath, e.m_cause);
		//return 0;
	}
	CAR car(&myFile, CArchive::load);
	car.Compress(true);
	catchUpDecompressesFromStart(&car);

	int Int;
	char* c = new char[n];
	// Call it once more and increment the counters
	car.decompressInt((char *)(&Int), n, memReader);
	decompressCalls[numDecompressCalls] = n;
	decompressCallTypes[numDecompressCalls] = 0;
	numDecompressCalls++;
	return Int;

	// OK, we can't expose CAR::decompress to C# because it is not managed.  C# is going to call CARWrapper::decompress, 
	// CARWrapper::decompress is going to skip ahead to the position of the last read and then call CAR::decompress?  No because
	// the tables inside CAR won't have been created.  So, CARWrapper::decompress is going to call CAR::decompress X times, starting
	// from the beginning of its memory stream and then return the result.
	// THIS ASSUMES THAT THERE ARE NO UNCOMPRESSED READS IN BETWEEN COMPRESSED ONES
}

void CARWrapper::catchUpDecompressesFromStart(CAR* car) {

	// Re-call the decompress all the way back up to the place we were at thus far
	for (int i = 0; i < numDecompressCalls; i++) {
		if (decompressCallTypes[i] == 0) {
			// Int
			char* c = new char[decompressCalls[i]];
			car->decompressInt(c, decompressCalls[i], memReader);
		}
		else if (decompressCallTypes[i] == 1) {
			CString c;
			car->decompressString(c, memReader);
		}
	}
}



char* CARWrapper::decompressString() {
	CString returnData;
	// Make the CAR
	CFile myFile;

	// read the whole hecking file into a byte[], and move the pointer ahead to some start position where the decompress is supposed to start
	CFileException e;

	if (!myFile.Open(mFullPath, CFile::modeRead, &e))
	{
		TRACE("Unable to open race db file %s, error %i\n", mFullPath, e.m_cause);
		//return 0;
	}
	CAR car(&myFile, CArchive::load);
	car.Compress(true);
	catchUpDecompressesFromStart(&car);

	CString str;
	// Call it once more and increment the counters
	car.decompressString(str, memReader);
	decompressCalls[numDecompressCalls] = 0;
	decompressCallTypes[numDecompressCalls] = 1;
	numDecompressCalls++;
	CT2A ascii(str);
	return ascii.m_psz;
}
