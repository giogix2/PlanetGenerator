/* The MIT License (MIT)
 * 
 * Copyright (c) 2015 Giovanni Ortolani, Taneli Mikkonen, Pingjiang Li, Tommi Puolamaa, Mitra Vahida
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. */



#include <iostream>
#include <exception>

#include "initOgre.h"
#include "PSphere.h"
#include "Common.h"
#include "Test.h"
#include "testui2/mainwindow.h"
#include <QApplication>
#include "ResourceParameter.h"


using namespace std;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <fcntl.h>
#include "windows.h"
#endif


int main(int argc, char *argv[])
{
	if(argc==1) //no arguments, run planet generator
	{

		//#pragma comment( linker, "/subsystem:windows" )
		QApplication a(argc, argv);
		MainWindow w;
		QFont font("Arial", 11);

		a.setFont(font);
		w.show();
		a.exec();
	}else if(argc==2 && (std::strcmp(argv[1],"t")==0) )
	{
#ifdef linux

		cout<<"  linux OS!"<<endl;

#endif
#ifdef __WINDOWS_

		AllocConsole();
		HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
		int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
		FILE* hf_out = _fdopen(hCrt, "w");
		setvbuf(hf_out, NULL, _IONBF, 1);
		*stdout = *hf_out;

		HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
		hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
		FILE* hf_in = _fdopen(hCrt, "r");
		setvbuf(hf_in, NULL, _IONBF, 128);
		*stdin = *hf_in;
		cout<<"  Windows OS!  "<<endl;

#endif
#ifdef _WIN32
		AllocConsole();
		HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
		int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
		FILE* hf_out = _fdopen(hCrt, "w");
		setvbuf(hf_out, NULL, _IONBF, 1);
		*stdout = *hf_out;

		HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
		hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
		FILE* hf_in = _fdopen(hCrt, "r");
		setvbuf(hf_in, NULL, _IONBF, 128);
		*stdin = *hf_in;
		cout<<"  WIN32 OS!  "<<endl;
#endif
		

		cout << "Testing Mode" << endl;
		testAll();
		cout << "Test Ended. Press Enter to exit." << endl;
		cin.get();
	}

	return 0;
}
