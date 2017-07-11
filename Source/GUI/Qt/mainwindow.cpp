/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include <QMessageBox>
#include <QFileDialog>
#include <QDropEvent>
#include <QtCore>
#include <QUrl>

#include "core.h"
#include "mainwindow.h"
#include "helpdialog.h"
#include "aboutdialog.h"
#include "tablewidget.h"
#include "ui_mainwindow.h"

#if defined(__APPLE__) && QT_VERSION < 0x050400
#include <CoreFoundation/CFURL.h>
#endif

//---------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *Parent) : QMainWindow(Parent), Ui(new Ui::MainWindow)
{
    Ui->setupUi(this);

    C = new Core();

    Ui->Table_Widget->Setup(C);

   connect(Ui->Table_Widget->model(), SIGNAL(rowsInserted(const QModelIndex& , int, int)),
           this, SLOT(Table_Widget_Changed()));
   connect(Ui->Table_Widget->model(), SIGNAL(rowsRemoved(const QModelIndex& , int, int)),
           this, SLOT(Table_Widget_Changed()));

   //Setup toolbar
   Ui->Tool_Bar->addAction(Ui->Menu_File_Open_Files);
   Ui->Tool_Bar->addAction(Ui->Menu_File_Open_Directory);
   Ui->Tool_Bar->addSeparator();
   Ui->Tool_Bar->addAction(Ui->Menu_File_Save_All);
   Ui->Tool_Bar->addSeparator();
   Ui->Tool_Bar->addAction(Ui->Menu_Help_About);
}

//---------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete Ui;
    delete C;
}

//---------------------------------------------------------------------------
void MainWindow::dragEnterEvent(QDragEnterEvent* Event)
{
    Event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *Event)
{
    if (Event->mimeData()->hasUrls())
    {
        QList<QUrl> Urls = Event->mimeData()->urls();
        for(QList<QUrl>::const_iterator It = Urls.begin(); It != Urls.end(); It++)
        {
            QString File_Name;
#if defined(__APPLE__) && QT_VERSION < 0x050400
            if (It->url().startsWith("file:///.file/id="))
            {
                CFErrorRef Error = 0;
                CFURLRef Cfurl = It->toCFURL();
                CFURLRef Absurl = CFURLCreateFilePathURL(kCFAllocatorDefault, Cfurl, &Error);

                if(Error)
                    continue;

                File_Name = QUrl::fromCFURL(Absurl).toLocalFile();
                CFRelease(Cfurl);
                CFRelease(Absurl);
            }
            else
#endif
            {
                File_Name = It->toLocalFile();
            }
            C->Open_Files(File_Name);
        }
    }

        Ui->Table_Widget->Update_Table();
}

//---------------------------------------------------------------------------
void MainWindow::on_Menu_File_Open_Files_triggered()
{
    QStringList File_Names;

    // Show open dialog
    File_Names = QFileDialog::getOpenFileNames(this,
                 "Select one or more files to open",
                 "",
                 "MOV files (*.mov);;All files (*.*)");

    if (File_Names.empty())
        return;

    for(int Pos = 0; Pos < File_Names.size(); Pos++)
    {
        C->Open_Files(File_Names[Pos]);
    }

    Ui->Table_Widget->Update_Table();
}

//---------------------------------------------------------------------------
void MainWindow::on_Menu_File_Open_Directory_triggered()
{
    QString File_Name;

    // Show open dialog
    File_Name = QFileDialog::getExistingDirectory(this,
                tr("Open Directory"),
                "",
                QFileDialog::ShowDirsOnly
              | QFileDialog::DontResolveSymlinks);

    if (File_Name.isEmpty())
        return;

    C->Open_Files(File_Name);

    Ui->Table_Widget->Update_Table();
}

//---------------------------------------------------------------------------
void MainWindow::on_Menu_Help_Help_triggered()
{
    // Showing Help Dialog
    QPointer<HelpDialog> Help = new HelpDialog(this);
    Help->exec();
}

//---------------------------------------------------------------------------
void MainWindow::on_Menu_Help_About_triggered()
{
    // Showing About Dialog
    QPointer<AboutDialog> About = new AboutDialog(this);
    About->exec();
}

void MainWindow::on_Menu_File_Close_All_triggered()
{
    FileList* Files = C->Get_Files();

    for(FileList::iterator It = Files->begin(); It != Files->end(); It++)
    {
        if(It->Modified)
        {
            int Ret = QMessageBox::warning(this, tr("MOV MetaEdit"),
                tr("The file %1 has been modified.\n"
                "Do you want to close anyways?").arg(It.key()),
                QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::Cancel);

            if(Ret == QMessageBox::Yes)
                continue;
            else if(Ret == QMessageBox::YesToAll)
                break;
            else
                return;
        }
    }

    Files->clear();

    Ui->Table_Widget->Update_Table();

}

//---------------------------------------------------------------------------
void MainWindow::on_Table_Widget_itemSelectionChanged()
{
    if(Ui->Table_Widget->selectedItems().size())
        Ui->Menu_File_Close->setEnabled(true);
    else
        Ui->Menu_File_Close->setEnabled(false);
}

//---------------------------------------------------------------------------
void MainWindow::Table_Widget_Changed()
{
    if(Ui->Table_Widget->rowCount())
        Ui->Menu_File_Close_All->setEnabled(true);
    else
        Ui->Menu_File_Close_All->setEnabled(false);
}
