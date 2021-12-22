/*XXX This Document was modified on 1635488950 */
#include <qrencode.h>
#include <dnqr.h>

gdImage *QrEncode ( char *str )
{
 if( !str || !strlen ( str ) )
  return NULL;

 QRcode *code;
 gdImage *Image;
 unsigned char *row = 0, *p = 0;

 code = QRcode_encodeString8bit ( str, 3, QR_ECLEVEL_H );
 if( !code )
  return NULL;

 p = code->data;
 int rell = ( code->width * 10 + 100 ), last = 0, ty = 0;

 Image = gdImageCreateTrueColor ( rell, rell );
 if( !Image )
  return NULL;

 for( int i = 0; i < ( rell ); i++ ) {
  for( int j = 0; j < ( rell ); j++ ) {
   gdImageSetPixel ( Image, i, j, 0xFFFFFF );
  }
 }

 for( int i = 50; i < ( code->width * 10 ) + 100; i++ ) {
  if( i < ( ( code->width * 10 ) + 50 ) ) {
   row = ( p + ( ( ( i / 10 ) - 5 ) * code->width ) );
   last = 0;
   for( int j = 50; j < ( code->width * 10 ) + 50; j++ ) {
	int ty = 0;
	if( row[( j / 10 ) - 5] & 1 ) {
	 ty = 1;
	 last = 1;
	} else if( last ) {
	 last = 0;
	 ty = 0;
	}
	if( ty ) {
	 gdImageSetPixel ( Image, i, j, 0x000000 );
	}
   }
  }
 }
 return Image;
}
