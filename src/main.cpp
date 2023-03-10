#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <libqalculate/Calculator.h>
extern "C" {
#include <qrencode.h>
#include <python3.8/Python.h>
#include <gd.h>
};

using namespace std;

struct QrException : public std::exception
{
								string stre;
								QrException (const string&& ex) : stre (ex)
								{}
								virtual const char* what()
								{
																return stre.data();
								}
};

struct CodeQr {
								QRcode* qrc;
								CodeQr (QRcode*&& qr) : qrc (qr) {}
								operator QRcode*&()
								{
																return qrc;
								}
								QRcode*& operator->()
								{
																return qrc;
								}
								~CodeQr (){}
};

struct Picture {
								gdImagePtr ptr{0};
								int refcnt{1};

								Picture()
								{
																ptr=0;
								}
								Picture(gdImagePtr ptr) : ptr(ptr) {}
								operator gdImagePtr&()
								{
																return ptr;
								}
								Picture& operator=(const Picture& cp)
								{
																ptr=cp.ptr;
																return *this;
								}
								Picture(Picture&& cp)
								{
																ptr=cp.ptr;
								}
								Picture(const Picture& cp)
								{
																ptr=cp.ptr;
								}
								gdImagePtr& operator->()
								{
																return ptr;
								}
								~Picture ()
								{
																if (ptr!=0)
																{
																								gdImageDestroy(ptr);
																}
								}
};

void
QrEncode(Picture& ret, char *str)
{
    if (!str || !strlen(str))
				{
												throw QrException("You provided empty string as an argument");
				}


    unsigned char *row = 0, *p = 0;
				CodeQr code (QRcode_encodeString8bit(str, 3, QR_ECLEVEL_H));
    if (!code)
				{
												throw QrException ("Failed to allocate a QRcode struct.");
				}

    p = code->data;
    int rell = (code->width * 10 + 100), last = 0, ty = 0;
				using namespace std;

     Picture Image(gdImageCreateTrueColor(rell, rell));
    if (!Image)
												throw QrException ("Failed to create a GD image object.");

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
    ret= Image;
}
int gd_my_xx_putbuf (gdIOCtx* x,const void *buf, int size)
{
								PyObject* fileobj=(PyObject*) x->data;
								PyObject_CallMethod (fileobj,
																"write",
																"(O)",
																PyBytes_FromStringAndSize ((const char *)buf, size));
								return size;
}

struct mqueue : public queue<Calculator*>
{
								mutex m;
								mqueue () {}
								void push(Calculator* c)
								{
																lock_guard<mutex> lk{m};
																queue<Calculator*>::push (c);
								}
								decltype(auto) size()
								{
																lock_guard<mutex> lk{m};
																return queue<Calculator*>::size();
								}
								decltype(auto) front()
								{
																lock_guard<mutex> lk{m};
																return queue<Calculator*>::front();
								}
								void pop()
								{
																lock_guard<mutex> lk{m};
																queue<Calculator*>::pop();
								}
};

mqueue qlc;

PyObject*
qr_str_qalc (PyObject* self,
																PyObject* args,
																PyObject* kwargs)
{
								char *str=0;
								if (!PyArg_ParseTuple (args, "s", &str))
								{
																return 0;
								}
								while (1)
								{
																if (!qlc.size ())
																								continue;
																Calculator* cp=qlc.front();
																string s=cp->calculateAndPrint
																								(string(str), 2000);
																qlc.pop ();
								return PyUnicode_FromString (s.data());
								}
}

PyObject*
qr_str_to_xx (PyObject* self,
																PyObject* args,
																PyObject* kwargs)
{
								char *str=0;
								PyObject* fileobj;
								if (!PyArg_ParseTuple (args, "sO", &str, &fileobj))
								{
																return 0;
								}

								Picture ar;
								try {
																QrEncode(ar, str);
								}
								catch (...)
								{
												PyErr_NoMemory ();
												return 0;
								}
								do {
												gdIOCtx octx;
												octx.data = fileobj;
												octx.putBuf = gd_my_xx_putbuf;
												gdImagePngCtxEx(ar, &octx, 1);

								} while (0);

								Py_XINCREF (Py_None);
								return Py_None;
}

PyObject* bltn_bgit ()
{
								PyObject* mod;
								static PyMethodDef methods[]={
									{"_tostr",
										(PyObject*(*)(PyObject*, PyObject*))
																		qr_str_to_xx, METH_VARARGS,
										"_tostr, converts your (arg: str) to Qrencoded picture"
									},
									{"_calc",
										(PyObject*(*)(PyObject*, PyObject*))
																		qr_str_qalc, METH_VARARGS,
										"_calc, calculates the speacied string"
									},
									{NULL, NULL, 0, NULL}
								};

								static PyModuleDef bgit_def={
									PyModuleDef_HEAD_INIT,
									"qr", 0, -1,
									methods
								};

								mod=PyModule_Create (&bgit_def);
								if (!mod)
								{
																PyErr_NoMemory ();
																return NULL;
								}
								return mod;

}

int (main)
 (int argsc, char **args)
try
{
								thread thd([]()
								{
																while (1)
																{
																								if (qlc.size()>10)
																								{
																																continue;
																								}
																								Calculator *cp=new Calculator();

																								cp->loadGlobalDefinitions ();
																								cp->loadGlobalPrefixes ();
																								cp->loadGlobalCurrencies ();
																								cp->loadGlobalFunctions ();
																								cp->loadGlobalUnits ();
																								cp->loadExchangeRates ();
																								cp->loadGlobalVariables ();
																								cp->loadGlobalDataSets ();
																								cp->loadGlobalDefinitions ();
																								qlc.push(cp);
																}
								});

								PyPreConfig pconfig;
								PyConfig config;
								PyImport_AppendInittab ("qr", bltn_bgit);
								PyConfig_InitPythonConfig (&config);
								PyConfig_SetBytesArgv (&config, argsc, args);
								Py_InitializeFromConfig (&config);
								Py_Initialize ();
								Py_BytesMain (argsc, args);
								Py_Finalize ();
    return 0;
}catch (std::exception& e)
{
								cerr<<"The entire program terminated ::"<<e.what ()<<endl;
								return 1;
}
