/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#ifndef TECHTABLEWIDGET_H
#define TECHTABLEWIDGET_H

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

//---------------------------------------------------------------------------
class ApertureValidator : public QValidator
{
    Q_OBJECT
public:
    ApertureValidator(QObject* Parent = 0);
    QValidator::State validate(QString& Input, int& Pos) const;
};

//---------------------------------------------------------------------------
class RatioValidator : public QValidator
{
    Q_OBJECT
public:
    RatioValidator(QObject* Parent = 0);
    QValidator::State validate(QString& Input, int& Pos) const;
};

//---------------------------------------------------------------------------
class CleanApertureValidator : public QValidator
{
    Q_OBJECT
public:
    CleanApertureValidator(QObject* Parent = 0);
    QValidator::State validate(QString& Input, int& Pos) const;
};

//---------------------------------------------------------------------------
class ApertureDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    ApertureDelegate(QObject* Parent = 0, Core* C = 0);

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
class RatioDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    RatioDelegate(QObject* Parent = 0, Core* C = 0);

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
class DoubleDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    DoubleDelegate(QObject* Parent = 0, Core* C = 0);

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
class FieldsDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    FieldsDelegate(Core* C, QObject* Parent = 0);

protected:
    QWidget* createEditor(QWidget* Parent,
                          const QStyleOptionViewItem& Option,
                          const QModelIndex& Index) const;

    void updateEditorGeometry(QWidget* Editor,
                              const QStyleOptionViewItem& Option,
                              const QModelIndex& Index) const;

    void setEditorData(QWidget* Editor, const QModelIndex& Index) const;
    void setModelData(QWidget* Editor, QAbstractItemModel* Model, const QModelIndex& Index) const;

protected slots:
    void On_Fields_Changed(int);
    void On_Changed(int);

private:
    Core* C;

signals:
    void Value_Changed(int Row) const;
};

//---------------------------------------------------------------------------
class ColorDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    ColorDelegate(Core* C, QObject* Parent = 0);

protected:
    QWidget* createEditor(QWidget* Parent,
                          const QStyleOptionViewItem& Option,
                          const QModelIndex& Index) const;

    void updateEditorGeometry(QWidget* Editor,
                              const QStyleOptionViewItem& Option,
                              const QModelIndex& Index) const;

    void setEditorData(QWidget* Editor, const QModelIndex& Index) const;
    void setModelData(QWidget* Editor, QAbstractItemModel* Model, const QModelIndex& Index) const;

protected slots:
    void On_Primaries_Changed(int);
    void On_Color_Changed(int);

private:
    Core* C;

signals:
    void Value_Changed(int Row) const;
};

//---------------------------------------------------------------------------
class CleanApertureDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    CleanApertureDelegate(QObject* Parent = 0, Core* C = 0);

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

class TechTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    enum COLUMNS
    {
        FILE_COLUMN,
        CLEF_COLUMN,
        PROF_COLUMN,
        ENOF_COLUMN,
        PASP_COLUMN,
        WSCL_COLUMN,
        FIEL_COLUMN,
        COLR_COLUMN,
        GAMA_COLUMN,
        CLAP_COLUMN,
        CHAN_COLUMN,
        MAX_COLUMN,
    };


    TechTableWidget(QWidget* Parent);

    void Setup(Core* C);
    void Set_Display(int Row, bool Valid);
    void Update_Table();

    void resizeEvent(QResizeEvent* Event);
    bool edit (const QModelIndex& Index, EditTrigger Trigger, QEvent* Event);

protected slots:
    void On_Value_Changed(int Row);

signals:
    void Enable_Save() const;

private:
    Core* C;
    int ColumnSize[MAX_COLUMN+1];
    float ColumnSize_Ratio[MAX_COLUMN+1];
};

#endif // TECHTABLEWIDGET_H
