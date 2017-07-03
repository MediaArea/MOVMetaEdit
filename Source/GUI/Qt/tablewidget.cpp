#include <QResizeEvent>
#include <QScrollBar>
#include <QFont>
#include <QDebug>

#include "tablewidget.h"

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
        if(!Acceptable.contains(Input.at(Pos)))
            return QValidator::Invalid;

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

    return QValidator::Acceptable;
}

//---------------------------------------------------------------------------
ComboBoxDelegate::ComboBoxDelegate(QObject* Parent, Core* C) : QItemDelegate(Parent), C(C)
{
}

//---------------------------------------------------------------------------
QWidget* ComboBoxDelegate::createEditor(QWidget* Parent,
                                        const QStyleOptionViewItem& Option,
                                        const QModelIndex& Index) const
{
    Q_UNUSED(Option)
    Q_UNUSED(Index)

    QComboBox* ComboBox = new QComboBox(Parent);

    ComboBox->setEditable(true);
    ComboBox->setInsertPolicy(QComboBox::InsertAtBottom);

    return ComboBox;
}

//---------------------------------------------------------------------------
void ComboBoxDelegate::updateEditorGeometry(QWidget* Editor,
                                            const QStyleOptionViewItem& Option,
                                            const QModelIndex& Index) const
{
    Q_UNUSED(Index)

    Editor->setGeometry(Option.rect);
}

//---------------------------------------------------------------------------
void ComboBoxDelegate::setEditorData(QWidget* Editor, const QModelIndex& Index) const
{
    if(!C)
        return;

    QComboBox* ComboBox = qobject_cast<QComboBox*>(Editor);

    QString FileName = Index.sibling(Index.row(), FILE_COLUMN).data().toString();

    MetaDataList* MetaData = C->Get_MetaData(FileName);

    for(MetaDataList::const_iterator It = MetaData->begin(); It != MetaData->end(); It++)
    {
        QString Registry = It.key();
        QString Value = It.value();

        ComboBox->addItem(Registry, Value);
    }
}

//---------------------------------------------------------------------------
void ComboBoxDelegate::setModelData(QWidget* Editor,
                                    QAbstractItemModel* Model,
                                    const QModelIndex& Index) const
{
    if(!C)
        return;

    QComboBox* ComboBox = qobject_cast<QComboBox*>(Editor);

    QString FileName = Index.sibling(Index.row(), FILE_COLUMN).data().toString();
    QString Registry = ComboBox->currentText();

    MetaDataList* MetaData = C->Get_MetaData(FileName);

    QString Value = (*MetaData)[Registry];

    Model->setData(Index, Registry);
    Model->setData(Index.sibling(Index.row(), VALUE_COLUMN), Value);
}

//---------------------------------------------------------------------------
ItemDelegate::ItemDelegate(QObject* Parent, Core* C) : QItemDelegate(Parent), C(C)
{
}

//---------------------------------------------------------------------------
QWidget* ItemDelegate::createEditor(QWidget* Parent,
                                    const QStyleOptionViewItem& Option,
                                    const QModelIndex& Index) const
{
    Q_UNUSED(Option)

    QLineEdit *Editor = new QLineEdit(Parent);

    if(Index.sibling(Index.row(), REGISTRY_COLUMN).data(Qt::EditRole).toString() == "ad-id.org")
        Editor->setValidator(new AdIdValidator);
    else
        Editor->setValidator(new OtherValidator);

    return Editor;
}

//---------------------------------------------------------------------------
void ItemDelegate::updateEditorGeometry(QWidget* Editor,
                                        const QStyleOptionViewItem& Option,
                                        const QModelIndex& Index) const
{
    Q_UNUSED(Index)

    Editor->setGeometry(Option.rect);
}


//---------------------------------------------------------------------------
void ItemDelegate::setEditorData(QWidget *Editor, const QModelIndex& Index) const
{
    qobject_cast<QLineEdit*>(Editor)->setText(Index.data(Qt::EditRole).toString());
}

