/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include <QResizeEvent>
#include <QScrollBar>
#include <QHeaderView>
#include <QStringList>
#include <QComboBox>

#include "Common/config.h"
#include "Common/mp4_Handler.h"
#include "chantablewidget.h"

//***************************************************************************
// Info
//***************************************************************************

//---------------------------------------------------------------------------
static const char* ColumnName_Default[ChanTableWidget::MAX_COLUMN] =
{
    "Description",
};

//---------------------------------------------------------------------------
static const int ColumnSize_Default[ChanTableWidget::MAX_COLUMN] =
{
    70,
};

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
ChannelDelegate::ChannelDelegate(QObject* Parent) : QItemDelegate(Parent)
{
}

//---------------------------------------------------------------------------
QWidget* ChannelDelegate::createEditor(QWidget* Parent,
                                       const QStyleOptionViewItem& Option,
                                       const QModelIndex& Index) const
{
    Q_UNUSED(Option)
    Q_UNUSED(Index)

    QComboBox *Editor=new QComboBox(Parent);
    Editor->setEditable(true);
    Editor->setValidator(new QIntValidator(0, 0xffffffff));

    return Editor;
}

//---------------------------------------------------------------------------
void ChannelDelegate::updateEditorGeometry(QWidget* Editor,
                                            const QStyleOptionViewItem& Option,
                                            const QModelIndex& Index) const
{
    Q_UNUSED(Index)

    Editor->setGeometry(Option.rect);
}

