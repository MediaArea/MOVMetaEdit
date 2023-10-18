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
#include "Common/config.h"
#include "mainwindow.h"
#include "helpdialog.h"
#include "aboutdialog.h"
#include "tablewidget.h"
#include "techtablewidget.h"
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

    Ui->Tech_Table_Widget->Setup(C);
    Ui->Table_Widget->Setup(C);

    connect(Ui->Tech_Table_Widget->model(), SIGNAL(rowsInserted(const QModelIndex& , int, int)),
            this, SLOT(Tech_Table_Widget_Changed()));
    connect(Ui->Tech_Table_Widget->model(), SIGNAL(rowsRemoved(const QModelIndex& , int, int)),
            this, SLOT(Tech_Table_Widget_Changed()));

    //Setup toolbar
    Ui->Tool_Bar->addAction(Ui->Menu_File_Open_Files);
    Ui->Tool_Bar->addAction(Ui->Menu_File_Open_Directory);
    Ui->Tool_Bar->addSeparator();
    Ui->Tool_Bar->addAction(Ui->Menu_File_Close);
    Ui->Tool_Bar->addSeparator();
    Ui->Tool_Bar->addAction(Ui->Menu_File_Save_All);
    Ui->Tool_Bar->addSeparator();
    Ui->Tool_Bar->addAction(Ui->Menu_View_Technical);
    Ui->Tool_Bar->addAction(Ui->Menu_View_AdId);
    Ui->Tool_Bar->addSeparator();
    Ui->Tool_Bar->addAction(Ui->Menu_Help_Help);
    Ui->Tool_Bar->addAction(Ui->Menu_Help_About);

    //Setup context menu
    Context_Menu = new QMenu(this);
    Context_Menu->addAction(Ui->Menu_File_Close);

    //Setup tech view menu
    QStringList HiddenColumns = QSettings().value("techtablewidget/hiddencolumns", QStringList()).toStringList();
    Ui->Menu_View_Tech_Clef->setData((int)TechTableWidget::CLEF_COLUMN);
    Ui->Menu_View_Tech_Clef->setChecked(!HiddenColumns.contains("clef"));
    connect(Ui->Menu_View_Tech_Clef, SIGNAL(toggled(bool)), Ui->Tech_Table_Widget, SLOT(On_View_Option_Changed(bool)));
    Ui->Menu_View_Tech_Prof->setData((int)TechTableWidget::PROF_COLUMN);
    Ui->Menu_View_Tech_Prof->setChecked(!HiddenColumns.contains("prof"));
    connect(Ui->Menu_View_Tech_Prof, SIGNAL(toggled(bool)), Ui->Tech_Table_Widget, SLOT(On_View_Option_Changed(bool)));
    Ui->Menu_View_Tech_Enof->setData((int)TechTableWidget::ENOF_COLUMN);
    Ui->Menu_View_Tech_Enof->setChecked(!HiddenColumns.contains("enof"));
    connect(Ui->Menu_View_Tech_Enof, SIGNAL(toggled(bool)), Ui->Tech_Table_Widget, SLOT(On_View_Option_Changed(bool)));
    Ui->Menu_View_Tech_Pasp->setData((int)TechTableWidget::PASP_COLUMN);
    Ui->Menu_View_Tech_Pasp->setChecked(!HiddenColumns.contains("pasp"));
    connect(Ui->Menu_View_Tech_Pasp, SIGNAL(toggled(bool)), Ui->Tech_Table_Widget, SLOT(On_View_Option_Changed(bool)));
    Ui->Menu_View_Tech_Wscl->setData((int)TechTableWidget::WSCL_COLUMN);
    Ui->Menu_View_Tech_Wscl->setChecked(!HiddenColumns.contains("wscl"));
    connect(Ui->Menu_View_Tech_Wscl, SIGNAL(toggled(bool)), Ui->Tech_Table_Widget, SLOT(On_View_Option_Changed(bool)));
    Ui->Menu_View_Tech_Fiel->setData((int)TechTableWidget::FIEL_COLUMN);
    Ui->Menu_View_Tech_Fiel->setChecked(!HiddenColumns.contains("fiel"));
    connect(Ui->Menu_View_Tech_Fiel, SIGNAL(toggled(bool)), Ui->Tech_Table_Widget, SLOT(On_View_Option_Changed(bool)));
    Ui->Menu_View_Tech_Colr->setData((int)TechTableWidget::COLR_COLUMN);
    Ui->Menu_View_Tech_Colr->setChecked(!HiddenColumns.contains("colr"));
    connect(Ui->Menu_View_Tech_Colr, SIGNAL(toggled(bool)), Ui->Tech_Table_Widget, SLOT(On_View_Option_Changed(bool)));
    Ui->Menu_View_Tech_Gama->setData((int)TechTableWidget::GAMA_COLUMN);
    Ui->Menu_View_Tech_Gama->setChecked(!HiddenColumns.contains("gama"));
    connect(Ui->Menu_View_Tech_Gama, SIGNAL(toggled(bool)), Ui->Tech_Table_Widget, SLOT(On_View_Option_Changed(bool)));
    Ui->Menu_View_Tech_Clap->setData((int)TechTableWidget::CLAP_COLUMN);
    Ui->Menu_View_Tech_Clap->setChecked(!HiddenColumns.contains("clap"));
    connect(Ui->Menu_View_Tech_Clap, SIGNAL(toggled(bool)), Ui->Tech_Table_Widget, SLOT(On_View_Option_Changed(bool)));
    Ui->Menu_View_Tech_Dpri->setData((int)TechTableWidget::DPRI_COLUMN);
    Ui->Menu_View_Tech_Dpri->setChecked(!HiddenColumns.contains("dpri"));
    connect(Ui->Menu_View_Tech_Dpri, SIGNAL(toggled(bool)), Ui->Tech_Table_Widget, SLOT(On_View_Option_Changed(bool)));
    Ui->Menu_View_Tech_Dlum->setData((int)TechTableWidget::DLUM_COLUMN);
    Ui->Menu_View_Tech_Dlum->setChecked(!HiddenColumns.contains("dlum"));
    connect(Ui->Menu_View_Tech_Dlum, SIGNAL(toggled(bool)), Ui->Tech_Table_Widget, SLOT(On_View_Option_Changed(bool)));
    Ui->Menu_View_Tech_Mcll->setData((int)TechTableWidget::MCLL_COLUMN);
    Ui->Menu_View_Tech_Mcll->setChecked(!HiddenColumns.contains("mcll"));
    connect(Ui->Menu_View_Tech_Mcll, SIGNAL(toggled(bool)), Ui->Tech_Table_Widget, SLOT(On_View_Option_Changed(bool)));
    Ui->Menu_View_Tech_Mfal->setData((int)TechTableWidget::MFAL_COLUMN);
    Ui->Menu_View_Tech_Mfal->setChecked(!HiddenColumns.contains("mfal"));
    connect(Ui->Menu_View_Tech_Mfal, SIGNAL(toggled(bool)), Ui->Tech_Table_Widget, SLOT(On_View_Option_Changed(bool)));
    Ui->Menu_View_Tech_Chan->setData((int)TechTableWidget::CHAN_COLUMN);
    Ui->Menu_View_Tech_Chan->setChecked(!HiddenColumns.contains("chan"));
    connect(Ui->Menu_View_Tech_Chan, SIGNAL(toggled(bool)), Ui->Tech_Table_Widget, SLOT(On_View_Option_Changed(bool)));

    connect(Ui->Tech_Table_Widget, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(Show_Context_Menu(const QPoint&)));

    connect(Ui->Table_Widget, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(Show_Context_Menu(const QPoint&)));

    connect(Ui->Tech_Table_Widget, SIGNAL(Enable_Save()),
            this, SLOT(Enable_Menu_Save_All()));

    connect(Ui->Table_Widget, SIGNAL(Enable_Save()),
            this, SLOT(Enable_Menu_Save_All()));

    Ui->Tech_Table_Widget->Update_Table();
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

    Ui->Tech_Table_Widget->Update_Table();
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

    Ui->Tech_Table_Widget->Update_Table();
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

    Ui->Tech_Table_Widget->Update_Table();
    Ui->Table_Widget->Update_Table();
}

