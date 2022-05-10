/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#pragma once

#include <QItemDelegate>
#include <QTableWidget>
#include <QDialog>
#include <QString>

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
class ChannelDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    ChannelDelegate(QObject* Parent=0);

protected:
    QWidget* createEditor(QWidget* Parent,
                          const QStyleOptionViewItem& Option,
                          const QModelIndex& Index) const;

    void updateEditorGeometry(QWidget* Editor,
                              const QStyleOptionViewItem& Option,
                              const QModelIndex& Index) const;

    void setEditorData(QWidget* Editor, const QModelIndex& Index) const;
    void setModelData(QWidget* Editor, QAbstractItemModel* Model, const QModelIndex& Index) const;

private:

signals:
    void Value_Changed(int Row) const;
};

//***************************************************************************
// TableWidget
//***************************************************************************

//---------------------------------------------------------------------------
class ChanTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    enum COLUMNS
    {
        DESC_COLUMN,
        MAX_COLUMN,
    };

    ChanTableWidget(QWidget* Parent);

    void Setup(QString* Channels);
    void Update_Table();

    void resizeEvent(QResizeEvent* Event);

protected slots:
    void On_Value_Changed(int Row);

private:
    QString  Previous;
    QString* Channels;
    int ColumnSize[MAX_COLUMN+1];
    float ColumnSize_Ratio[MAX_COLUMN+1];
};
