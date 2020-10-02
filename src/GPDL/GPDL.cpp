
#include <afx.h>
#include <afxtempl.h>
#include "..\Shared\GPDLOpCodes.h"
#include "..\Shared\GPDLcomp.h"
#include "..\Shared\ramfile.h"





GPDLCOMP GPDLcomp;
CFile outfile;
FILE *infile;
char infileLine[1001];
CArchive outarchive(&outfile, CArchive::store);


void *My_malloc(int size)
{
  return malloc(size);
}

void My_free(void *p)
{
  free(p);
}

void *My_realloc(void *p, int size)
{
  return realloc(p,size);
}


void usage(void) {
  char line[80];
	fprintf(stderr,"Usage:\n");
	fprintf(stderr,"   DiscourseCompile  <input text file> <output binary file> [listfile]\n");
  fprintf(stderr," Press Enter -----");
  gets_s(line,80);
	exit(1);
}

const char *GetOneLine(int /*n*/)
{
  if (infile ==NULL) return NULL;
  if (fgets(infileLine,1000,infile) == NULL) return NULL;
  return infileLine;
}

int ProcessErrorMessage(const CString& errMsg, bool wait)
{
  char line[80];
  fprintf(stderr, "%s\n",LPCSTR(errMsg));
  if (wait)
  {
    fprintf(stderr, "    Press Enter  ");
    gets_s(line,80);
  };
  return 0;
}

int main(int argc,char *argv[])
{
  int result;
  CString fname;
  CString pname;
  FILE *listfile=NULL;
  bool listing=false;
  printf("GPDL compiler version 4.7 - 29 Nov 2011\n");
	if (argc<3) usage();
	if (argc>4) usage();
  if ((infile=fopen(argv[1],"r")) == NULL)
  {
		fprintf(stderr,"Cannot open input file %s\n",argv[1]);
		usage();
	};
	if (!outfile.Open(argv[2],CFile::modeWrite|CFile::modeCreate))
  {
		fprintf(stderr,"Cannot open out file %s\n",argv[2]);
		usage();
	};
	if (argc>3)
  {
		listfile=fopen(argv[3],"w");
		if (listfile==NULL) {
			fprintf(stderr,"Cannot open listing file %s\n",argv[3]);
			usage();
		};
		listing=true;
	};
  result = GPDLcomp.CompileProgram(GetOneLine, ProcessErrorMessage);
// ******************************************
// Produce binary file and listing file
// ******************************************
  if (result == 0)
  {
	  GPDLcomp.WriteCode(outarchive);
	  GPDLcomp.WriteConstants(outarchive);
    GPDLcomp.WriteDictionary(outarchive);
	  if (listing)
    {
      ASSERT(listfile != NULL);
      GPDLcomp.list(listfile);
    };
  };
	outarchive.Close();
	outfile.Close();
  printf("GPDL Compiler exiting...\n");
  
	return 0;
}
