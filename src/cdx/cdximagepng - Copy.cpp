#include "CDXImagePNG.h"
#include "../PNG/png.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDXImagePNG::CDXImagePNG()
{
}

CDXImagePNG::~CDXImagePNG()
{
}

BOOL CDXImagePNG::Validate(const char *head, const int headsize, const char *foot, const int footsize)
{
  return (!png_sig_cmp((BYTE*)head, 0, 8));
}

#define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)

inline int
CalculateLine(int width, int bitcount) {
	return ((width * bitcount) + 7) / 8;
}

inline int
CalculatePitch(int line) {
	return line + 3 & ~3;
}

inline int
CalculateUsedColors(int bitcount) {
	int c = 1 << bitcount;

	return ((c <= 256) && (c > 1)) ? c : 0;
}

//LPDIRECTDRAWSURFACE CDXImagePNG::GetImage(LPDIRECTDRAW lpDD, const char *file, LONG lSize, BYTE memType )
CDX_LPDIRECTDRAWSURFACE CDXImagePNG::GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* file, BYTE memType)
{
  png_structp png_ptr;
  png_infop info_ptr;
  unsigned int sig_read = 0;
  png_uint_32 width, height;
  int bit_depth, color_type;
  FILE *fp;
  double gamma = 0;
	double screen_gamma = 2.2;
  
  if ((fp = fopen(file, "rb")) == NULL)
    return NULL;
  
    /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also supply the
    * the compiler header file version, so that we know if the application
    * was compiled with a compatible version of the library.  REQUIRED
  */
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                    NULL, 
                                    NULL, 
                                    NULL);
  
  if (png_ptr == NULL)
  {
    fclose(fp);
    return NULL;
  }
  
  /* Allocate/initialize the memory for image information.  REQUIRED. */
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL)
  {
    fclose(fp);
    png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    return NULL;
  }
  
  /* Set error handling if you are using the setjmp/longjmp method (this is
  * the normal method of doing things with libpng).  REQUIRED unless you
  * set up your own error handlers in the png_create_read_struct() earlier.
  */
  
  if (setjmp(png_jmpbuf(png_ptr)))
  {
    /* Free all of the memory associated with the png_ptr and info_ptr */
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    fclose(fp);
    /* If we get here, we had a problem reading the file */
    return NULL;
  }
  
  /* Set up the input control if you are using standard C streams */
  png_init_io(png_ptr, fp);
   
  png_set_strip_alpha(png_ptr);
  png_set_bgr(png_ptr);
  png_set_strip_16(png_ptr);
  png_set_gray_to_rgb(png_ptr);

  if (png_get_gAMA(png_ptr, info_ptr, &gamma))
    png_set_gamma(png_ptr, screen_gamma, gamma);				
  else
	  png_set_gamma(png_ptr, screen_gamma, 0.45455);

  /*
  * If you have enough memory to read in the entire image at once,
  * and you need to specify only transforms that can be controlled
  * with one of the PNG_TRANSFORM_* bits (this presently excludes
  * dithering, filling, setting background, and doing gamma
  * adjustment), then you can read the entire image (including
  * pixels) into the info structure with this call:
  */
  png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_ALPHA, NULL);
  
  /* At this point you have read the entire image */  
  png_get_IHDR(png_ptr, info_ptr, &width, &height,
               &bit_depth, &color_type, NULL,NULL,NULL);
 			
  if (color_type == PNG_COLOR_TYPE_PALETTE) 
	  png_set_expand(png_ptr);

  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
	  png_set_expand(png_ptr);

  // remove alpha and transparency data
  png_color_16 my_background;
  memset(&my_background, 0, sizeof(my_background));
  png_color_16p image_background;

  if (png_get_bKGD(png_ptr, info_ptr, &image_background))
      png_set_background(png_ptr, image_background,
                         PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
  else
      png_set_background(png_ptr, &my_background,
                         PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
  //if (bit_depth == 16)
  //    png_set_strip_16(png_ptr);

  //if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	//  png_set_gray_to_rgb(png_ptr);

  //if (png_get_gAMA(png_ptr, info_ptr, &gamma))
	//  png_set_gamma(png_ptr, screen_gamma, gamma);				
  //else
	//  png_set_gamma(png_ptr, screen_gamma, 0.45455);

	int pixel_depth = bit_depth * info_ptr->channels;
	int colors_used = CalculateUsedColors(pixel_depth);

	int line   = CalculateLine(width, pixel_depth);
	int pitch  = CalculatePitch(line);

  if(!AllocateStorage(width, height, pixel_depth))
    return NULL;

	//setup the BitmapInfo structure required for StretchDIBits
	SetupBitmapInfo();

  if ((color_type & PNG_COLOR_MASK_ALPHA) && (info_ptr->channels == 4))
    pixel_depth = bit_depth * 3;      

  png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

  switch(pixel_depth) 
  {
	case 2 :
	case 4 :
  case 8 :
  {		  
	  // read the palette or fill the palette with an ascending shade of gray	  
	  png_colorp pal;
	  int num_palette;

	  png_get_PLTE(png_ptr, info_ptr, &pal, &num_palette);

    RGBQUAD buf;
    memset(&buf, 0, sizeof(buf));

	  switch (color_type) 
    {
		  case PNG_COLOR_TYPE_GRAY :
		  {
			  for (int count = 0; count < colors_used; count++) {
				  m_pBitmapInfo->bmiColors[count].rgbRed = count;
				  m_pBitmapInfo->bmiColors[count].rgbGreen = count;
				  m_pBitmapInfo->bmiColors[count].rgbBlue = count;
			  }

			  break;
		  }

		  case PNG_COLOR_TYPE_PALETTE :
		  {
			  for (int count = 0; count < info_ptr->num_palette; count++) {
				  m_pBitmapInfo->bmiColors[count].rgbRed = pal->red;
				  m_pBitmapInfo->bmiColors[count].rgbGreen = pal->green;
				  m_pBitmapInfo->bmiColors[count].rgbBlue = pal->blue;				  
				  pal++;
			  }

			  break;
		  }

		  default :
        break;
	  }

	  // read the image data

	  BYTE *bits = ((BYTE*)GetStoragePtr() + (line * height)) - line;

	  for (unsigned rows = 0; rows < height; rows++) 
    {
		  memcpy(bits, row_pointers[rows], line);
		  bits -= line;
	  }

	  break;
  }

  case 24 :
  {  
	  // read the image data
	  BYTE *bits = ((BYTE*)GetStoragePtr() + (line * height)) - line;

	  for (unsigned rows = 0; rows < height; rows++) 
    {
		  memcpy(bits, row_pointers[rows], line);
		  bits -= line;
	  }
	  break;
  }

  default :
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    fclose(fp);
    return NULL;
  }


  //png_get_rowbytes(png_ptr, info_ptr)
  /* clean up after the read, and free any memory allocated - REQUIRED */
  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
  
  /* close the file */
  fclose(fp);
  
	//create surface, this uses member variables set during AllocateStorage
	if(!CreateSurface(lpDD, memType))
		return NULL;

	//return pointer to surface
	return CopyBits();
}
