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
#include "testui2/mainwindow.h"
#include <QApplication>
#include "ResourceParameter.h"
#include <QDebug>

using namespace std;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <fcntl.h>
#include "windows.h"
#endif

int main(int argc, char *argv[])
{
    unsigned int fontSize;
    QApplication a(argc, argv);
    MainWindow w;

    // Calculate fontsize. size 8 is for dpi 96
    fontSize = 8;
    qDebug() << "DPI is:" << w.logicalDpiX() << w.logicalDpiY();
    fontSize = (unsigned int)((((float)fontSize+0.25f) * (96.0f / w.logicalDpiY()))+0.5f);
    qDebug() << "Setting fontsize to:" << fontSize;

    QFont font("Arial", fontSize);
    a.setFont(font);

    w.show();
    a.exec();

    return 0;
}
