#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTableWidgetItem>
#include <QMainWindow>

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

    Core* C;

private slots:
    void on_Menu_File_Open_Files_triggered();
    void on_Menu_File_Open_Directory_triggered();
    void on_Menu_Help_Help_triggered();
    void on_Menu_Help_About_triggered();
    void on_Menu_File_Close_All_triggered();
    void on_Table_Widget_itemSelectionChanged();
    void Table_Widget_Changed();
};

#endif // MAINWINDOW_H
