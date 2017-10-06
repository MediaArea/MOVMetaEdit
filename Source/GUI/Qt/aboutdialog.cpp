/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include <QAbstractTextDocumentLayout>
#include <QDesktopServices>

#include "config.h"
#include "aboutdialog.h"
#include "ui_aboutdialog.h"

//---------------------------------------------------------------------------
AboutDialog::AboutDialog(QWidget* Parent) : QDialog(Parent), Ui(new Ui::AboutDialog)
{
    Ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    Ui->NameVersion->insertHtml(NameVersion_HTML());
    Ui->NameVersion->setMaximumHeight((int)
        Ui->NameVersion->document()->documentLayout()->documentSize().height() + 6);

    on_Button_Description_clicked();
}

//---------------------------------------------------------------------------
AboutDialog::~AboutDialog()
{
    delete Ui;
}

//---------------------------------------------------------------------------
const char* AboutDialog::NameVersion_HTML()
{
    return "MOV MetaEdit " VERSION;
}

//---------------------------------------------------------------------------
void AboutDialog::on_Button_Description_clicked()
{
    Ui->Text->setSource(QUrl("qrc:/Documentation/Description.html"));

    Ui->Button_Description->setDisabled(true);
    Ui->Button_AuthorLicence->setDisabled(false);
    Ui->Button_Contact->setDisabled(false);
}

//---------------------------------------------------------------------------
void AboutDialog::on_Button_AuthorLicence_clicked()
{
    Ui->Text->setSource(QUrl("qrc:/License.html"));

    Ui->Button_Description->setDisabled(false);
    Ui->Button_AuthorLicence->setDisabled(true);
    Ui->Button_Contact->setDisabled(false);
}

//---------------------------------------------------------------------------
void AboutDialog::on_Button_Contact_clicked()
{
    Ui->Text->setSource(QUrl("qrc:/Documentation/Contact.html"));

    Ui->Button_Description->setDisabled(false);
    Ui->Button_AuthorLicence->setDisabled(false);
    Ui->Button_Contact->setDisabled(true);
}

//---------------------------------------------------------------------------
void AboutDialog::on_Sponsor1_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.ad-id.org"));
}
