/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QApplication>

#include "Common/config.h"
#include "Common/mp4_Handler.h"
#include "channeldialog.h"
#include "chantablewidget.h"
#include "ui_channeldialog.h"


//***************************************************************************
// ChannelDialog
//***************************************************************************

//---------------------------------------------------------------------------
ChannelDialog::ChannelDialog(QString Channels_, QWidget* Parent) : QDialog(Parent), Channels(Channels_), Ui(new Ui::ChannelDialog)
{
    Ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    move(QApplication::desktop()->screenGeometry().width() / 10,
        QApplication::desktop()->screenGeometry().height() / 10);

    Ui->Channel_Table->Setup(&Channels);
}

//---------------------------------------------------------------------------
ChannelDialog::~ChannelDialog()
{
    delete Ui;
}
