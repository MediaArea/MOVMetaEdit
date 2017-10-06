/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QItemDelegate>
#include <QTableWidget>
#include <QHeaderView>
#include <QValidator>
#include <QLineEdit>
#include <QComboBox>

#include "core.h"

//***************************************************************************
// Helpers
//***************************************************************************

enum COLUMNS
{
    FILE_COLUMN,
    REGISTRY_COLUMN,
    VALUE_COLUMN,
    MAX_COLUMN,
};

//---------------------------------------------------------------------------
class AdIdValidator : public QValidator
{
    Q_OBJECT
public:
    AdIdValidator(QObject* Parent = 0);
    QValidator::State validate(QString& Input, int& Pos) const;
};

//---------------------------------------------------------------------------
class OtherValidator : public QValidator
{
    Q_OBJECT
public:
    OtherValidator(QObject* Parent = 0);
    QValidator::State validate(QString& Input, int& Pos) const;
};

//---------------------------------------------------------------------------
class NoneValidator : public QValidator
{
    Q_OBJECT
public:
    NoneValidator(QObject* Parent = 0);
    QValidator::State validate(QString& Input, int& Pos) const;
};

//---------------------------------------------------------------------------
class RegistryDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    RegistryDelegate(QObject* Parent = 0, Core* C = 0);

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
    Core* C;

signals:
    void Value_Changed(int Row) const;
};

//---------------------------------------------------------------------------
class ValueDelegate : public QItemDelegate
{
    Q_OBJECT

public:
     ValueDelegate(QObject* Parent = 0, Core* C = 0);

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
    Core* C;

signals:
    void Value_Changed(int Row) const;
};

//***************************************************************************
// TableWidget
//***************************************************************************

class TableWidget : public QTableWidget
{
    Q_OBJECT

public:
    TableWidget(QWidget* Parent);

    void Setup(Core* C);
    void Set_Display(int Row, bool Valid, bool Modified, bool ValueValid);
    void Update_Table();

    void resizeEvent(QResizeEvent* Event);

protected slots:
    void On_Value_Changed(int Row);

signals:
    void Enable_Save(bool Value) const;

private:
    Core* C;
    int ColumnSize[MAX_COLUMN+1];
    float ColumnSize_Ratio[MAX_COLUMN+1];
};

#endif // TABLEWIDGET_H
