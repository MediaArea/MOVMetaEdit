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
#include <QLineEdit>

#include "Common/config.h"
#include "Common/mp4_Handler.h"
#include "ThirdParty/json/json.hpp"
#include "chantablewidget.h"

using namespace nlohmann;

//***************************************************************************
// Info
//***************************************************************************

//---------------------------------------------------------------------------
static const char* ColumnName_Default[ChanTableWidget::MAX_COLUMN] =
{
    "Layout",
    "Description",
};

//---------------------------------------------------------------------------
static const int ColumnSize_Default[ChanTableWidget::MAX_COLUMN] =
{
    120,
    80,
};

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
ChannelComboBox::ChannelComboBox(QWidget* Parent) : QComboBox(Parent)
{
    setEditable(true);

    QLineEdit* Editor = lineEdit();
    if(Editor)
        Editor->setClearButtonEnabled(true);

    connect(this, SIGNAL(highlighted(int)), this, SLOT(handleHighlighted(int))); // TODO: find a better signal for backuping the current state
    connect(this, SIGNAL(activated(int)), this, SLOT(handleActivated(int)));
}

//---------------------------------------------------------------------------
void ChannelComboBox::handleHighlighted(int Index)
{
    Q_UNUSED(Index);

    QLineEdit* Editor = lineEdit();
    if (!Editor)
        return;

    // Save current state
    PreviousText = Editor->text();
    PreviousCursorPosition = Editor->cursorPosition();
    PreviousSelectionStart = Editor->selectionStart();
    PreviousSelectionEnd = Editor->selectionEnd();
}

