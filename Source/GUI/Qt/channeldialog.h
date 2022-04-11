/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#ifndef CHANNELDIALOG_H
#define CHANNELDIALOG_H

#include <QDialog>
#include <QString>

//***************************************************************************
// Dialog
//***************************************************************************

namespace Ui {
class ChannelDialog;
}

class ChannelDialog : public QDialog
{
    Q_OBJECT

public:
    ChannelDialog(QString Channel, QWidget* Parent=0);
    ~ChannelDialog();

    QString Data() { return Channels; };

private slots:

private:
    Ui::ChannelDialog* Ui;
    QString Channels;
};

#endif // CHANNELDIALOG_H
