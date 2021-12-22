#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <qrencode.h>
#include <gd.h>

gdImage *QrEncode(char *str)
{
    if (!str || !strlen(str))
	return NULL;

    QRcode *code;
    gdImage *Image;
    unsigned char *row = 0, *p = 0;

    code = QRcode_encodeString8bit(str, 3, QR_ECLEVEL_H);
    if (!code)
	return NULL;

    p = code->data;
    int rell = (code->width * 10 + 100), last = 0, ty = 0;

    Image = gdImageCreateTrueColor(rell, rell);
    if (!Image)
	return NULL;

    for (int i = 0; i < (rell); i++) {
	for (int j = 0; j < (rell); j++) {
	    gdImageSetPixel(Image, i, j, 0xFFFFFF);
	}
    }

    for (int i = 50; i < (code->width * 10) + 100; i++) {
	if (i < ((code->width * 10) + 50)) {
	    row = (p + (((i / 10) - 5) * code->width));
	    last = 0;
	    for (int j = 50; j < (code->width * 10) + 50; j++) {
		int ty = 0;
		if (row[(j / 10) - 5] & 1) {
		    ty = 1;
		    last = 1;
		} else if (last) {
		    last = 0;
		    ty = 0;
		}
		if (ty) {
		    gdImageSetPixel(Image, i, j, 0x000000);
		}
	    }
	}
    }
    return Image;
}

int (main)
 (int argsc, char **args) {
    if (!args[1])
	return 0;
    gdImagePtr ar = QrEncode(args[1]);
    if (!ar)
	return 1;
    do {
	gdImagePng(ar, stdout);
	fflush(stdout);
    } while (0);
    gdImageDestroy(ar);
    return 0;
}
