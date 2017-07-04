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
    else if (Link == QUrl("qrc:/Documentation/interface.html"))
        Ui->Tab_Widget->setCurrentIndex(1);
    else
        QDesktopServices::openUrl(Link);
}
