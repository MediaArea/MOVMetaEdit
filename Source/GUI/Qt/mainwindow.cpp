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
#include <QNetworkReply>
#include <QDesktopServices>

#include "core.h"
#include "config.h"
#include "mainwindow.h"
#include "helpdialog.h"
#include "aboutdialog.h"
#include "tablewidget.h"
#include "ui_mainwindow.h"

#include "ZenLib/ZtringListList.h"

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
    Ui->Tool_Bar->addAction(Ui->Menu_File_Close);
    Ui->Tool_Bar->addSeparator();
    Ui->Tool_Bar->addAction(Ui->Menu_File_Save_All);
    Ui->Tool_Bar->addSeparator();
    Ui->Tool_Bar->addAction(Ui->Menu_Help_Help);
    Ui->Tool_Bar->addAction(Ui->Menu_Help_About);

    //Setup context menu
    Context_Menu = new QMenu(this);
    Context_Menu->addAction(Ui->Menu_File_Close);
    //Context_Menu->actions.at(0)->setShortcut(QKeySequence());

    connect(Ui->Table_Widget, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(Show_Context_Menu(const QPoint&)));

    connect(Ui->Table_Widget, SIGNAL(Enable_Save(bool)),
            Ui->Menu_File_Save_All, SLOT(setEnabled(bool)));

    Ui->Table_Widget->Update_Table();

#if !defined(MOVMETAEDIT_NO_VERSION_CHECK)
    CheckUpdate();
#endif
}

//---------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete Context_Menu;
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
void MainWindow::closeEvent(QCloseEvent *event)
{
    on_Menu_File_Close_All_triggered();

    if (C->Get_Files()->empty())
        event->accept();
    else
        event->ignore();
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
void MainWindow::on_Menu_File_Save_All_triggered()
{
    FileList* Files = C->Get_Files();

    for(FileList::iterator It = Files->begin(); It != Files->end(); It++)
    {
        if(It->Modified)
        {
            It->Valid = false; //Done for not writing again the same file
            It->Modified = false;
            C->Save_File(It.key());
        }
    }

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

void MainWindow::on_Menu_File_Close_triggered()
{
    bool Auto = false;

    // Get selections
    QModelIndexList Selected = Ui->Table_Widget->selectionModel()->selectedRows(FILE_COLUMN);
    for(QModelIndexList::iterator It = Selected.begin(); It != Selected.end(); It++)
    {
        QString FileName = It->data(Qt::EditRole).toString();
        if(!Auto && (*C->Get_Files())[FileName].Modified == true)
        {
            int Ret = QMessageBox::warning(this, tr("MOV MetaEdit"),
                tr("The file %1 has been modified.\n"
                "Do you want to close anyways?").arg(FileName),
                QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::Cancel);

            if(Ret == QMessageBox::Cancel)
                return;
            else if(Ret == QMessageBox::YesToAll)
                Auto = true;
        }
        C->Get_Files()->remove(FileName);
    }
    Ui->Table_Widget->Update_Table();
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
void MainWindow::on_Menu_New_Version_triggered()
{
    QDesktopServices::openUrl(QUrl("https://mediaarea.net/MOVMetaEdit"));
}

//---------------------------------------------------------------------------
void MainWindow::Show_Context_Menu(const QPoint& Pos)
{
    Context_Menu->exec(Ui->Table_Widget->mapToGlobal(Pos));
}

//---------------------------------------------------------------------------
void MainWindow::Table_Widget_Changed()
{
    if(Ui->Table_Widget->rowCount())
        Ui->Menu_File_Close_All->setEnabled(true);
    else
        Ui->Menu_File_Close_All->setEnabled(false);
}

#if !defined(MOVMETAEDIT_NO_VERSION_CHECK)
//---------------------------------------------------------------------------
void MainWindow::CheckUpdate()
{
    CheckUpdate_Handle = new QNetworkAccessManager(this);

#if QT_VERSION >=0x040700
    if (!CheckUpdate_Handle->networkAccessible())
    {
        CheckUpdate_Handle->deleteLater();
        CheckUpdate_Handle = NULL;
        return;
    }
#endif

    connect(CheckUpdate_Handle, SIGNAL(finished(QNetworkReply*)), this, SLOT(CheckUpdateReceived(QNetworkReply*)));
    CheckUpdate_Handle->get(QNetworkRequest(QUrl("https://mediaarea.net/movmetaedit_check/test_" VERSION ".txt")));
}

//---------------------------------------------------------------------------
void MainWindow::CheckUpdateReceived(QNetworkReply* NetworkReply)
{
    QByteArray ReplyTemp(NetworkReply->readAll());
    CheckUpdate_Handle->deleteLater();
    CheckUpdate_Handle = NULL;

    ZtringListList Reply;
    ZtringList Version_cur;
    ZtringList Version_get;
    Reply.Separator_Set(1, ","); //CSV
    Reply.Write(Ztring(ReplyTemp.begin(), ReplyTemp.size()));

    Ztring VersionTemp = Reply.FindValue("NewVersion");

    if (VersionTemp.empty())
        return;

    Version_cur.Separator_Set(0, ".");
    Version_get.Separator_Set(0, ".");

    Version_cur.Write(Ztring(VERSION));
    Version_get.Write(VersionTemp);

    for (size_t Pos = 0; Pos < Version_cur.size() || Pos < Version_get.size(); Pos++)
    {
        int64u Cur = Pos<Version_cur.size()?Version_cur.Read(Pos).To_int64u():0;
        int64u Get = Pos<Version_get.size()?Version_get.Read(Pos).To_int64u():0;

        if (Get > Cur)
        {
            QString StatutTip = Ui->Menu_New_Version->statusTip()
              .arg(QString().fromStdString(((VersionTemp.To_UTF8()))));

            Ui->Menu_New_Version->setStatusTip(StatutTip);
            Ui->Menu_New_Version->setVisible(true);

            //MacOS don't support toplevel menu actions
#if defined(__APPLE__)
            Ui->Menu_New_Version->setMenuRole(QAction::ApplicationSpecificRole);
            Ui->Menu_Bar->addMenu(Ui->Menu_New_Version->text())->addAction(Ui->Menu_New_Version);
#endif
            return;
        }
        else if (Get < Cur)
            return;
    }
}
#endif //MOVMETAEDIT_NO_VERSION_CHECK
