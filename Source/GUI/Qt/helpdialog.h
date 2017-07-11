/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QUrl>

namespace Ui {
class HelpDialog;
}

class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpDialog(QWidget* Parent = 0);
    ~HelpDialog();

private slots:
    void on_Text1_anchorClicked(const QUrl& Link);

private:
    Ui::HelpDialog* Ui;
};

#endif // HELPDIALOG_H