//---------------------------------------------------------------------------
void MainWindow::on_Menu_File_Save_All_triggered()
{
    FileList* Files = C->Get_Files();

    for(FileList::iterator It = Files->begin(); It != Files->end(); It++)
    {
        if(It->Modified())
        {
            It->Valid = false; //Done for not writing again the same file
            C->Save_File(It.key());
        }
    }

    Ui->Tech_Table_Widget->Update_Table();
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
    QModelIndexList Selected;
    if (Ui->StackedWidget->currentIndex()==0) //TODO: use enum
        Selected = Ui->Tech_Table_Widget->selectionModel()->selectedRows(FILE_COLUMN);
    else if (Ui->StackedWidget->currentIndex()==1) //TODO: use enum
        Selected = Ui->Table_Widget->selectionModel()->selectedRows(FILE_COLUMN);
    for(QModelIndexList::iterator It = Selected.begin(); It != Selected.end(); It++)
    {
        QString FileName = It->data(Qt::EditRole).toString();
        if(!Auto && (*C->Get_Files())[FileName].Modified())
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
    Ui->Tech_Table_Widget->Update_Table();
    Ui->Table_Widget->Update_Table();
}

//---------------------------------------------------------------------------
void MainWindow::on_Menu_File_Close_All_triggered()
{
    FileList* Files = C->Get_Files();

    for(FileList::iterator It = Files->begin(); It != Files->end(); It++)
    {
        if(It->Modified())
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

    Ui->Tech_Table_Widget->Update_Table();
    Ui->Table_Widget->Update_Table();
}

//---------------------------------------------------------------------------
void MainWindow::on_Menu_View_Technical_triggered()
{
    if (Ui->StackedWidget->currentIndex()!=0) //TODO: use enum
    {
        Ui->Menu_View_Tech_Separator->setVisible(true);
        Ui->Menu_View_Tech_Clef->setVisible(true);
        Ui->Menu_View_Tech_Prof->setVisible(true);
        Ui->Menu_View_Tech_Enof->setVisible(true);
        Ui->Menu_View_Tech_Pasp->setVisible(true);
        Ui->Menu_View_Tech_Wscl->setVisible(true);
        Ui->Menu_View_Tech_Fiel->setVisible(true);
        Ui->Menu_View_Tech_Colr->setVisible(true);
        Ui->Menu_View_Tech_Gama->setVisible(true);
        Ui->Menu_View_Tech_Clap->setVisible(true);
        Ui->Menu_View_Tech_Dpri->setVisible(true);
        Ui->Menu_View_Tech_Dlum->setVisible(true);
        Ui->Menu_View_Tech_Mcll->setVisible(true);
        Ui->Menu_View_Tech_Mfal->setVisible(true);
        Ui->Menu_View_Tech_Chan->setVisible(true);

        Ui->StackedWidget->setCurrentIndex(0);
        Ui->Table_Widget->clearSelection();
    }
}

//---------------------------------------------------------------------------
void MainWindow::on_Menu_View_AdId_triggered()
{
    if (Ui->StackedWidget->currentIndex()!=1) //TODO: use enum
    {
        Ui->Menu_View_Tech_Separator->setVisible(false);
        Ui->Menu_View_Tech_Clef->setVisible(false);
        Ui->Menu_View_Tech_Prof->setVisible(false);
        Ui->Menu_View_Tech_Enof->setVisible(false);
        Ui->Menu_View_Tech_Pasp->setVisible(false);
        Ui->Menu_View_Tech_Wscl->setVisible(false);
        Ui->Menu_View_Tech_Fiel->setVisible(false);
        Ui->Menu_View_Tech_Colr->setVisible(false);
        Ui->Menu_View_Tech_Gama->setVisible(false);
        Ui->Menu_View_Tech_Clap->setVisible(false);
        Ui->Menu_View_Tech_Dpri->setVisible(false);
        Ui->Menu_View_Tech_Dlum->setVisible(false);
        Ui->Menu_View_Tech_Mcll->setVisible(false);
        Ui->Menu_View_Tech_Mfal->setVisible(false);
        Ui->Menu_View_Tech_Chan->setVisible(false);

        Ui->StackedWidget->setCurrentIndex(1);
        Ui->Tech_Table_Widget->clearSelection();
    }
}

//---------------------------------------------------------------------------
void MainWindow::on_Tech_Table_Widget_itemSelectionChanged()
{
    if(Ui->Tech_Table_Widget->selectedItems().size())
        Ui->Menu_File_Close->setEnabled(true);
    else
        Ui->Menu_File_Close->setEnabled(false);
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
void MainWindow::Enable_Menu_Save_All()
{
    if (!C)
        return;

    bool Enable=false;
    for (FileList::iterator It=C->Get_Files()->begin(); It!= C->Get_Files()->end(); It++)
        Enable|=It.value().Modified();

    Ui->Menu_File_Save_All->setEnabled(Enable);
}

//---------------------------------------------------------------------------
void MainWindow::on_Menu_New_Version_triggered()
{
    QDesktopServices::openUrl(QUrl("https://mediaarea.net/MOVMetaEdit"));
}

//---------------------------------------------------------------------------
void MainWindow::Show_Context_Menu(const QPoint& Pos)
{
    if (Ui->StackedWidget->currentIndex()==0) //TODO: use enum
        Context_Menu->exec(Ui->Tech_Table_Widget->mapToGlobal(Pos));
    else if (Ui->StackedWidget->currentIndex()==1) //TODO: use enum
        Context_Menu->exec(Ui->Table_Widget->mapToGlobal(Pos));
}

//---------------------------------------------------------------------------
void MainWindow::Tech_Table_Widget_Changed()
{
    if(Ui->Tech_Table_Widget->rowCount())
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
