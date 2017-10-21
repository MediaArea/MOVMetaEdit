/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include <QResizeEvent>
#include <QScrollBar>
#include <QFont>
#include <QDebug>

#include "tablewidget.h"

//***************************************************************************
// Info
//***************************************************************************

static const char* ColumnName_Default[MAX_COLUMN] =
{
    "File Name",
    "Registry",
    "Value",
};

static const int ColumnSize_Default[MAX_COLUMN] =
{
    60,
    20,
    20,
};

//***************************************************************************
// Helpers
//***************************************************************************


//---------------------------------------------------------------------------
AdIdValidator::AdIdValidator(QObject* Parent) : QValidator(Parent)
{
}

//---------------------------------------------------------------------------
QValidator::State AdIdValidator::validate(QString& Input, int& Pos) const
{
    Q_UNUSED(Pos)

    if(Input.size() > 12)
        return QValidator::Invalid;

    QString Acceptable = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    for(int Pos = 0; Pos < Input.size(); Pos++)
    {
        if (Input[Pos] >= 'a' && Input[Pos] <= 'z')
            Input[Pos] = Input[Pos].toUpper();

        if(!Acceptable.contains(Input.at(Pos)))
            return QValidator::Invalid;
    }

    if(Input.size() < 11)
        return QValidator::Intermediate;

    return QValidator::Acceptable;
}

//---------------------------------------------------------------------------
OtherValidator::OtherValidator(QObject* Parent) : QValidator(Parent)
{
}

//---------------------------------------------------------------------------
QValidator::State OtherValidator::validate(QString& Input, int& Pos) const
{
    Q_UNUSED(Pos)

    if(Input.size() > 255)
        return QValidator::Invalid;

    if(Input.isEmpty())
        return QValidator::Intermediate;

    return QValidator::Acceptable;
}

//---------------------------------------------------------------------------
NoneValidator::NoneValidator(QObject* Parent) : QValidator(Parent)
{
}

//---------------------------------------------------------------------------
QValidator::State NoneValidator::validate(QString& Input, int& Pos) const
{
    Q_UNUSED(Pos)

    if(!Input.isEmpty())
        return QValidator::Invalid;

    return QValidator::Acceptable;
}

//---------------------------------------------------------------------------
RegistryDelegate::RegistryDelegate(QObject* Parent, Core* C) : QItemDelegate(Parent), C(C)
{
}

//---------------------------------------------------------------------------
QWidget* RegistryDelegate::createEditor(QWidget* Parent,
                                        const QStyleOptionViewItem& Option,
                                        const QModelIndex& Index) const
{
    Q_UNUSED(Option)
    Q_UNUSED(Index)

    QLineEdit *Editor = new QLineEdit(Parent);

    return Editor;
}

//---------------------------------------------------------------------------
void RegistryDelegate::updateEditorGeometry(QWidget* Editor,
                                            const QStyleOptionViewItem& Option,
                                            const QModelIndex& Index) const
{
    Q_UNUSED(Index)

    Editor->setGeometry(Option.rect);
}

//---------------------------------------------------------------------------
void RegistryDelegate::setEditorData(QWidget* Editor, const QModelIndex& Index) const
{
    qobject_cast<QLineEdit*>(Editor)->setText(Index.data(Qt::EditRole).toString());
}

//---------------------------------------------------------------------------
void RegistryDelegate::setModelData(QWidget* Editor,
                                    QAbstractItemModel* Model,
                                    const QModelIndex& Index) const
{
    QLineEdit* LineEditor = qobject_cast<QLineEdit*>(Editor);

    QString OldValue = Model->data(Index, Qt::EditRole).toString();
    QString Value = LineEditor->text();

    if(Value != OldValue)
    {
        Model->setData(Index, Value);
        emit Value_Changed(Index.row());
    }
}

//---------------------------------------------------------------------------
ValueDelegate::ValueDelegate(QObject* Parent, Core* C) : QItemDelegate(Parent), C(C)
{
}

//---------------------------------------------------------------------------
QWidget* ValueDelegate::createEditor(QWidget* Parent,
                                    const QStyleOptionViewItem& Option,
                                    const QModelIndex& Index) const
{
    Q_UNUSED(Option)
    Q_UNUSED(Index)

    QLineEdit *Editor = new QLineEdit(Parent);

    return Editor;
}

//---------------------------------------------------------------------------
void ValueDelegate::updateEditorGeometry(QWidget* Editor,
                                        const QStyleOptionViewItem& Option,
                                        const QModelIndex& Index) const
{
    Q_UNUSED(Index)

    Editor->setGeometry(Option.rect);
}