//---------------------------------------------------------------------------
void ChannelDelegate::setEditorData(QWidget* Editor, const QModelIndex& Index) const
{
    QComboBox* ChannelBox=qobject_cast<QComboBox*>(Editor);
    ChannelBox->clear();

    if (Index.data(Qt::UserRole).toString()=="ABSENT")
        ChannelBox->addItem("", "ABSENT");
    else if (Index.data(Qt::UserRole).toString()=="MULTIPLES")
        ChannelBox->addItem("(Multiples)", "MULTIPLES");
    else if (Index.data(Qt::UserRole).toString()=="NODESCRIPTION")
        ChannelBox->addItem("(No description)", "NODESCRIPTION");
    ChannelBox->addItem(QString("0 - ")+mp4_chan_ChannelDescription(0).c_str(), "0");
    ChannelBox->addItem(QString("1 - ")+mp4_chan_ChannelDescription(1).c_str(), "1");
    ChannelBox->addItem(QString("2 - ")+mp4_chan_ChannelDescription(2).c_str(), "2");
    ChannelBox->addItem(QString("3 - ")+mp4_chan_ChannelDescription(3).c_str(), "3");
    ChannelBox->addItem(QString("4 - ")+mp4_chan_ChannelDescription(4).c_str(), "4");
    ChannelBox->addItem(QString("5 - ")+mp4_chan_ChannelDescription(5).c_str(), "5");
    ChannelBox->addItem(QString("6 - ")+mp4_chan_ChannelDescription(6).c_str(), "6");
    ChannelBox->addItem(QString("7 - ")+mp4_chan_ChannelDescription(7).c_str(), "7");
    ChannelBox->addItem(QString("8 - ")+mp4_chan_ChannelDescription(8).c_str(), "8");
    ChannelBox->addItem(QString("9 - ")+mp4_chan_ChannelDescription(9).c_str(), "9");
    ChannelBox->addItem(QString("10 - ")+mp4_chan_ChannelDescription(10).c_str(), "10");
    ChannelBox->addItem(QString("11 - ")+mp4_chan_ChannelDescription(11).c_str(), "11");
    ChannelBox->addItem(QString("12 - ")+mp4_chan_ChannelDescription(12).c_str(), "12");
    ChannelBox->addItem(QString("13 - ")+mp4_chan_ChannelDescription(13).c_str(), "13");
    ChannelBox->addItem(QString("14 - ")+mp4_chan_ChannelDescription(14).c_str(), "14");
    ChannelBox->addItem(QString("15 - ")+mp4_chan_ChannelDescription(15).c_str(), "15");
    ChannelBox->addItem(QString("16 - ")+mp4_chan_ChannelDescription(16).c_str(), "16");
    ChannelBox->addItem(QString("17 - ")+mp4_chan_ChannelDescription(17).c_str(), "17");
    ChannelBox->addItem(QString("18 - ")+mp4_chan_ChannelDescription(18).c_str(), "18");
    ChannelBox->addItem(QString("33 - ")+mp4_chan_ChannelDescription(33).c_str(), "33");
    ChannelBox->addItem(QString("34 - ")+mp4_chan_ChannelDescription(34).c_str(), "34");
    ChannelBox->addItem(QString("35 - ")+mp4_chan_ChannelDescription(35).c_str(), "35");
    ChannelBox->addItem(QString("36 - ")+mp4_chan_ChannelDescription(36).c_str(), "36");
    ChannelBox->addItem(QString("37 - ")+mp4_chan_ChannelDescription(37).c_str(), "37");
    ChannelBox->addItem(QString("38 - ")+mp4_chan_ChannelDescription(38).c_str(), "38");
    ChannelBox->addItem(QString("39 - ")+mp4_chan_ChannelDescription(39).c_str(), "39");
    ChannelBox->addItem(QString("40 - ")+mp4_chan_ChannelDescription(40).c_str(), "40");
    ChannelBox->addItem(QString("41 - ")+mp4_chan_ChannelDescription(41).c_str(), "41");
    ChannelBox->addItem(QString("42 - ")+mp4_chan_ChannelDescription(42).c_str(), "42");
    ChannelBox->addItem(QString("43 - ")+mp4_chan_ChannelDescription(43).c_str(), "43");
    ChannelBox->addItem(QString("44 - ")+mp4_chan_ChannelDescription(44).c_str(), "44");
    ChannelBox->addItem(QString("45 - ")+mp4_chan_ChannelDescription(45).c_str(), "45");
    ChannelBox->addItem(QString("200 - ")+mp4_chan_ChannelDescription(200).c_str(), "200");
    ChannelBox->addItem(QString("201 - ")+mp4_chan_ChannelDescription(201).c_str(), "201");
    ChannelBox->addItem(QString("202 - ")+mp4_chan_ChannelDescription(202).c_str(), "202");
    ChannelBox->addItem(QString("203 - ")+mp4_chan_ChannelDescription(203).c_str(), "203");
    ChannelBox->addItem(QString("204 - ")+mp4_chan_ChannelDescription(204).c_str(), "204");
    ChannelBox->addItem(QString("205 - ")+mp4_chan_ChannelDescription(205).c_str(), "205");
    ChannelBox->addItem(QString("206 - ")+mp4_chan_ChannelDescription(206).c_str(), "206");
    ChannelBox->addItem(QString("207 - ")+mp4_chan_ChannelDescription(207).c_str(), "207");
    ChannelBox->addItem(QString("301 - ")+mp4_chan_ChannelDescription(301).c_str(), "301");
    ChannelBox->addItem(QString("302 - ")+mp4_chan_ChannelDescription(302).c_str(), "302");
    ChannelBox->addItem(QString("304 - ")+mp4_chan_ChannelDescription(304).c_str(), "304");
    ChannelBox->addItem(QString("305 - ")+mp4_chan_ChannelDescription(305).c_str(), "305");
    ChannelBox->addItem(QString("400 - ")+mp4_chan_ChannelDescription(400).c_str(), "400");

    if (ChannelBox->findData(Index.data(Qt::UserRole).toString())==-1)
        ChannelBox->addItem(Index.data(Qt::UserRole).toString(), Index.data(Qt::UserRole).toString());

    if (Index.data(Qt::UserRole).toString()!="ABSENT")
    {
        ChannelBox->insertSeparator(ChannelBox->count());
        ChannelBox->addItem("Delete", "DELETE");
    }

    ChannelBox->setCurrentIndex(ChannelBox->findData(Index.data(Qt::UserRole).toString()));
}

