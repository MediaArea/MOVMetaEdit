#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget* Parent = 0);
    ~AboutDialog();

private slots:
    void on_Button_Description_clicked();
    void on_Button_AuthorLicence_clicked();
    void on_Button_Contact_clicked();
    void on_Sponsor1_clicked();

private:
    Ui::AboutDialog* Ui;

    const char* NameVersion_HTML();
};

#endif // ABOUTDIALOG_H
