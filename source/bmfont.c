/* example1.c                                                      */
/*                                                                 */
/* This small program shows how to print a rotated string with the */
/* FreeType 2 library.                                             */


#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <falso_jni/FalsoJNI.h>


#define PERLINE_MAXW 32
#define PERLINE_WCNT 24
#define PIXEL_PER_BYTE  4
#define WIDTH  (PERLINE_WCNT*PERLINE_MAXW)

#define PERLINE_MAXH PERLINE_MAXW
#define HEIGHT  PERLINE_MAXH


/* origin is the upper left corner */
unsigned int image[HEIGHT][WIDTH];
FT_Library    library;
FT_Face       face;
FT_GlyphSlot  slot;
FT_Error      error;
extern void (*nativeInitBitmapDC)(JNIEnv*  env, jobject thiz, int width, int height, jbyteArray pixels);
jbyteArray bArr;

/* Replace this function with something useful. */

void draw_bitmap( FT_Bitmap* bitmap, FT_Int x, FT_Int y)
{
    FT_Int  i, j, p, q;
    FT_Int  x_max = x + bitmap->width;
    FT_Int  y_max = y + bitmap->rows;


    /* for simplicity, we assume that `bitmap->pixel_mode' */
    /* is `FT_PIXEL_MODE_GRAY' (i.e., not a bitmap font)   */

    for ( i = x, p = 0; i < x_max; i++, p++ )
    {
        for ( j = y, q = 0; j < y_max; j++, q++ )
        {
          if ( i < 0 || j < 0 || i >= WIDTH || j >= HEIGHT )
            continue;
          char gray = bitmap->buffer[q * bitmap->width + p];

          image[j][i] |= (gray<<16) | (gray<<8) | gray;
          //image[j][i] = gray;
        }
    }
}

unsigned long utf8_next_char( const char** p )
{
    const unsigned char* s = (const unsigned char*)*p;
    unsigned long c = 0;

    if ( !*s ) return 0;

    if ( (*s & 0x80) == 0 ) {
        c = *s++;
    } else if ( (*s & 0xE0) == 0xC0 ) {
        c = ((*s++ & 0x1F) << 6);
        if (*s) c |= (*s++ & 0x3F);
    } else if ( (*s & 0xF0) == 0xE0 ) {
        c = ((*s++ & 0x0F) << 12);
        if (*s) c |= ((*s++ & 0x3F) << 6);
        if (*s) c |= (*s++ & 0x3F);
    } else if ( (*s & 0xF8) == 0xF0 ) {
        c = ((*s++ & 0x07) << 18);
        if (*s) c |= ((*s++ & 0x3F) << 12);
        if (*s) c |= ((*s++ & 0x3F) << 6);
        if (*s) c |= (*s++ & 0x3F);
    } else {
        s++; 
    }

    *p = (const char*)s;
    return c;
}

int bm_init()
{
    error = FT_Init_FreeType( &library );              /* initialize library */
    /* error handling omitted */
    sceClibPrintf("isInited library: %d\n", error);
    error = FT_New_Face( library, DATA_PATH"assets/fonts/gothic_bold.ttf", 0, &face );/* create face object */
    sceClibPrintf("isInited Font: %d\n", error);
    slot = face->glyph;
    // prefetch slot

    bArr = jni->NewByteArray(&jni, sizeof(image));

//    FT_Done_Face    ( face );
//    FT_Done_FreeType( library );

    return 0;
}

void bm_draw(const char *str,const char *fontName,int pFontSize,int pAlignment,int pWidth,int pHeight){
    unsigned long code;
    int           n, num_chars;
    FT_Int       x=0, y=pFontSize;

    FT_Set_Pixel_Sizes(face, 0, pFontSize);
    /* error handling omitted */
    memset(image,0,sizeof(image));
    while ((code = utf8_next_char(&str)) !=0)
    {
      /* load glyph image into the slot (erase previous one) */
      error = FT_Load_Char( face, code, FT_LOAD_RENDER );
      if ( error )
        continue;                 /* ignore errors */

      draw_bitmap( &slot->bitmap,
                  x + slot->bitmap_left,
                  y - slot->bitmap_top );
      x += slot->advance.x >> 6;
      y += slot->advance.y >> 6;
    }
    //show_image(PERLINE_MAXH - slot->bitmap_top, 0);
    jni->SetByteArrayRegion(&jni, bArr, 0, sizeof(image), (const jbyte *)image);
    nativeInitBitmapDC(&jni,NULL,WIDTH,HEIGHT,bArr);
}
