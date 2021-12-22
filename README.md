# dnqr (Ditto Nedo's QR encoder)

Did You ever thought of converting a string lateral to a QR encoded picture ?
Of course if you didn't now you are considering such and experiment.
I did thought of such a conversion especially from "string" to gdImagePtr\* type.
Which I can flexibly write to any source and tamper with any way i like.

## Create gdImagePtr Objects of QR coded picture

The Issue of Converting a text string to a QR encoded picture is solved by *dnqr* util. It makes the convertion of the output image easy by creating a (libgd) gdImagePtr which
you can use flexibly.

# Example Program
To Test the main.c code you need libqrencode libarchive and libgd.

* libqrenncode
* libarchive
* libgd

# The Example I wrote illustrates the other profound flexiblity of the Function.
	cd src
	make
#Use
	Copy The Function in QrEncode in the file main.c into your source code and enjoy it
