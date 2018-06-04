/*
 * MIT License

Copyright (c) 2018 Luiz Gustavo de Campos

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/



#include "databasecontroler.h"
#include "qtspotify.h"

#include "mainwindow.h"

#include <QApplication>


#include <QSslSocket>
#include <QDebug>



int main(int argc, char *argv[])
{

    //  Check the existens of the SSL Libraries
    //  In Windows they have to be installed separatley
    if (!QSslSocket::supportsSsl()) {
        qWarning () << "No SSL Support";
        exit (1);
    }
    qDebug () << QSslSocket::sslLibraryVersionString();

    QApplication a(argc, argv);

    QtSpotify* w = new QtSpotify();

    w->show();

    a.connect(&a, &QApplication::aboutToQuit, [=]() { exit (0);});

    return a.exec();
}