//---------------------------------------------------------------------------
void ChannelDelegate::setModelData(QWidget* Editor,
                                   QAbstractItemModel* Model,
                                   const QModelIndex& Index) const
{
    QComboBox* ChannelBox=qobject_cast<QComboBox*>(Editor);

    QString OldValue = Model->data(Index, Qt::UserRole).toString();

    bool Ok=false;
    QString Value=ChannelBox->currentText();
    Value.toUInt(&Ok);
    if (!Ok)
        Value=ChannelBox->currentData().toString();

    if(Value!=OldValue)
    {
       Model->setData(Index, Value, Qt::UserRole);
        emit Value_Changed(Index.row());
    }
}

//***************************************************************************
// TableWidget
//***************************************************************************

//---------------------------------------------------------------------------
ChanTableWidget::ChanTableWidget(QWidget* Parent) : QTableWidget(Parent), Channels(NULL)
{
}

//---------------------------------------------------------------------------
void ChanTableWidget::Setup(QString* Channels_)
{
    Previous=*Channels_;
    Channels=Channels_;

    // Setup table widget
    QStringList HLabels;

    for (int i = 0; i < MAX_COLUMN; i++)
    {
        HLabels.append(ColumnName_Default[i]);
        ColumnSize[i] = 0;
    }
    setColumnCount(MAX_COLUMN);
    setHorizontalHeaderLabels(HLabels);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    ChannelDelegate* ChannelEditor = new ChannelDelegate(this);
    connect(ChannelEditor, SIGNAL(Value_Changed(int)), this, SLOT(On_Value_Changed(int)));
    setItemDelegateForColumn(DESC_COLUMN, qobject_cast<QAbstractItemDelegate*>(ChannelEditor));

    Update_Table();
}

//---------------------------------------------------------------------------
void ChanTableWidget::Update_Table()
{
    //Get audio tracks
    QStringList Tracks=Channels->split(",");
    QStringList OldTracks=Previous.split(",");

    //Display
    for(int Pos=0; Pos<Tracks.size(); Pos++)
    {
        int Row=Pos;
        if(Row==rowCount())
            insertRow(rowCount());


        QStringList Desc=Tracks[Pos].split("=");
        if (Desc.size()!=2)
            continue;

        QString Title;
        if (Desc[1]=="ABSENT")
            Title="";
        else if(Desc[1]=="DELETE")
            Title=("(Delete)");
        else if(Desc[1]=="MULTIPLES")
            Title=("(Multiples)");
        else if(Desc[1]=="NODESCRIPTION")
            Title=("(No description)");
        else
        {
            bool Ok=false;
            QString Label = QString(mp4_chan_ChannelDescription(Desc[1].toUInt()).c_str());
            Label.toUInt(&Ok);
            if (Ok)
                Title = Label;
            else
                Title=Desc[1]+" - "+ Label;
        }

        QTableWidgetItem* Value=new QTableWidgetItem(Title);
        Value->setData(Qt::UserRole, Desc[1]);

        if (Pos<OldTracks.size())
        {
            QStringList OldDesc=OldTracks[Pos].split("=");
            if (OldDesc.size()==2 && OldDesc[1]!=Desc[1])
            {
                QFont Font=item(Row, DESC_COLUMN)->font();
                Font.setBold(true);
                item(Row, DESC_COLUMN)->setFont(Font);
            }
        }

        setItem(Row, DESC_COLUMN, Value);
    }
}

//---------------------------------------------------------------------------
void ChanTableWidget::resizeEvent(QResizeEvent* Event)
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
void ChanTableWidget::On_Value_Changed(int Row)
{
    QStringList Tracks=Channels->split(",");
    if (Row>=Tracks.size())
        return;

    QStringList Desc=Tracks[Row].split("=");
    if (Desc.size()!=2)
        return;

    Desc[1]=item(Row, DESC_COLUMN)->data(Qt::UserRole).toString();

    Tracks[Row]=Desc.join("=");
    *Channels=Tracks.join(",");

    Update_Table();
}