//---------------------------------------------------------------------------
void ValueDelegate::setEditorData(QWidget *Editor, const QModelIndex& Index) const
{
    qobject_cast<QLineEdit*>(Editor)->setText(Index.data(Qt::EditRole).toString());
}

//---------------------------------------------------------------------------
void ValueDelegate::setModelData(QWidget* Editor,
                                QAbstractItemModel* Model,
                                const QModelIndex& Index) const
{
    QLineEdit* LineEditor = qobject_cast<QLineEdit*>(Editor);

    QString OldValue = Model->data(Index, Qt::EditRole).toString();
    QString Value = LineEditor->text();

    if(Value != OldValue)
    {
        Model->setData(Index, Value);
        emit Value_Changed(Index.row());
    }
}

//***************************************************************************
// TableWidget
//***************************************************************************

//---------------------------------------------------------------------------
TableWidget::TableWidget(QWidget* Parent) : QTableWidget(Parent)
{
}

//---------------------------------------------------------------------------
void TableWidget::Setup(Core *C)
{
    this->C = C;

    // Setup table widget
    QStringList Header_Labels;

    for (int i = 0; i < MAX_COLUMN; i++)
    {
        Header_Labels.append(ColumnName_Default[i]);
        ColumnSize[i] = 0;
    }
    setColumnCount(MAX_COLUMN);
    setHorizontalHeaderLabels(Header_Labels);
#if QT_VERSION < 0x050000
    horizontalHeader()->setResizeMode(QHeaderView::Interactive);
#else
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
#endif

    ValueDelegate* ValueEditor = new ValueDelegate(NULL, C);
    connect(ValueEditor, SIGNAL(Value_Changed(int)), this, SLOT(On_Value_Changed(int)));

    RegistryDelegate* RegistryEditor = new RegistryDelegate(NULL, C);
    connect(RegistryEditor, SIGNAL(Value_Changed(int)), this, SLOT(On_Value_Changed(int)));

    setItemDelegateForColumn(REGISTRY_COLUMN, qobject_cast<QAbstractItemDelegate*>(RegistryEditor));
    setItemDelegateForColumn(VALUE_COLUMN, qobject_cast<QAbstractItemDelegate*>(ValueEditor));
}

//---------------------------------------------------------------------------
void TableWidget::Set_Display(int Row, bool Valid, bool Modified, bool ValueValid)
{
    if(Row > this->rowCount())
        return;

    if (!Valid)
    {
        for (int Col = 1; Col < this->columnCount(); Col++)
        {
            Qt::ItemFlags Flags = this->item(Row, Col)->flags();
            if (Flags.testFlag(Qt::ItemIsEnabled))
                Flags &= ~Qt::ItemIsEnabled;
            if (Flags.testFlag(Qt::ItemIsSelectable))
                Flags &= ~Qt::ItemIsSelectable;
            item(Row, Col)->setFlags(Flags);
        }
        item(Row, REGISTRY_COLUMN)->setBackgroundColor(QColor(255, 63, 63, 127));
        item(Row, VALUE_COLUMN)->setBackgroundColor(QColor(255, 63, 63, 127));
        return;
    }

    item(Row, REGISTRY_COLUMN)->setBackgroundColor(QColor(173, 216, 230, 127));

    if (!ValueValid)
    {
        for (int Col = 0; Col < this->columnCount(); Col++)
        {
            QFont Font = this->item(Row, Col)->font();
            Font.setBold(false);
            this->item(Row, Col)->setFont(Font);
        }
        item(Row, VALUE_COLUMN)->setBackgroundColor(QColor(230, 173, 173, 127));
        return;
    }

    for(int Col = 0; Col < this->columnCount(); Col++)
    {
        QFont Font = this->item(Row, Col)->font();
        Font.setBold(Modified);
        this->item(Row, Col)->setFont(Font);
    }
    item(Row, VALUE_COLUMN)->setBackgroundColor(QColor(173, 216, 230, 127));
}