//---------------------------------------------------------------------------
void ChannelComboBox::handleActivated(int Index)
{
    QLineEdit* Editor = lineEdit();
    if (!Editor)
        return;

    QString NewElement = itemText(Index);
    QString NewElementData = itemData(Index).toString();

    if(PreviousText.isEmpty() ||
       (PreviousSelectionStart == 0 && PreviousSelectionEnd == PreviousText.length()) ||
       NewElementData == "DELETE" ||
       NewElementData == "ABSENT" ||
       NewElementData == "NODESCRIPTION") // Replace all
        Editor->setText(NewElement);
    else if (PreviousSelectionStart != PreviousSelectionEnd)
    {
        if (PreviousSelectionStart > 0 && PreviousText[PreviousSelectionStart] == '+')
            PreviousSelectionStart++; // If only the leading '+' is selected, don't overwrite the Previous item.

        int StartBoundary = PreviousSelectionStart > 0 ? PreviousText.lastIndexOf('+', PreviousSelectionStart - 1) : -1;

        StartBoundary = (StartBoundary == -1) ? 0 : StartBoundary + 1;

        if (PreviousSelectionEnd > 0 && PreviousText[PreviousSelectionEnd - 1] == '+')
            PreviousSelectionEnd--; // If only the trailing '+' is selected, don't overwrite the next item.

        int EndBoundary = PreviousText.indexOf('+', PreviousSelectionEnd);

        EndBoundary = (EndBoundary == -1) ? PreviousText.length() : EndBoundary;

        PreviousText.replace(StartBoundary, EndBoundary - StartBoundary, NewElement);
        Editor->setText(PreviousText);
    }
    else // No selection, just insert or remplace at the cursor position
    {
        if (PreviousCursorPosition < 0 || PreviousCursorPosition >= PreviousText.length()) // Append
        {
            if (PreviousText.endsWith('+'))
                Editor->setText(PreviousText + NewElement);
            else
                Editor->setText(PreviousText + "+" + NewElement);
        }
        else if (PreviousCursorPosition == 0) // Prepend
        {
            if (PreviousText.startsWith('+'))
                Editor->setText(NewElement + PreviousText);
            else
                Editor->setText(NewElement + "+" + PreviousText);
        }
        else if (PreviousText[PreviousCursorPosition] == '+') // Just before a '+', insert before
        {
                NewElement = "+" + NewElement;

            PreviousText.insert(PreviousCursorPosition, NewElement);
            Editor->setText(PreviousText);
        }
        else if (PreviousText[PreviousCursorPosition - 1] == '+') // Just after a '+', insert after
        {
            NewElement = NewElement + "+";

            PreviousText.insert(PreviousCursorPosition, NewElement);
            Editor->setText(PreviousText);
        }
        else
        {
            int LeftBoundary = PreviousText.lastIndexOf('+', PreviousCursorPosition - 1);
            int RightBoundary = PreviousText.indexOf('+', PreviousCursorPosition);

            LeftBoundary = (LeftBoundary == -1) ? 0 : LeftBoundary + 1;
            RightBoundary = (RightBoundary == -1) ? PreviousText.length() : RightBoundary;

            if (LeftBoundary > 0 && PreviousText[LeftBoundary - 1] != '+')
                NewElement = "+" + NewElement;

            if (RightBoundary < PreviousText.length() && PreviousText[RightBoundary] != '+')
                NewElement = NewElement + "+";

            PreviousText.replace(LeftBoundary, RightBoundary - LeftBoundary, NewElement);
            Editor->setText(PreviousText);
        }
    }
}

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

    if (!Index.isValid())
        return nullptr;

     QWidget* Editor;

    if (Index.column() == ChanTableWidget::LAYOUT_COLUMN)
    {
        Editor=new QComboBox(Parent);
        ((ChannelComboBox*)Editor)->setEditable(true);
        ((ChannelComboBox*)Editor)->setValidator(new QIntValidator(0, 0xffffffff));
    }
    else
    {
        Editor=new ChannelComboBox(Parent);
        ((ChannelComboBox*)Editor)->setEditable(true);
        ((ChannelComboBox*)Editor)->setValidator(new QIntValidator(0, 0xffffffff));
    }

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
    if (!Index.isValid() || !Editor)
        return;

    if (Index.column() == ChanTableWidget::DESC_COLUMN)
    {
        ChannelComboBox* ChannelBox = qobject_cast<ChannelComboBox*>(Editor);
        ChannelBox->clear();

        if (Index.data(Qt::UserRole).toString() == "ABSENT")
            ChannelBox->addItem("", "ABSENT");
        else if (Index.data(Qt::UserRole).toString() == "NODESCRIPTION")
            ChannelBox->addItem("(No description)", "NODESCRIPTION");
        ChannelBox->addItem(mp4_chan_ChannelDescription(0).c_str(), "0");
        ChannelBox->addItem(mp4_chan_ChannelDescription(1).c_str(), "1");
        ChannelBox->addItem(mp4_chan_ChannelDescription(2).c_str(), "2");
        ChannelBox->addItem(mp4_chan_ChannelDescription(3).c_str(), "3");
        ChannelBox->addItem(mp4_chan_ChannelDescription(4).c_str(), "4");
        ChannelBox->addItem(mp4_chan_ChannelDescription(5).c_str(), "5");
        ChannelBox->addItem(mp4_chan_ChannelDescription(6).c_str(), "6");
        ChannelBox->addItem(mp4_chan_ChannelDescription(7).c_str(), "7");
        ChannelBox->addItem(mp4_chan_ChannelDescription(8).c_str(), "8");
        ChannelBox->addItem(mp4_chan_ChannelDescription(9).c_str(), "9");
        ChannelBox->addItem(mp4_chan_ChannelDescription(10).c_str(), "10");
        ChannelBox->addItem(mp4_chan_ChannelDescription(11).c_str(), "11");
        ChannelBox->addItem(mp4_chan_ChannelDescription(12).c_str(), "12");
        ChannelBox->addItem(mp4_chan_ChannelDescription(13).c_str(), "13");
        ChannelBox->addItem(mp4_chan_ChannelDescription(14).c_str(), "14");
        ChannelBox->addItem(mp4_chan_ChannelDescription(15).c_str(), "15");
        ChannelBox->addItem(mp4_chan_ChannelDescription(16).c_str(), "16");
        ChannelBox->addItem(mp4_chan_ChannelDescription(17).c_str(), "17");
        ChannelBox->addItem(mp4_chan_ChannelDescription(18).c_str(), "18");
        ChannelBox->addItem(mp4_chan_ChannelDescription(33).c_str(), "33");
        ChannelBox->addItem(mp4_chan_ChannelDescription(34).c_str(), "34");
        ChannelBox->addItem(mp4_chan_ChannelDescription(35).c_str(), "35");
        ChannelBox->addItem(mp4_chan_ChannelDescription(36).c_str(), "36");
        ChannelBox->addItem(mp4_chan_ChannelDescription(37).c_str(), "37");
        ChannelBox->addItem(mp4_chan_ChannelDescription(38).c_str(), "38");
        ChannelBox->addItem(mp4_chan_ChannelDescription(39).c_str(), "39");
        ChannelBox->addItem(mp4_chan_ChannelDescription(40).c_str(), "40");
        ChannelBox->addItem(mp4_chan_ChannelDescription(41).c_str(), "41");
        ChannelBox->addItem(mp4_chan_ChannelDescription(42).c_str(), "42");
        ChannelBox->addItem(mp4_chan_ChannelDescription(43).c_str(), "43");
        ChannelBox->addItem(mp4_chan_ChannelDescription(44).c_str(), "44");
        ChannelBox->addItem(mp4_chan_ChannelDescription(45).c_str(), "45");
        ChannelBox->addItem(mp4_chan_ChannelDescription(200).c_str(), "200");
        ChannelBox->addItem(mp4_chan_ChannelDescription(201).c_str(), "201");
        ChannelBox->addItem(mp4_chan_ChannelDescription(202).c_str(), "202");
        ChannelBox->addItem(mp4_chan_ChannelDescription(203).c_str(), "203");
        ChannelBox->addItem(mp4_chan_ChannelDescription(204).c_str(), "204");
        ChannelBox->addItem(mp4_chan_ChannelDescription(205).c_str(), "205");
        ChannelBox->addItem(mp4_chan_ChannelDescription(206).c_str(), "206");
        ChannelBox->addItem(mp4_chan_ChannelDescription(207).c_str(), "207");
        ChannelBox->addItem(mp4_chan_ChannelDescription(301).c_str(), "301");
        ChannelBox->addItem(mp4_chan_ChannelDescription(302).c_str(), "302");
        ChannelBox->addItem(mp4_chan_ChannelDescription(304).c_str(), "304");
        ChannelBox->addItem(mp4_chan_ChannelDescription(305).c_str(), "305");
        ChannelBox->addItem(mp4_chan_ChannelDescription(400).c_str(), "400");

        if (ChannelBox->findData(Index.data(Qt::UserRole).toString()) == -1)
        {
            vector<uint32_t> Codes;
            QString Title;
            bool Ignore, Delete;
            mp4_chan_ChannelCodes(Index.data(Qt::UserRole).toString().toStdString(), Codes, Ignore, Delete);
            if (!Ignore && !Delete)
            {
                for (auto& Code : Codes)
                {
                    if (!Title.isEmpty())
                        Title += "+";
                    Title += QString::fromStdString(mp4_chan_ChannelDescription(Code));
                }
            }

            ChannelBox->addItem(Title, Index.data(Qt::UserRole).toString());
        }

        if (Index.data(Qt::UserRole).toString() != "ABSENT")
        {
            ChannelBox->insertSeparator(ChannelBox->count());
            ChannelBox->addItem("(Delete)", "DELETE");
        }

        ChannelBox->setCurrentIndex(ChannelBox->findData(Index.data(Qt::UserRole).toString()));
    }
    else if (Index.column() == ChanTableWidget::LAYOUT_COLUMN)
    {
        QComboBox* LayoutBox=qobject_cast<QComboBox*>(Editor);
        LayoutBox->clear();

        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x000000 ).c_str()), "0");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x010000 ).c_str()), "65536");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x640001 ).c_str()), "6553601");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x650002 ).c_str()), "6619138");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x660002 ).c_str()), "6684674");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x670002 ).c_str()), "6750210");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x680002 ).c_str()), "6815746");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x690002 ).c_str()), "6881282");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x6A0002 ).c_str()), "6946818");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x6B0004 ).c_str()), "7012356");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x6C0004 ).c_str()), "7077892");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x6D0005 ).c_str()), "7143429");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x6E0006 ).c_str()), "7208966");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x6F0008 ).c_str()), "7274504");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x700008 ).c_str()), "7340040");
        LayoutBox->addItem(QString("MPEG_1.0 (Mono)"),                         "6553601"); // MPEG_1.0 is Mono
        LayoutBox->addItem(QString("MPEG_2.0 (Stereo)"),                       "6619138"); // MPEG_2.0 is Stereo
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x710003 ).c_str()), "7405571");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x720003 ).c_str()), "7471107");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x730004 ).c_str()), "7536644");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x740004 ).c_str()), "7602180");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x750005 ).c_str()), "7667717");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x760005 ).c_str()), "7733253");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x770005 ).c_str()), "7798789");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x780005 ).c_str()), "7864325");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x790006 ).c_str()), "7929862");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x7A0006 ).c_str()), "7995398");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x7B0006 ).c_str()), "8060934");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x7C0006 ).c_str()), "8126470");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x7D0007 ).c_str()), "8192007");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x7E0008 ).c_str()), "8257544");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x7F0008 ).c_str()), "8323080");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x800008 ).c_str()), "8388616");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x810008 ).c_str()), "8454152");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x820008 ).c_str()), "8519688");
        LayoutBox->addItem(QString("ITU_1.0 (Mono)"),                          "6553601"); // ITU_1.0 is Mono
        LayoutBox->addItem(QString("ITU_2.0 (Stereo)"),                        "6619138"); // ITU_2.0 is Stereo
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x830003 ).c_str()), "8585219");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x840004 ).c_str()), "8650756");
        LayoutBox->addItem(QString("ITU_3.0 (MPEG_3.0A)"),                     "7405571"); // ITU_3.0 is MPEG_3.0A
        LayoutBox->addItem(QString("ITU_3.1 (MPEG_4.0A)"),                     "7536644"); // ITU_3.1 is MPEG_4.0A
        LayoutBox->addItem(QString("ITU_3.2 (MPEG_5.0A)"),                     "7667717"); // ITU_3.2 is MPEG_5.0A
        LayoutBox->addItem(QString("ITU_3.2.1 (MPEG_5.1A)"),                   "7929862"); // ITU_3.2.1 is MPEG_5.1A
        LayoutBox->addItem(QString("ITU_3.4.1 (MPEG_7.1C)"),                   "8388616"); // ITU_3.4.1 is MPEG_7.1C
        LayoutBox->addItem(QString("DVD_0 (Mono)"),                            "6553601"); // DVD_0 is Mono
        LayoutBox->addItem(QString("DVD_1 (Stereo)"),                          "6619138"); // DVD_1 is Stereo
        LayoutBox->addItem(QString("DVD_2 (ITU_2.1)"),                         "8585219"); // DVD_2 is ITU_2.1
        LayoutBox->addItem(QString("DVD_3 (ITU_2.2)"),                         "6619138"); // DVD_3 is ITU_2.2
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x850003 ).c_str()), "8716291");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x860004 ).c_str()), "8781828");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x870005 ).c_str()), "8847365");
        LayoutBox->addItem(QString("DVD_7 (MPEG_3.0A)"),                       "7405571"); // DVD_7 is MPEG_3.0A
        LayoutBox->addItem(QString("DVD_8 (MPEG_4.0A)"),                       "7536644"); // DVD_8 is MPEG_4.0A
        LayoutBox->addItem(QString("DVD_9 (MPEG_5.0A)"),                       "7667717"); // DVD_9 is MPEG_5.0A
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x880004 ).c_str()), "8912900");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x890005 ).c_str()), "8978437");
        LayoutBox->addItem(QString("DVD_12 (MPEG_5.1A)"),                      "7929862"); // DVD_12 is MPEG_5.1A
        LayoutBox->addItem(QString("DVD_13 (MPEG_4.0A)"),                      "7536644"); // DVD_13 is MPEG_4.0A
        LayoutBox->addItem(QString("DVD_14 (MPEG_5.0A)"),                      "7667717"); // DVD_14 is MPEG_5.0A
        LayoutBox->addItem(QString("DVD_15 (DVD_10)"),                         "8912900"); // DVD_15 is DVD_10
        LayoutBox->addItem(QString("DVD_16 (DVD_11)"),                         "8978437"); // DVD_16 is DVD_11
        LayoutBox->addItem(QString("DVD_17 (MPEG_5.1A)"),                      "7929862"); // DVD_17 is MPEG_5.1A
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x8A0005 ).c_str()), "9043973");
        LayoutBox->addItem(QString("DVD_19 (MPEG_5.0B)"),                      "7864325"); // DVD_19 is MPEG_5.0B
        LayoutBox->addItem(QString("DVD_20 (MPEG_5.1B)"),                      "7995398"); // DVD_20 is MPEG_5.1B
        LayoutBox->addItem(QString("AudioUnit_4 (Quadraphonic)"),              "7077892"); // AudioUnit_4 is Quadraphonic
        LayoutBox->addItem(QString("AudioUnit_5 (Pentagonal)")  ,              "7143429"); // AudioUnit_5 is Pentagonal
        LayoutBox->addItem(QString("AudioUnit_6 (Hexagonal)"),                 "7208966"); // AudioUnit_6 is Hexagonal
        LayoutBox->addItem(QString("AudioUnit_8 (Octogonal)"),                 "7274504"); // AudioUnit_8 is Octagonal
        LayoutBox->addItem(QString("AudioUnit_5.0 (MPEG_5.0B)"),               "7864325"); // AudioUnit_5.0 is MPEG_5.0B
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x8B0006 ).c_str()), "9109510");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x8C0007 ).c_str()), "9175047");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x8D0006 ).c_str()), "9240582");
        LayoutBox->addItem(QString("AudioUnit_5.1 (MPEG_5.1A)"),               "7929862"); // AudioUnit_5.1 is MPEG_5.1A
        LayoutBox->addItem(QString("AudioUnit_6.1 (MPEG_6.1A)"),               "8192007"); // AudioUnit_6.1 is MPEG_6.1A
        LayoutBox->addItem(QString("AudioUnit_7.1 (MPEG_7.1C)"),               "8388616"); // AudioUnit_7.1 is MPEG_7.1C
        LayoutBox->addItem(QString("AudioUnit_7.1Front (MPEG_7.11)"),          "8257544"); // AudioUnit_7.1Front is MPEG_7.1A
        LayoutBox->addItem(QString("AAC_3.0 (MPEG_3.0B)"),                     "7471107"); // AAC_3.0 is MPEG_3.0B
        LayoutBox->addItem(QString("AAC_Quadraphonic (Quadraphonic)"),         "7077892"); // AAC_Quadraphonic is Quadraphonic
        LayoutBox->addItem(QString("AAC_4.0 (MPEG_4.0B)"),                     "7602180"); // AAC_4.0 is MPEG_4.0B
        LayoutBox->addItem(QString("AAC_5.0 (MPEG_5.0D)"),                     "7864325"); // AAC_5.0 is MPEG_5.0D
        LayoutBox->addItem(QString("AAC_5.1 (MPEG_5.1D)"),                     "8126470"); // AAC_5.1 is MPEG_5.1D
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x8E0007 ).c_str()), "9306119");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x8F0007 ).c_str()), "9371655");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x900008 ).c_str()), "9437192");
        LayoutBox->addItem(QString("AAC_7.1 (MPEG_7.1B)"),                     "8323080"); // AAC_7.1 is MPEG_7.1B
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x910010 ).c_str()), "9502736");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x920015 ).c_str()), "9568277");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x940007 ).c_str()), "9699335");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x950002 ).c_str()), "9764866");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x960003 ).c_str()), "9830403");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x970004 ).c_str()), "9895940");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x980004 ).c_str()), "9961476");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x990004 ).c_str()), "10027012");
        LayoutBox->addItem(QString(mp4_chan_ChannelLayout(0x9A0005 ).c_str()), "10092549");

        if (LayoutBox->findData(Index.data(Qt::UserRole).toString()) == -1)
            LayoutBox->addItem(Index.data(Qt::UserRole).toString(), Index.data(Qt::UserRole).toString());

        LayoutBox->setCurrentIndex(LayoutBox->findData(Index.data(Qt::UserRole).toString()));
    }
}

