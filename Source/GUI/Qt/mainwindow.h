/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTableWidgetItem>
#include <QMainWindow>
#include <QMenu>

#include "core.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* Parent = 0);
    ~MainWindow();

private:
    void dragEnterEvent(QDragEnterEvent* Event);
    void dropEvent(QDropEvent* Event);
    void Update_Table();

    Ui::MainWindow* Ui;
    QMenu* Context_Menu;

    Core* C;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_Menu_File_Open_Files_triggered();
    void on_Menu_File_Open_Directory_triggered();
    void on_Menu_File_Save_All_triggered();
    void on_Menu_Help_Help_triggered();
    void on_Menu_Help_About_triggered();
    void on_Menu_File_Close_triggered();
    void on_Menu_File_Close_All_triggered();
    void on_Table_Widget_itemSelectionChanged();
    void Show_Context_Menu(const QPoint&);
    void Table_Widget_Changed();
};

#endif // MAINWINDOW_H