//---------------------------------------------------------------------------
void ItemDelegate::setModelData(QWidget* Editor,
                                QAbstractItemModel* Model,
                                const QModelIndex& Index) const
{
    if(!C)
        return;

    QLineEdit* LineEditor = qobject_cast<QLineEdit*>(Editor);

    QString OldValue = Model->data(Index, Qt::EditRole).toString();
    QString Value = LineEditor->text();

    if(Value != OldValue)
    {
        QString FileName = Index.sibling(Index.row(), FILE_COLUMN).data(Qt::EditRole).toString();
        QString Registry = Index.sibling(Index.row(), REGISTRY_COLUMN).data(Qt::EditRole).toString();
        MetaDataList* MetaData = C->Get_MetaData(FileName);

        if(!MetaData)
            return;

        MetaData->insert(Registry, Value);
        Model->setData(Index, Value);

        (*C->Get_Files())[FileName].Modified = true;
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

    Header_Labels.append("File Name");
    Header_Labels.append("Registry");
    Header_Labels.append("Value");
    setColumnCount(3);
    setHorizontalHeaderLabels(Header_Labels);
#if QT_VERSION < 0x050000
    horizontalHeader()->setResizeMode(QHeaderView::Interactive);
#else
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
#endif

    //TODO: load and save user preference
    setColumnWidth(0, width() * 60 / 100);
    setColumnWidth(1, width() * 20 / 100);
    setColumnWidth(2, width() * 20 / 100);

    ItemDelegate* ItemEditor = new ItemDelegate(NULL, C);
    connect(ItemEditor, SIGNAL(Value_Changed(int)), this, SLOT(On_Value_Changed(int)));

    setItemDelegateForColumn(1, qobject_cast<QAbstractItemDelegate*>(new ComboBoxDelegate(NULL, C)));
    setItemDelegateForColumn(2, qobject_cast<QAbstractItemDelegate*>(ItemEditor));
}

//---------------------------------------------------------------------------
void TableWidget::Set_Modified(int Row, bool Modified)
{
    if(Row > this->rowCount())
        return;

    for(int Col = 0; Col < this->columnCount(); Col++)
    {
        QFont Font = this->item(Row, Col)->font();
        Font.setBold(Modified);
        this->item(Row, Col)->setFont(Font);
    }
}

//---------------------------------------------------------------------------
void TableWidget::Update_Table()
{
    //Get opened files
    FileList* Files = C->Get_Files();

    //Get displayed entries
    QStringList Entries;
    for(int Row = rowCount() - 1; Row >= 0; Row--)
    {
        QString Entry = item(Row, 0)->data(Qt::DisplayRole).toString();

        if(Files->find(Entry) == Files->end())
        {
            //Remove deleted entry
            removeRow(Row);
            continue;
        }

        Entries.append(Entry);
        //Display modified entries in bold
        Set_Modified(Row, Files->value(Entry).Modified);
    }

    //Display new files
    for(FileList::iterator It = Files->begin(); It != Files->end(); It++)
    {
        if(!Entries.contains(It.key()))
        {
            QTableWidgetItem* Item = new QTableWidgetItem(It.key());
            Item->setFlags(Item->flags() ^ Qt::ItemIsEditable);

            insertRow(rowCount());
            setItem(rowCount() - 1, 0, Item);

            setItem(rowCount() - 1, 1, new QTableWidgetItem("ad-id.org"));
            setItem(rowCount() - 1, 2, new QTableWidgetItem(It->MetaData["ad-id.org"]));

            Set_Modified(rowCount() - 1, It->Modified);
        }
    }
}

//---------------------------------------------------------------------------
void TableWidget::resizeEvent(QResizeEvent* Event)
{
    //Do nothing if columns size exceed aviable space
    if(!horizontalScrollBar()->isVisible())
    {
        qreal Total_New = Event->size().width();
        //Get the current space occupied by each column
        qreal Total_Old = columnWidth(0) + columnWidth(1) + columnWidth(2);
        qreal Col0_Percent = columnWidth(0) * 100 / Total_Old;
        qreal Col1_Percent = columnWidth(1) * 100 / Total_Old;
        qreal Col2_Percent = columnWidth(1) * 100 / Total_Old;

        setColumnWidth(0, Total_New * Col0_Percent / 100);
        setColumnWidth(1, Total_New * Col1_Percent / 100);
        setColumnWidth(2, Total_New * Col2_Percent / 100);
    }
    //Call base resizeEvent to handle the vertical resizing
    QTableView::resizeEvent(Event);
}

//---------------------------------------------------------------------------
void TableWidget::On_Value_Changed(int Row)
{
    Set_Modified(Row, true);
}