//---------------------------------------------------------------------------
void TableWidget::Update_Table()
{
    size_t Valid = 0;
    bool Modified = false;

    //Get opened files
    FileList* Files = C->Get_Files();

    //Remove deleted entry
    for(int Row = rowCount() - 1; Row >= 0; Row--)
    {
        QString Entry = item(Row, FILE_COLUMN)->data(Qt::DisplayRole).toString();
        if(Files->find(Entry) == Files->end())
        {
            removeRow(Row);
            continue;
        }
    }

    //Display new files
    for(FileList::iterator It = Files->begin(); It != Files->end(); It++)
    {
        int Row = rowCount();
        QModelIndexList Matches = model()->match(model()->index(0,0), Qt::DisplayRole, It.key(), -1);

        for(QModelIndexList::iterator It2 = Matches.begin(); It2 != Matches.end(); It2++)
        {
            if(It2->column() == FILE_COLUMN)
            {
                Row = It2->row();
                break;
            }
        }

        if(Row == rowCount())
            insertRow(rowCount());

        QTableWidgetItem* Name = new QTableWidgetItem(It.key());
        Name->setFlags(Name->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem* Registry;
        QTableWidgetItem* Value;
        if (It->Valid)
        {
            Registry = new QTableWidgetItem(It->MetaData.first);
            Registry->setToolTip("Double-click for editing the Universal Ad ID registry of this file.");
            Value = new QTableWidgetItem(It->MetaData.second);
            Value->setToolTip("Double-click for editing the Universal Ad ID value of this file.\nA-Z 0-9 only.");
        }
        else
        {
            Registry = new QTableWidgetItem("(Parsing error)");
            Value = new QTableWidgetItem(It->H->PerFile_Error.str().c_str());
        }

        setItem(Row, FILE_COLUMN, Name);
        setItem(Row, REGISTRY_COLUMN, Registry);
        setItem(Row, VALUE_COLUMN, Value);

        if (It->Modified && It->ValueValid)
            Modified = true;

        if (It->Valid)
            Valid++;

        Set_Display(Row, It->Valid, It->Modified, It->ValueValid);
    }

    if (Valid)
        setStatusTip(QString::number(Valid)+ " editable files found, double-click on Registry or Value cells for editing then save");
    else
        setStatusTip("Drag and drop some MOV/MP4 files");

    emit Enable_Save(Modified);
}

//---------------------------------------------------------------------------
void TableWidget::resizeEvent(QResizeEvent* Event)
{
    //Changed?
    bool c = false;
    bool d = false;
    for (int i = 0; i < MAX_COLUMN; i++)
    {
        if (ColumnSize[i] != columnWidth(i))
            c = true;
        if (!ColumnSize[i])
            d = true;
    }
    if (d)
    {
        //First time
        int ColumnSize_Default_Total=0;
        for (int i = 0; i < MAX_COLUMN; i++)
            ColumnSize_Default_Total += ColumnSize_Default[i];
        for (int i = 0; i < MAX_COLUMN; i++)
            ColumnSize_Ratio[i] = ((float)ColumnSize_Default[i]) / ColumnSize_Default_Total;
        ColumnSize_Ratio[MAX_COLUMN] = 0;
    }
    else if (c)
    {
        // With small widths, update was having weird behavior due to rounding issues
        float t = 1;
        for (int i = 0; i < MAX_COLUMN; i++)
        {
            ColumnSize_Ratio[i] = ((float)columnWidth(i)) / size().width();
            t -= ColumnSize_Ratio[i];
        }
        ColumnSize_Ratio[MAX_COLUMN] = t;
    }

    //Update
    qreal Total_New = Event->size().width();
    for (int i = 0; i < MAX_COLUMN; i++)
    {
        ColumnSize[i] = (int)(Total_New * ColumnSize_Ratio[i]);
        setColumnWidth(i, ColumnSize[i]);
    }

    //Call base resizeEvent to handle the vertical resizing
    QTableView::resizeEvent(Event);
}

//---------------------------------------------------------------------------
void TableWidget::On_Value_Changed(int Row)
{
    if(!C)
        return;

    QString FileName = item(Row, FILE_COLUMN)->data(Qt::EditRole).toString();
    QString Registry = item(Row, REGISTRY_COLUMN)->data(Qt::EditRole).toString();
    QString Value = item(Row, VALUE_COLUMN)->data(Qt::EditRole).toString();

    MetaDataType* MetaData = C->Get_MetaData(FileName);

        if(!MetaData)
            return;

    (*C->Get_Files())[FileName].Modified = false;
    MetaData->first = Registry;
    MetaData->second = Value;

    QValidator::State State = QValidator::Invalid;
    int Pos = 0;
    if (Registry == "ad-id.org")
        State = AdIdValidator().validate(Value, Pos);
    else if (Registry.isEmpty())
        State = NoneValidator().validate(Value, Pos);
    else
        State = OtherValidator().validate(Value, Pos);

    bool Modified = *MetaData != (*C->Get_Files())[FileName].Previous && State == QValidator::Acceptable;
    (*C->Get_Files())[FileName].Modified = Modified;
    (*C->Get_Files())[FileName].ValueValid = State == QValidator::Acceptable;

    Update_Table();
}