//---------------------------------------------------------------------------
void ChannelDelegate::setModelData(QWidget* Editor,
                                   QAbstractItemModel* Model,
                                   const QModelIndex& Index) const
{
    if (!Index.isValid() || !Editor || !Model)
        return;

    if (Index.column() == ChanTableWidget::DESC_COLUMN)
    {
        ChannelComboBox *ChannelBox = qobject_cast<ChannelComboBox *>(Editor);
        string CurrentText = ChannelBox->currentText().toStdString();
        string CurrentData = ChannelBox->currentData().toString().toStdString();
        if (CurrentData == "NODESCRIPTION" || CurrentData == "ABSENT" || CurrentData == "DELETE")
            CurrentText = CurrentData;

        QString OldValue = Model->data(Index, Qt::UserRole).toString();

        bool Ignore, Delete;
        vector<uint32_t> Codes;
        QString Value;
        mp4_chan_ChannelCodes(CurrentText, Codes, Ignore, Delete);
        if (Ignore)
            Value = "ABSENT";
        else if (Delete)
            Value = "DELETE";
        else
        {
            for (auto& Code : Codes)
            {
                if (!Value.isEmpty())
                    Value += "+";
                Value += QString::fromStdString(mp4_chan_ChannelDescription(Code));
            }
        }

        if (Value != OldValue)
        {
            Model->setData(Index, Value, Qt::UserRole);
            emit Value_Changed(Index.row());
        }
    }
    else if (Index.column() == ChanTableWidget::LAYOUT_COLUMN)
    {
        QComboBox* LayoutBox = qobject_cast<QComboBox*>(Editor);

        QString OldValue = Model->data(Index, Qt::UserRole).toString();

        bool Ok=false;
        QString Value=LayoutBox->currentText();
        Value.toUInt(&Ok);
        if (!Ok)
            Value=LayoutBox->currentData().toString();

        if(Value!=OldValue)
        {
            Model->setData(Index, Value, Qt::UserRole);
            emit Value_Changed(Index.row());
        }
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
    setItemDelegateForColumn(LAYOUT_COLUMN, qobject_cast<QAbstractItemDelegate*>(ChannelEditor));
    setItemDelegateForColumn(DESC_COLUMN, qobject_cast<QAbstractItemDelegate*>(ChannelEditor));

    Update_Table();
}

//---------------------------------------------------------------------------
void ChanTableWidget::Update_Table()
{
    //Get audio tracks
    json Json = json::parse(Channels->toStdString(), nullptr, false);
    json PreviousJson = json::parse(Previous.toStdString(), nullptr, false);
    if (!Json.is_discarded())
    {
        size_t Pos = 0;
        for (auto &Entry : Json.items())
        {
            if(rowCount() == Pos)
                insertRow(rowCount());

            QString Index;
            Index=Entry.key().c_str();

            auto Layout = Entry.value().find("layout");
            auto Descriptions = Entry.value().find("descriptions");
            {
                QString Title;
                QString UserData;

                bool Layout_Modified = false;
                if (Layout != Entry.value().end() && Layout->is_string())
                {
                    Title = QString().fromStdString(Layout->get<string>());
                    UserData = Title;
                    uint32_t Code;
                    mp4_chan_ChannelLayoutCode(Layout->get<string>(), Code);
                    UserData = QString::number(Code);

                    if (PreviousJson.contains(Entry.key()))
                    {
                        auto PrevLayout = PreviousJson[Entry.key()].find("layout");
                        if (PrevLayout != PreviousJson[Entry.key()].end() &&
                            PrevLayout->is_string() &&
                            PrevLayout->get<string>() != Layout->get<string>())
                            Layout_Modified=true;
                    }
                }

                QTableWidgetItem* Value = new QTableWidgetItem(Title);
                Value->setData(Qt::UserRole, UserData);
                Value->setData(Qt::UserRole+1, Index);
                if (Layout_Modified)
                {
                    QFont Font=Value->font();
                    Font.setBold(true);
                    Value->setFont(Font);
                }

                setItem(Pos, LAYOUT_COLUMN, Value);
            }

            {
                QString Title;
                QString UserData;
                bool Descriptions_Modified=false;

                if (Descriptions != Entry.value().end() && Descriptions->is_string())
                {
                    Title = QString().fromStdString(Descriptions->get<string>());
                    vector<uint32_t> Codes;
                    bool Ignore, Delete;
                    mp4_chan_ChannelCodes(Descriptions->get<string>(), Codes, Ignore, Delete);
                    if (Ignore)
                    {
                        Title = "";
                        UserData = "ABSENT";
                    }
                    else if (Delete)
                    {
                        Title = "(Delete)";
                        UserData = "DELETE";
                    }
                    else
                    {
                        for (auto &Code : Codes)
                        {
                            if (!UserData.isEmpty())
                                UserData += "+";
                            UserData += QString().number(Code);
                        }
                    }

                    if (PreviousJson.contains(Entry.key()))
                    {
                        auto PrevDescriptions = PreviousJson[Entry.key()].find("descriptions");
                        if (PrevDescriptions != PreviousJson[Entry.key()].end() &&
                            PrevDescriptions->is_string() &&
                            PrevDescriptions->get<string>() != Descriptions->get<string>())
                            Descriptions_Modified = true;
                    }
                }
                else if (Layout != Entry.value().end() && Layout->is_string() && Layout->get<string>() == "UseChannelDescriptions")
                {
                    Title = "(No description)";
                    UserData = "NODESCRIPTION";
                }

                QTableWidgetItem* Value=new QTableWidgetItem(Title);
                if (item(Pos, LAYOUT_COLUMN)->data(Qt::UserRole).toString() != "0")
                    Value->setFlags(Value->flags() & ~Qt::ItemIsEditable);
                Value->setData(Qt::UserRole, UserData);
                Value->setData(Qt::UserRole+1, Index);
                if (Descriptions_Modified)
                {
                    QFont Font=Value->font();
                    Font.setBold(true);
                    Value->setFont(Font);
                }

                setItem(Pos, DESC_COLUMN, Value);
            }

            Pos++;
        }
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
    json Json = json::parse(Channels->toStdString(), nullptr, false);

    if (Json.is_discarded())
        return;

    string Index = item(Row, LAYOUT_COLUMN)->data(Qt::UserRole+1).toString().toStdString();
    string Layout = mp4_chan_ChannelLayout(item(Row, LAYOUT_COLUMN)->data(Qt::UserRole).toString().toUInt());

    string Desc = item(Row, DESC_COLUMN)->data(Qt::UserRole).toString().toStdString();

    if (Json.contains(Index))
    {
        Json[Index]["layout"] = Layout;

        uint32_t Code;
        mp4_chan_ChannelLayoutCode(Layout, Code);
        if (Code==0)
            Json[Index]["descriptions"] = Desc;
        else
            Json[Index].erase("descriptions");
    }

    *Channels = QString::fromStdString(Json.dump());

    Update_Table();
}
