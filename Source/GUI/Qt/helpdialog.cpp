/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QApplication>

#include "config.h"
#include "helpdialog.h"
#include "ui_helpdialog.h"

//---------------------------------------------------------------------------
HelpDialog::HelpDialog(QWidget* Parent) : QDialog(Parent), Ui(new Ui::HelpDialog)
{
    Ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    move(QApplication::desktop()->screenGeometry().width() / 10,
        QApplication::desktop()->screenGeometry().height() / 10);
    resize(QApplication::desktop()->screenGeometry().width() -
        QApplication::desktop()->screenGeometry().width() / 10*2,
        QApplication::desktop()->screenGeometry().height() / 2);
}

//---------------------------------------------------------------------------
HelpDialog::~HelpDialog()
{
    delete Ui;
}

void HelpDialog::on_Text1_anchorClicked(const QUrl& Link)
{
    if (Link == QUrl("qrc:/Documentation/index.html"))
        Ui->Tab_Widget->setCurrentIndex(0);
    else if (Link == QUrl("qrc:/Documentation/UniversalAdID.html"))
        Ui->Tab_Widget->setCurrentIndex(1);
    else
        QDesktopServices::openUrl(Link);
}
