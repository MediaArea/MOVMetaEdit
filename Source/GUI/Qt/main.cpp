/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include <QApplication>

#include "mainwindow.h"

#if defined(_WIN32) && !defined(_DLL)
    #include <QtPlugin>
    Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
