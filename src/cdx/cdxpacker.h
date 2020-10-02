//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxpacker.h,v $
// $Author: cocoaspud $
//
// $Log: cdxpacker.h,v $
// Revision 1.5  2001/11/12 16:35:03  cocoaspud
// copy/paste events, cdx compile error under VC7
//
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.1.1.1  2000/04/22 16:10:54  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 1.4  1999/08/29 22:29:04  MICHAELR
// added CDXDLL support to classes/etc
//
// Revision 1.3  1999/07/17 10:22:28  Mindcry
// Another small change
//
// Revision 1.2  1999/07/17 10:00:01  Mindcry
// Changed the header. Hope it will show now the Revision and changeslist
//
// Revision 1.1  1999/07/17 11:56:00  Mindcry
// Initial Revision to CVS.
// for expand to Surface you need a special compressor!
// ( I will post it later )
//
// $Revision: 1.5 $
//////////////////////////////////////////////////////////////////////////////////

#define N			4096/* size of ring buffer */
#define F			60	/* upper limit for match_length */
#define THRESHOLD	2   /* encode string into position and length
						   if match_length is greater than this */
#define NIL			N	/* index for root of binary search trees */
#define M   15

#define Q1  (1UL << M)
#define Q2  (2 * Q1)
#define Q3  (3 * Q1)
#define Q4  (4 * Q1)
#define MAX_CUM (Q1 - 1)

#define N_CHAR  (256 - THRESHOLD + F)

typedef void (* EXPANDPROC)(char ch, DWORD pos, DWORD width, DWORD heigth, void *data );
typedef int (* PACKPROC)(DWORD pos, DWORD width, DWORD heigth, void *data );

class CDXAPI CDXPacker
{
public:
	int FilePtrInfo( FILE *in, int &w, int &h, BYTE &b );
	int FileInfo( char *in, int &w, int &h, BYTE &b );
	BOOL PackFile( char *in, char *out );
	BOOL PackRam( BYTE *in, DWORD w, DWORD h, BYTE d, char *out );
	BOOL PackTo( DWORD w, DWORD h, BYTE d, PACKPROC lpProc, void *data, char *out );
	BOOL ExpandToFile( char *in, char *out );
	BYTE *ExpandToRam( char *in );
	BYTE *ExpandToRam( FILE *in );
	BOOL ExpandTo( char *in, EXPANDPROC lpProc, void *data );
	BOOL ExpandTo( FILE *in, EXPANDPROC lpProc, void *data );
	BOOL ExpandSurface( char *in, CDXSurface *lpDDS );
	BOOL ExpandSurface( FILE *in, CDXSurface *lpDDS );

	DWORD textsize, codesize, width, height;
	BYTE bpp;

private:
	void Init( int comp );
	void StartModel(void);
	void UpdateModel(int sym);

	int SpecWrite( BYTE ch );
	int SpecRead( void );
	BOOL ReadHeader( void );
	BOOL WriteHeader( void );

	/* Expand routinez */
	int GetBit(void);
	int BinarySearchSym(WORD x);
	int BinarySearchPos(WORD x);
	void StartDecode(void);
	int DecodeChar(void);
	int DecodePosition(void);
	BOOL Decode(void);

	/* Compress Routinez */
	BOOL PutBit( int bit );
	void FlushBitBuffer( void );
	void InitTree(void);
	void InsertNode(int r);
	void DeleteNode(int p);
	void Output(int bit);
	void EncodeChar(int ch);
	void EncodePosition(int position);
	void EncodeEnd(void);
	BOOL Encode(void);

	EXPANDPROC m_lpEProc;
	PACKPROC m_lpPProc;
	void *m_data;
	DWORD pos, mode;

	BYTE *ramptr;

	BYTE text_buf[N + F - 1];
	int	match_position, match_length, shifts,
		lson[N + 1], rson[N + 257], dad[N + 1],
		char_to_sym[N_CHAR], sym_to_char[N_CHAR + 1];
	WORD	sym_freq[N_CHAR + 1],
			sym_cum[N_CHAR + 1],
			position_cum[N + 1],
			buffer, mask;

	DWORD  low, high, value;
	FILE *fin, *fout;
};

void DibBlit( CDXSurface *lpDDS, int dw, int dh, char *data, int sw, int sh, int bpp  );
