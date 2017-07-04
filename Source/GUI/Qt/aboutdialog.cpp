#include <QAbstractTextDocumentLayout>

#include "config.h"
#include "aboutdialog.h"
#include "ui_aboutdialog.h"

//---------------------------------------------------------------------------
AboutDialog::AboutDialog(QWidget* Parent) : QDialog(Parent), Ui(new Ui::AboutDialog)
{
    Ui->setupUi(this);

    Ui->NameVersion->insertHtml(NameVersion_HTML());
    Ui->NameVersion->setMaximumHeight((int)
        Ui->NameVersion->document()->documentLayout()->documentSize().height() + 6);

    Ui->Text->setHtml(Description_HTML());
    Ui->Button_Description->setDisabled(true);
    Ui->Button_AuthorLicence->setDisabled(false);
    Ui->Button_Contact->setDisabled(false);
}

//---------------------------------------------------------------------------
AboutDialog::~AboutDialog()
{
    delete Ui;
}

//---------------------------------------------------------------------------
const char* AboutDialog::NameVersion_HTML()
{
    return "<a href=\"https://www.mediaarea.net\">MediaArea</a> MOV MetaEdit version " VERSION;
}

//---------------------------------------------------------------------------
const char* AboutDialog::Description_HTML()
{
    return "<H3>DESCRIPTION</H3>"
           ""
           "TODO";
}

//---------------------------------------------------------------------------
const char* AboutDialog::AuthorLicense_HTML()
{
    return "<h3>AUTHOR and LICENSE</h3>"
    ""
    "TODO";
}

//---------------------------------------------------------------------------
const char*AboutDialog:: Contact_HTML()
{
    return "<h3>CONTACT</h3>"
    ""
    "TODO";
}

//---------------------------------------------------------------------------
void AboutDialog::on_Button_Description_clicked()
{
    Ui->Text->setHtml(Description_HTML());

    Ui->Button_Description->setDisabled(true);
    Ui->Button_AuthorLicence->setDisabled(false);
    Ui->Button_Contact->setDisabled(false);
}

//---------------------------------------------------------------------------
void AboutDialog::on_Button_AuthorLicence_clicked()
{
    Ui->Text->setHtml(AuthorLicense_HTML());

    Ui->Button_Description->setDisabled(false);
    Ui->Button_AuthorLicence->setDisabled(true);
    Ui->Button_Contact->setDisabled(false);
}

//---------------------------------------------------------------------------
void AboutDialog::on_Button_Contact_clicked()
{
    Ui->Text->setHtml(Contact_HTML());

    Ui->Button_Description->setDisabled(false);
    Ui->Button_AuthorLicence->setDisabled(false);
    Ui->Button_Contact->setDisabled(true);
}
