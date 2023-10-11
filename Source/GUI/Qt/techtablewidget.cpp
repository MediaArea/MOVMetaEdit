/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include <QResizeEvent>
#include <QScrollBar>
#include <QBoxLayout>
#include <QFont>
#include <QDebug>
#include <QLabel>

#include "techtablewidget.h"
#include "channeldialog.h"

//***************************************************************************
// Info
//***************************************************************************

static const char* ColumnName_Default[TechTableWidget::MAX_COLUMN] =
{
    "File Name",
    "Clean Ap.",
    "Prod Ap.",
    "Enc. Ap.",
    "PAR",
    "w-scale",
    "Field",
    "Color",
    "Gamma",
    "Aperture",
    "Display Primaries",
    "Luminance",
    "Max. CLL",
    "Max. FALL",
    "Channels",
};

static const int ColumnSize_Default[TechTableWidget::MAX_COLUMN] =
{
    60,
    20,
    20,
    20,
    20,
    15,
    40,
    60,
    15,
    30,
    70,
    30,
    20,
    20,
    20,
};


//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
QString mp4_fiel_Fields(QString Field)
{
    if (Field.isEmpty())
        return QString();
    else if (Field=="1")
        return Field + " - Progressive";
    else if (Field=="2")
       return Field + " - Interlaced";

    return Field + " - Unknown";
}

//---------------------------------------------------------------------------
QString mp4_fiel_Detail(QString Detail)
{
    if (Detail.isEmpty())
        return QString();
    else if (Detail=="1")
       return Detail + " - TT";
    else if (Detail=="6")
       return Detail + " - BB";
    else if (Detail=="9")
       return Detail + " - BT";
    else if (Detail=="14")
       return Detail + " - TB";

    return Detail + " - Unknown";
}

//---------------------------------------------------------------------------
QString mp4_colr_Primaries(QString Primaries)
{
    if (Primaries.isEmpty())
        return QString();
    else if (Primaries=="1")
        return Primaries + " - BT.709";
    else if (Primaries=="4")
        return Primaries + " - BT.470 System M";
    else if (Primaries=="5")
        return Primaries + " - BT.601 PAL";
    else if (Primaries=="6")
        return Primaries + " - BT.601 NTSC";
    else if (Primaries=="7")
        return Primaries + " - SMPTE 240M";
    else if (Primaries=="8")
        return Primaries + " - Generic film";
    else if (Primaries=="9")
        return Primaries + " - BT.2020";
    else if (Primaries=="10")
        return Primaries + " - XYZ";
    else if (Primaries=="11")
        return Primaries + " - DCI P3";
    else if (Primaries=="12")
        return Primaries + " - Display P3";
    else if (Primaries=="22")
        return Primaries + " - EBU Tech 3213";

    return Primaries + " - Unknown";
}

//---------------------------------------------------------------------------
QString mp4_colr_Transfer(QString Transfer)
{
    if (Transfer.isEmpty())
        return QString();
    else if (Transfer=="1")
        return Transfer + " - BT.709";
    else if (Transfer=="4")
        return Transfer + " - BT.470 System M";
    else if (Transfer=="5")
        return Transfer + " - BT.470 System B/G";
    else if (Transfer=="6")
        return Transfer + " - BT.601";
    else if (Transfer=="7")
        return Transfer + " - SMPTE 240M";
    else if (Transfer=="8")
        return Transfer + " - Linear";
    else if (Transfer=="9")
        return Transfer + " - Logarithmic (100:1)";
    else if (Transfer=="10")
        return Transfer + " - Logarithmic (316.22777:1)";
    else if (Transfer=="11")
        return Transfer + " - xvYCC";
    else if (Transfer=="12")
        return Transfer + " - BT.1361";
    else if (Transfer=="13")
        return Transfer + " - sRGB/sYCC";
    else if (Transfer=="14")
        return Transfer + " - BT.2020 (10-bit)";
    else if (Transfer=="15")
        return Transfer + " - BT.2020 (12-bit)";
    else if (Transfer=="16")
        return Transfer + " - PQ";
    else if (Transfer=="17")
        return Transfer + " - SMTPE 428M";
    else if (Transfer=="18")
        return Transfer + " - HLG";

    return Transfer + " - Unknown";
}

//---------------------------------------------------------------------------
QString mp4_colr_Matrix(QString Matrix)
{
    if (Matrix.isEmpty())
        return QString();
    else if (Matrix=="0")
        return Matrix + " - Identity";
    else if (Matrix=="1")
        return Matrix + " - BT.709";
    else if (Matrix=="4")
        return Matrix + " - FCC 73.682";
    else if (Matrix=="5")
        return Matrix + " - BT.470 System B/G";
    else if (Matrix=="6")
        return Matrix + " - BT.601";
    else if (Matrix=="7")
        return Matrix + " - SMPTE 240M";
    else if (Matrix=="8")
        return Matrix + " - YCgCo";
    else if (Matrix=="9")
        return Matrix + " - BT.2020 non-constant";
    else if (Matrix=="10")
        return Matrix + " - BT.2020 constant";
    else if (Matrix=="11")
        return Matrix + " - Y'D'zD'x";
    else if (Matrix=="12")
        return Matrix + " - Chromaticity-derived non-constant";
    else if (Matrix=="13")
        return Matrix + " - Chromaticity-derived constant";
    else if (Matrix=="14")
        return Matrix + " - ICtCp";

    return Matrix + " - Unknown";
}

//---------------------------------------------------------------------------
ApertureValidator::ApertureValidator(QObject* Parent) : QValidator(Parent)
{
}

//---------------------------------------------------------------------------
QValidator::State ApertureValidator::validate(QString& Input, int& Pos) const
{
    Q_UNUSED(Pos)

    if (Input.isEmpty())
        return QValidator::Acceptable;

    bool Separator_Present=false;
    QValidator::State State=QValidator::Intermediate;
    for(int Pos = 0; Pos < Input.size(); Pos++)
    {
        if (Input[Pos]=='X')
            Input[Pos]=Input[Pos].toLower();

        if (Input[Pos]>='0' && Input[Pos]<='9')
        {
            if (Separator_Present)
                State=QValidator::Acceptable;
        }
        else if (Input[Pos]=='x')
        {
            if (Separator_Present || !Pos)
                return QValidator::Invalid;

            Separator_Present=true;
        }
        else
            return QValidator::Invalid;
    }

    return State;
}

//---------------------------------------------------------------------------
RatioValidator::RatioValidator(QObject* Parent) : QValidator(Parent)
{
}

//---------------------------------------------------------------------------
QValidator::State RatioValidator::validate(QString& Input, int& Pos) const
{
    Q_UNUSED(Pos)

    if (Input.isEmpty())
        return QValidator::Acceptable;

    bool Separator_Present=false;
    QValidator::State State=QValidator::Intermediate;
    for(int Pos = 0; Pos < Input.size(); Pos++)
    {
        if (Input[Pos]>='0' && Input[Pos]<='9')
        {
            if (Separator_Present)
                State=QValidator::Acceptable;
        }
        else if (Input[Pos]==':')
        {
            if (Separator_Present || !Pos)
                return QValidator::Invalid;

            Separator_Present=true;
        }
        else
            return QValidator::Invalid;
    }

    return State;
}

//---------------------------------------------------------------------------
CleanApertureValidator::CleanApertureValidator(QObject* Parent) : QValidator(Parent)
{
}

//---------------------------------------------------------------------------
QValidator::State CleanApertureValidator::validate(QString& Input, int& Pos) const
{
    Q_UNUSED(Pos)

    if (Input.isEmpty())
        return QValidator::Acceptable;

    bool Den=false;
    unsigned int Field=0;
    QValidator::State State=QValidator::Intermediate;
    for(int Pos = 0; Pos < Input.size(); Pos++)
    {
        if (Input[Pos]>='0' && Input[Pos]<='9')
        {
            if (Field==3)
                State=QValidator::Acceptable;
        }
        else if (Input[Pos]==',')
        {
            if (++Field>3 || !Pos)
                return QValidator::Invalid;

            Den=false;
        }
        else if (Input[Pos]==':')
        {
            if (Den)
                return QValidator::Invalid;

            Den=true;
            State=QValidator::Intermediate;
        }
        else
            return QValidator::Invalid;
    }

    return State;
}

//---------------------------------------------------------------------------
PrimariesValidator::PrimariesValidator(QObject* Parent) : QValidator(Parent)
{
}

//---------------------------------------------------------------------------
QValidator::State PrimariesValidator::validate(QString& Input, int& Pos) const
{
    Q_UNUSED(Pos)

    if (Input.isEmpty())
        return QValidator::Acceptable;

    QValidator::State State=QValidator::Intermediate;

    QDoubleValidator Validator;
    Validator.setLocale(QLocale(QLocale::C));

    QStringList Values = Input.split(',');
    if (Values.size() > 8)
    {
        State =  QValidator::Invalid;
    }
    else if (Values.size() > 0)
    {
        for (int Pos=0; Pos<Values.size(); Pos++)
        {
            int Cursor=0;
            QValidator::State Cur = Validator.validate(Values[Pos], Cursor);

            if (Cur == QValidator::Invalid)
            {
                State = QValidator::Invalid;
                break;
            }
            else if (Pos==0 || State >= Cur)
                State = Cur;
        }

        if (Values.size() < 8 && State == QValidator::Acceptable)
            State = QValidator::Intermediate;
    }
    Input=Values.join(',');

    return State;
}

//---------------------------------------------------------------------------
ApertureDelegate::ApertureDelegate(QObject* Parent, Core* C) : QItemDelegate(Parent), C(C)
{
}

//---------------------------------------------------------------------------
QWidget* ApertureDelegate::createEditor(QWidget* Parent,
                                        const QStyleOptionViewItem& Option,
                                        const QModelIndex& Index) const
{
    Q_UNUSED(Option)
    Q_UNUSED(Index)

    QLineEdit *Editor = new QLineEdit(Parent);
    Editor->setValidator(new ApertureValidator());

    return Editor;
}

//---------------------------------------------------------------------------
void ApertureDelegate::updateEditorGeometry(QWidget* Editor,
                                            const QStyleOptionViewItem& Option,
                                            const QModelIndex& Index) const
{
    Q_UNUSED(Index)

    Editor->setGeometry(Option.rect);
}

//---------------------------------------------------------------------------
void ApertureDelegate::setEditorData(QWidget* Editor, const QModelIndex& Index) const
{
    qobject_cast<QLineEdit*>(Editor)->setText(Index.data(Qt::EditRole).toString());
}

//---------------------------------------------------------------------------
void ApertureDelegate::setModelData(QWidget* Editor,
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
RatioDelegate::RatioDelegate(QObject* Parent, Core* C) : QItemDelegate(Parent), C(C)
{
}

//---------------------------------------------------------------------------
QWidget* RatioDelegate::createEditor(QWidget* Parent,
                                        const QStyleOptionViewItem& Option,
                                        const QModelIndex& Index) const
{
    Q_UNUSED(Option)
    Q_UNUSED(Index)

    QLineEdit *Editor = new QLineEdit(Parent);
    Editor->setValidator(new RatioValidator());

    return Editor;
}

//---------------------------------------------------------------------------
void RatioDelegate::updateEditorGeometry(QWidget* Editor,
                                            const QStyleOptionViewItem& Option,
                                            const QModelIndex& Index) const
{
    Q_UNUSED(Index)

    Editor->setGeometry(Option.rect);
}

//---------------------------------------------------------------------------
void RatioDelegate::setEditorData(QWidget* Editor, const QModelIndex& Index) const
{
    qobject_cast<QLineEdit*>(Editor)->setText(Index.data(Qt::EditRole).toString());
}

//---------------------------------------------------------------------------
void RatioDelegate::setModelData(QWidget* Editor,
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
DoubleDelegate::DoubleDelegate(QObject* Parent, Core* C) : QItemDelegate(Parent), C(C)
{
}

//---------------------------------------------------------------------------
QWidget* DoubleDelegate::createEditor(QWidget* Parent,
                                        const QStyleOptionViewItem& Option,
                                        const QModelIndex& Index) const
{
    Q_UNUSED(Option)
    Q_UNUSED(Index)

    QDoubleValidator* Validator=new QDoubleValidator();
    Validator->setLocale(QLocale(QLocale::C));
    Validator->setBottom(0.0);

    QLineEdit *Editor = new QLineEdit(Parent);
    Editor->setValidator(Validator);

    return Editor;
}

//---------------------------------------------------------------------------
void DoubleDelegate::updateEditorGeometry(QWidget* Editor,
                                            const QStyleOptionViewItem& Option,
                                            const QModelIndex& Index) const
{
    Q_UNUSED(Index)

    Editor->setGeometry(Option.rect);
}

//---------------------------------------------------------------------------
void DoubleDelegate::setEditorData(QWidget* Editor, const QModelIndex& Index) const
{
    qobject_cast<QLineEdit*>(Editor)->setText(Index.data(Qt::EditRole).toString());
}

//---------------------------------------------------------------------------
void DoubleDelegate::setModelData(QWidget* Editor,
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
FieldsDelegate::FieldsDelegate(Core* C, QObject* Parent) : QItemDelegate(Parent), C(C)
{
}

//---------------------------------------------------------------------------
QWidget* FieldsDelegate::createEditor(QWidget* Parent,
                                        const QStyleOptionViewItem& Option,
                                        const QModelIndex& Index) const
{
    Q_UNUSED(Option)
    Q_UNUSED(Index)

    QWidget* Widget=new QWidget(Parent);
    Widget->setAutoFillBackground(true);
    QComboBox* FieldsBox=new QComboBox(Widget);
    FieldsBox->setFocusPolicy(Qt::NoFocus);
    FieldsBox->setObjectName("FieldsBox");
    FieldsBox->setFrame(false);
    QComboBox* DetailBox=new QComboBox(Widget);
    DetailBox->setFocusPolicy(Qt::NoFocus);
    DetailBox->setObjectName("DetailBox");
    DetailBox->setFrame(false);

    QBoxLayout* Layout=new QBoxLayout(QBoxLayout::LeftToRight, Widget);
    Layout->addWidget(FieldsBox, 1);
    Layout->addWidget(DetailBox, 1);

    connect(FieldsBox, SIGNAL(currentIndexChanged(int)), this, SLOT(On_Fields_Changed(int)));
    connect(FieldsBox, SIGNAL(activated(int)), this, SLOT(On_Changed(int)));
    connect(DetailBox, SIGNAL(activated(int)), this, SLOT(On_Changed(int)));

    return Widget;
}

//---------------------------------------------------------------------------
void FieldsDelegate::updateEditorGeometry(QWidget* Editor,
                                            const QStyleOptionViewItem& Option,
                                            const QModelIndex& Index) const
{
    Q_UNUSED(Index)

    Editor->setGeometry(Option.rect);
}

//---------------------------------------------------------------------------
void FieldsDelegate::setEditorData(QWidget* Editor, const QModelIndex& Index) const
{
    QComboBox* FieldsBox=Editor->findChild<QComboBox*>("FieldsBox");
    QComboBox* DetailBox=Editor->findChild<QComboBox*>("DetailBox");

    if (!FieldsBox || !DetailBox)
        return;

    FieldsBox->clear();
    FieldsBox->addItem("", "");
    FieldsBox->addItem(mp4_fiel_Fields("1"), "1");
    FieldsBox->addItem(mp4_fiel_Fields("2"), "2");

    QStringList Old=Index.data(Qt::UserRole).toString().split(",");
    QString Fields=Old.isEmpty()?"":Old[0];
    QString Details=Old.size()<2?"":Old[1];

    if (!Fields.isEmpty() && Fields!="1" && Fields!="2")
        FieldsBox->addItem(mp4_fiel_Fields(Fields), Fields);

    int Width=0;
    QFontMetrics Fm(FieldsBox->font());
    for(int Pos=0; Pos<FieldsBox->count(); Pos++)
    {
        int Temp=Fm.boundingRect(FieldsBox->itemText(Pos)).width();
        if (Width<Temp)
            Width=Temp;
    }
    FieldsBox->setStyleSheet(QString("QListView {min-width: %1px; }").arg(10+(Width*1.15)));

    FieldsBox->setItemData(FieldsBox->findData(Fields), Details, Qt::UserRole+1);
    FieldsBox->setCurrentIndex(FieldsBox->findData(Fields));
}

//---------------------------------------------------------------------------
void FieldsDelegate::setModelData(QWidget* Editor,
                                 QAbstractItemModel* Model,
                                 const QModelIndex& Index) const
{
    QComboBox* FieldsBox=Editor->findChild<QComboBox*>("FieldsBox");
    QComboBox* DetailBox=Editor->findChild<QComboBox*>("DetailBox");

    if (!FieldsBox || !DetailBox)
        return;

    QString Fields=FieldsBox->currentData().toString();
    QString Detail=DetailBox->currentData().toString();
    QString Value;

    if (Fields.isEmpty())
        Value=Fields;
    else
        Value=Fields+","+Detail;

    QString OldValue = Model->data(Index, Qt::UserRole).toString();
    if(Value != OldValue)
    {
        Model->setData(Index, Value, Qt::UserRole);
        emit Value_Changed(Index.row());
    }
}

//---------------------------------------------------------------------------
void FieldsDelegate::On_Fields_Changed(int)
{
    QWidget* Widget=qobject_cast<QWidget*>(sender()->parent());
    if (!Widget)
        return;

    QComboBox* FieldsBox=Widget->findChild<QComboBox*>("FieldsBox");
    QComboBox* DetailBox=Widget->findChild<QComboBox*>("DetailBox");

    if (!FieldsBox || !DetailBox)
        return;

    DetailBox->clear();
    DetailBox->setEnabled(false);
    QString Fields=FieldsBox->currentData().toString();
    QString Detail=FieldsBox->currentData(Qt::UserRole+1).toString();

    if (Fields.isEmpty())
    {
        DetailBox->addItem("", "");
        DetailBox->setCurrentIndex(DetailBox->findData(""));
    }
    else if (Fields=="1")
    {
        DetailBox->addItem("0", "0");
        DetailBox->setCurrentIndex(DetailBox->findData("0"));
    }
    else if (Fields=="2")
    {
        DetailBox->setEnabled(true);
        DetailBox->addItem(mp4_fiel_Detail("1"), "1");
        DetailBox->addItem(mp4_fiel_Detail("6"), "6");
        DetailBox->addItem(mp4_fiel_Detail("9"), "9");
        DetailBox->addItem(mp4_fiel_Detail("14"), "14");

        if (!Detail.isEmpty())
        {
            if (Detail!="1" && Detail!="6"  && Detail!="9" && Detail!="14")
                DetailBox->addItem(mp4_fiel_Detail(Detail), Detail);

            DetailBox->setCurrentIndex(DetailBox->findData(Detail));
        }
        else
            DetailBox->setCurrentIndex(0);
    }
    else
    {
        DetailBox->addItem(mp4_fiel_Detail(Detail), Detail);
        DetailBox->setCurrentIndex(DetailBox->findData(Detail));
    }

    int Width=0;
    QFontMetrics Fm(DetailBox->font());
    for(int Pos=0; Pos<DetailBox->count(); Pos++)
    {
        int Temp=Fm.boundingRect(DetailBox->itemText(Pos)).width();
        if (Width<Temp)
            Width=Temp;
    }
    DetailBox->setStyleSheet(QString("QListView {min-width: %1px; }").arg(10+(Width*1.15)));

    FieldsBox->setItemData(FieldsBox->currentIndex(), DetailBox->currentData(), Qt::UserRole+1);
}

//---------------------------------------------------------------------------
void FieldsDelegate::On_Changed(int)
{
    QWidget* Widget=qobject_cast<QWidget*>(sender()->parent());
    if (!Widget)
        return;

    emit commitData(Widget);
}

//---------------------------------------------------------------------------
ColorDelegate::ColorDelegate(Core* C, QObject* Parent) : QItemDelegate(Parent), C(C)
{
}

//---------------------------------------------------------------------------
QWidget* ColorDelegate::createEditor(QWidget* Parent,
                                        const QStyleOptionViewItem& Option,
                                        const QModelIndex& Index) const
{
    Q_UNUSED(Option)
    Q_UNUSED(Index)

    QWidget* Widget=new QWidget(Parent);
    Widget->setAutoFillBackground(true);
    QComboBox* PrimariesBox=new QComboBox(Widget);
    PrimariesBox->setObjectName("PrimariesBox");
    PrimariesBox->setFocusPolicy(Qt::NoFocus);
    PrimariesBox->setFrame(false);
    QComboBox* TransferBox=new QComboBox(Widget);
    TransferBox->setObjectName("TransferBox");
    TransferBox->setFocusPolicy(Qt::NoFocus);
    TransferBox->setFrame(false);
    QComboBox* MatrixBox=new QComboBox(Widget);
    MatrixBox->setFocusPolicy(Qt::NoFocus);
    MatrixBox->setObjectName("MatrixBox");
    MatrixBox->setFrame(false);

    QBoxLayout* Layout=new QBoxLayout(QBoxLayout::LeftToRight, Widget);
    Layout->addWidget(PrimariesBox, 1);
    Layout->addWidget(TransferBox, 1);
    Layout->addWidget(MatrixBox, 1);
    connect(PrimariesBox, SIGNAL(currentIndexChanged(int)), this, SLOT(On_Primaries_Changed(int)));
    connect(PrimariesBox, SIGNAL(activated(int)), this, SLOT(On_Color_Changed(int)));
    connect(TransferBox, SIGNAL(activated(int)), this, SLOT(On_Color_Changed(int)));
    connect(MatrixBox, SIGNAL(activated(int)), this, SLOT(On_Color_Changed(int)));

    return Widget;
}

//---------------------------------------------------------------------------
void ColorDelegate::updateEditorGeometry(QWidget* Editor,
                                            const QStyleOptionViewItem& Option,
                                            const QModelIndex& Index) const
{
    Q_UNUSED(Index)

    Editor->setGeometry(Option.rect);
}

//---------------------------------------------------------------------------
void ColorDelegate::setEditorData(QWidget* Editor, const QModelIndex& Index) const
{
    QComboBox* PrimariesBox=Editor->findChild<QComboBox*>("PrimariesBox");
    QComboBox* TransferBox=Editor->findChild<QComboBox*>("TransferBox");
    QComboBox* MatrixBox=Editor->findChild<QComboBox*>("MatrixBox");

    if (!PrimariesBox || !TransferBox || !MatrixBox)
        return;

    PrimariesBox->clear();
    PrimariesBox->addItem("", "");
    PrimariesBox->addItem(mp4_colr_Primaries("1"), "1");
    PrimariesBox->addItem(mp4_colr_Primaries("4"), "4");
    PrimariesBox->addItem(mp4_colr_Primaries("5"), "5");
    PrimariesBox->addItem(mp4_colr_Primaries("6"), "6");
    PrimariesBox->addItem(mp4_colr_Primaries("7"), "7");
    PrimariesBox->addItem(mp4_colr_Primaries("8"), "8");
    PrimariesBox->addItem(mp4_colr_Primaries("9"), "9");
    PrimariesBox->addItem(mp4_colr_Primaries("10"), "10");
    PrimariesBox->addItem(mp4_colr_Primaries("11"), "11");
    PrimariesBox->addItem(mp4_colr_Primaries("12"), "12");
    PrimariesBox->addItem(mp4_colr_Primaries("22"), "22");

    TransferBox->clear();
    TransferBox->addItem(mp4_colr_Transfer("1"), "1");
    TransferBox->addItem(mp4_colr_Transfer("4"), "4");
    TransferBox->addItem(mp4_colr_Transfer("5"), "5");
    TransferBox->addItem(mp4_colr_Transfer("6"), "6");
    TransferBox->addItem(mp4_colr_Transfer("7"), "7");
    TransferBox->addItem(mp4_colr_Transfer("8"), "8");
    TransferBox->addItem(mp4_colr_Transfer("9"), "9");
    TransferBox->addItem(mp4_colr_Transfer("10"), "10");
    TransferBox->addItem(mp4_colr_Transfer("11"), "11");
    TransferBox->addItem(mp4_colr_Transfer("12"), "12");
    TransferBox->addItem(mp4_colr_Transfer("13"), "13");
    TransferBox->addItem(mp4_colr_Transfer("14"), "14");
    TransferBox->addItem(mp4_colr_Transfer("15"), "15");
    TransferBox->addItem(mp4_colr_Transfer("16"), "16");
    TransferBox->addItem(mp4_colr_Transfer("17"), "17");
    TransferBox->addItem(mp4_colr_Transfer("18"), "18");

    MatrixBox->clear();
    MatrixBox->addItem(mp4_colr_Matrix("0"), "0");
    MatrixBox->addItem(mp4_colr_Matrix("1"), "1");
    MatrixBox->addItem(mp4_colr_Matrix("4"), "4");
    MatrixBox->addItem(mp4_colr_Matrix("5"), "5");
    MatrixBox->addItem(mp4_colr_Matrix("6"), "6");
    MatrixBox->addItem(mp4_colr_Matrix("7"), "7");
    MatrixBox->addItem(mp4_colr_Matrix("8"), "8");
    MatrixBox->addItem(mp4_colr_Matrix("9"), "9");
    MatrixBox->addItem(mp4_colr_Matrix("10"), "10");
    MatrixBox->addItem(mp4_colr_Matrix("11"), "11");
    MatrixBox->addItem(mp4_colr_Matrix("12"), "12");
    MatrixBox->addItem(mp4_colr_Matrix("13"), "13");
    MatrixBox->addItem(mp4_colr_Matrix("14"), "14");

    QStringList Old=Index.data(Qt::UserRole).toString().split(",");
    QString Primaries=Old.isEmpty()?"":Old[0];
    QString Transfer=Old.size()<2?"":Old[1];
    QString Matrix=Old.size()<3?"":Old[2];

    if (!Primaries.isEmpty() && PrimariesBox->findData(Primaries)==-1)
        PrimariesBox->addItem(mp4_colr_Primaries(Primaries), Primaries);

    {
        int Width=0;
        QFontMetrics Fm(PrimariesBox->font());
        for(int Pos=0; Pos<PrimariesBox->count(); Pos++)
        {
            int Temp=Fm.boundingRect(PrimariesBox->itemText(Pos)).width();
            if (Width<Temp)
                Width=Temp;
        }
        PrimariesBox->setStyleSheet(QString("QListView {min-width: %1px; }").arg(10+(Width*1.15)));
    }

    PrimariesBox->setCurrentIndex(PrimariesBox->findData(Primaries));

    if (!Transfer.isEmpty() && TransferBox->findData(Transfer)==-1)
        TransferBox->addItem(mp4_colr_Transfer(Transfer), Transfer);

    {
        int Width=0;
        QFontMetrics Fm(TransferBox->font());
        for(int Pos=0; Pos<TransferBox->count(); Pos++)
        {
            int Temp=Fm.boundingRect(TransferBox->itemText(Pos)).width();
            if (Width<Temp)
                Width=Temp;
        }
        TransferBox->setStyleSheet(QString("QListView {min-width: %1px; }").arg(10+(Width*1.15)));
    }

    TransferBox->setCurrentIndex(Transfer.isEmpty()?1:TransferBox->findData(Transfer));

    if (!Matrix.isEmpty() && MatrixBox->findData(Matrix)==-1)
        MatrixBox->addItem(mp4_colr_Matrix(Matrix), Matrix);

    {
        int Width=0;
        QFontMetrics Fm(MatrixBox->font());
        for(int Pos=0; Pos<MatrixBox->count(); Pos++)
        {
            int Temp=Fm.boundingRect(MatrixBox->itemText(Pos)).width();
            if (Width<Temp)
                Width=Temp;
        }
        MatrixBox->setStyleSheet(QString("QListView {min-width: %1px; }").arg(10+(Width*1.15)));
    }

    MatrixBox->setCurrentIndex(Matrix.isEmpty()?1:MatrixBox->findData(Matrix));
}

//---------------------------------------------------------------------------
void ColorDelegate::setModelData(QWidget* Editor,
                                 QAbstractItemModel* Model,
                                 const QModelIndex& Index) const
{
    QComboBox* PrimariesBox=Editor->findChild<QComboBox*>("PrimariesBox");
    QComboBox* TransferBox=Editor->findChild<QComboBox*>("TransferBox");
    QComboBox* MatrixBox=Editor->findChild<QComboBox*>("MatrixBox");

    if (!PrimariesBox || !TransferBox || !MatrixBox)
        return;

    QString Primaries=PrimariesBox->currentData().toString();
    QString Transfer=TransferBox->currentData().toString();
    QString Matrix=MatrixBox->currentData().toString();
    QString Value;

    if (Primaries.isEmpty())
        Value=Primaries;
    else
        Value=Primaries+","+Transfer+","+Matrix;

    QString OldValue = Model->data(Index, Qt::UserRole).toString();
    if(Value != OldValue)
    {
        Model->setData(Index, Value, Qt::UserRole);
        emit Value_Changed(Index.row());
    }
}

//---------------------------------------------------------------------------
void ColorDelegate::On_Primaries_Changed(int)
{
    QWidget* Widget=qobject_cast<QWidget*>(sender()->parent());
    if (!Widget)
        return;

    QComboBox* PrimariesBox=Widget->findChild<QComboBox*>("PrimariesBox");
    QComboBox* TransferBox=Widget->findChild<QComboBox*>("TransferBox");
    QComboBox* MatrixBox=Widget->findChild<QComboBox*>("MatrixBox");

    if (!PrimariesBox || !TransferBox || !MatrixBox)
        return;

    QString Primaries=PrimariesBox->currentData().toString();
    if (Primaries.isEmpty())
    {
        if (TransferBox->findData("")==-1)
            TransferBox->addItem("", "");

        TransferBox->setCurrentIndex(TransferBox->findData(""));
        TransferBox->setEnabled(false);

        if (MatrixBox->findData("")==-1)
            MatrixBox->addItem("", "");

        MatrixBox->setCurrentIndex(MatrixBox->findData(""));
        MatrixBox->setEnabled(false);
    }
    else
    {
        if (TransferBox->findData("")!=-1)
        {
            TransferBox->removeItem(TransferBox->findData(""));
            TransferBox->setCurrentIndex(0);
        }
        TransferBox->setEnabled(true);

        if (MatrixBox->findData("")!=-1)
        {
            MatrixBox->removeItem(MatrixBox->findData(""));
            MatrixBox->setCurrentIndex(0);
        }
        MatrixBox->setEnabled(true);
    }
}

//---------------------------------------------------------------------------
void ColorDelegate::On_Color_Changed(int)
{
    QWidget* Widget=qobject_cast<QWidget*>(sender()->parent());
    if (!Widget)
        return;

    emit commitData(Widget);
}

//---------------------------------------------------------------------------
CleanApertureDelegate::CleanApertureDelegate(QObject* Parent, Core* C) : QItemDelegate(Parent), C(C)
{
}

//---------------------------------------------------------------------------
QWidget* CleanApertureDelegate::createEditor(QWidget* Parent,
                                             const QStyleOptionViewItem& Option,
                                             const QModelIndex& Index) const
{
    Q_UNUSED(Option)
    Q_UNUSED(Index)

    QLineEdit *Editor = new QLineEdit(Parent);
    Editor->setValidator(new CleanApertureValidator());

    return Editor;
}

//---------------------------------------------------------------------------
void CleanApertureDelegate::updateEditorGeometry(QWidget* Editor,
                                                 const QStyleOptionViewItem& Option,
                                                 const QModelIndex& Index) const
{
    Q_UNUSED(Index)

    Editor->setGeometry(Option.rect);
}

//---------------------------------------------------------------------------
void CleanApertureDelegate::setEditorData(QWidget* Editor, const QModelIndex& Index) const
{
    qobject_cast<QLineEdit*>(Editor)->setText(Index.data(Qt::EditRole).toString());
}

//---------------------------------------------------------------------------
void CleanApertureDelegate::setModelData(QWidget* Editor,
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
PrimariesDelegate::PrimariesDelegate(QObject* Parent, Core* C) : QItemDelegate(Parent), C(C)
{
}

//---------------------------------------------------------------------------
QWidget* PrimariesDelegate::createEditor(QWidget* Parent,
                                             const QStyleOptionViewItem& Option,
                                             const QModelIndex& Index) const
{
    Q_UNUSED(Option)
    Q_UNUSED(Index)

    QTableWidget *Editor = new QTableWidget(Parent);

    Editor->setColumnCount(2);
    Editor->setHorizontalHeaderLabels(QStringList { "X", "Y" });
    Editor->setRowCount(4);
    Editor->setVerticalHeaderLabels(QStringList { "Red", "Green", "Blue", "White Point" });

    Editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    Editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    for(int Row = 0; Row < Editor->rowCount(); Row++)
    {
        for(int Col = 0; Col < Editor->columnCount(); Col++)
        {
            QLineEdit* LineEditor = new QLineEdit(Editor);
            QDoubleValidator* DoubleValidator = new QDoubleValidator(LineEditor);

            DoubleValidator->setLocale(QLocale(QLocale::C));
            LineEditor->setValidator(DoubleValidator);
            Editor->setCellWidget(Row, Col, LineEditor);
        }
    }

    return Editor;
}

//---------------------------------------------------------------------------
void PrimariesDelegate::updateEditorGeometry(QWidget* Editor,
                                                 const QStyleOptionViewItem& Option,
                                                 const QModelIndex& Index) const
{
    Q_UNUSED(Index)

    QTableWidget* TableWidget = qobject_cast<QTableWidget*>(Editor);

    int Width = TableWidget->contentsMargins().left() +
        TableWidget->contentsMargins().right() +
        TableWidget->verticalHeader()->length();

    for (int Col = 0; Col < TableWidget->columnCount(); Col++)
        Width += TableWidget->columnWidth(Col);

    int Height = TableWidget->contentsMargins().top() +
        TableWidget->contentsMargins().bottom() +
        TableWidget->horizontalHeader()->height();

    for (int Row = 0; Row < TableWidget->rowCount(); Row++)
        Height += TableWidget->rowHeight(Row);

    int wOffset = qobject_cast<QWidget*>(parent())->width() -
        Option.rect.x() -
        Width;

    if (wOffset > 0)
        wOffset = 0;
    else
        wOffset -= 20;

    int hOffset = qobject_cast<QWidget*>(parent())->height() -
        Option.rect.y() -
        Height;

    if (hOffset > 0)
        hOffset = 0;
    else
        hOffset -= 20;

    TableWidget->setGeometry(Option.rect.x() + wOffset, Option.rect.y() + hOffset, Width, Height);
}

//---------------------------------------------------------------------------
void PrimariesDelegate::setEditorData(QWidget* Editor, const QModelIndex& Index) const
{
    QTableWidget* TableWidget = qobject_cast<QTableWidget*>(Editor);

    QStringList Values = Index.data(Qt::EditRole).toString().split(',');
    qobject_cast<QLineEdit*>(TableWidget->cellWidget(0, 0))->setText(Values.value(0));
    qobject_cast<QLineEdit*>(TableWidget->cellWidget(0, 1))->setText(Values.value(1));
    qobject_cast<QLineEdit*>(TableWidget->cellWidget(1, 0))->setText(Values.value(2));
    qobject_cast<QLineEdit*>(TableWidget->cellWidget(1, 1))->setText(Values.value(3));
    qobject_cast<QLineEdit*>(TableWidget->cellWidget(2, 0))->setText(Values.value(4));
    qobject_cast<QLineEdit*>(TableWidget->cellWidget(2, 1))->setText(Values.value(5));
    qobject_cast<QLineEdit*>(TableWidget->cellWidget(3, 0))->setText(Values.value(6));
    qobject_cast<QLineEdit*>(TableWidget->cellWidget(3, 1))->setText(Values.value(7));
}

//---------------------------------------------------------------------------
void PrimariesDelegate::setModelData(QWidget* Editor,
                                         QAbstractItemModel* Model,
                                         const QModelIndex& Index) const
{
    QTableWidget* TableWidget = qobject_cast<QTableWidget*>(Editor);

    QString rX = qobject_cast<QLineEdit*>(TableWidget->cellWidget(0, 0))->text().trimmed();
    QString rY = qobject_cast<QLineEdit*>(TableWidget->cellWidget(0, 1))->text().trimmed();
    QString gX = qobject_cast<QLineEdit*>(TableWidget->cellWidget(1, 0))->text().trimmed();
    QString gY = qobject_cast<QLineEdit*>(TableWidget->cellWidget(1, 1))->text().trimmed();
    QString bX = qobject_cast<QLineEdit*>(TableWidget->cellWidget(2, 0))->text().trimmed();
    QString bY = qobject_cast<QLineEdit*>(TableWidget->cellWidget(2, 1))->text().trimmed();
    QString wX = qobject_cast<QLineEdit*>(TableWidget->cellWidget(3, 0))->text().trimmed();
    QString wY = qobject_cast<QLineEdit*>(TableWidget->cellWidget(3, 1))->text().trimmed();

    QStringList Values { rX, rY, gX, gY, bX, bY, wX, wY };

    bool Empty=true;
    for (QString& Value : Values)
    {
        if (Value.isEmpty())
            Value = "0";
        else
            Empty=false;
        
    }

    if (Empty)
        Values.clear();

    QString OldValue = Model->data(Index, Qt::EditRole).toString();
    QString Value = Values.join(',');

    if(Value != OldValue)
    {
        Model->setData(Index, Value);
        emit Value_Changed(Index.row());
    }
}

//---------------------------------------------------------------------------
LuminanceDelegate::LuminanceDelegate(QObject* Parent, Core* C) : QItemDelegate(Parent), C(C)
{
}

//---------------------------------------------------------------------------
QWidget* LuminanceDelegate::createEditor(QWidget* Parent,
                                             const QStyleOptionViewItem& Option,
                                             const QModelIndex& Index) const
{
    Q_UNUSED(Option)
    Q_UNUSED(Index)

    QTableWidget *Editor = new QTableWidget(Parent);

    Editor->setColumnCount(2);
    Editor->setHorizontalHeaderLabels(QStringList { "Min", "Max" });
    Editor->setRowCount(1);
    Editor->verticalHeader()->setVisible(false);

    Editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    Editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    for(int Row = 0; Row < Editor->rowCount(); Row++)
    {
        for(int Col = 0; Col < Editor->columnCount(); Col++)
        {
            QLineEdit* LineEditor = new QLineEdit(Editor);
            QDoubleValidator* DoubleValidator = new QDoubleValidator(LineEditor);

            DoubleValidator->setLocale(QLocale(QLocale::C));
            LineEditor->setValidator(DoubleValidator);
            Editor->setCellWidget(Row, Col, LineEditor);
        }
    }

    return Editor;
}

//---------------------------------------------------------------------------
void LuminanceDelegate::updateEditorGeometry(QWidget* Editor,
                                                 const QStyleOptionViewItem& Option,
                                                 const QModelIndex& Index) const
{
    Q_UNUSED(Index)

    QTableWidget* TableWidget = qobject_cast<QTableWidget*>(Editor);

    int Width = TableWidget->contentsMargins().left() +
        TableWidget->contentsMargins().right() +
        TableWidget->verticalHeader()->length();

    for (int Col = 0; Col < TableWidget->columnCount(); Col++)
        Width += TableWidget->columnWidth(Col);

    int Height = TableWidget->contentsMargins().top() +
        TableWidget->contentsMargins().bottom() +
        TableWidget->horizontalHeader()->height();

    for (int Row = 0; Row < TableWidget->rowCount(); Row++)
        Height += TableWidget->rowHeight(Row);

    int wOffset = qobject_cast<QWidget*>(parent())->width() -
        Option.rect.x() -
        Width;

    if (wOffset > 0)
        wOffset = 0;
    else
        wOffset -= 20;

    int hOffset = qobject_cast<QWidget*>(parent())->height() -
        Option.rect.y() -
        Height;

    if (hOffset > 0)
        hOffset = 0;
    else
        hOffset -= 20;

    TableWidget->setGeometry(Option.rect.x() + wOffset, Option.rect.y() + hOffset, Width, Height);
}

//---------------------------------------------------------------------------
void LuminanceDelegate::setEditorData(QWidget* Editor, const QModelIndex& Index) const
{
    QTableWidget* TableWidget = qobject_cast<QTableWidget*>(Editor);

    QStringList Values = Index.data(Qt::EditRole).toString().split(',');
    qobject_cast<QLineEdit*>(TableWidget->cellWidget(0, 0))->setText(Values.value(0));
    qobject_cast<QLineEdit*>(TableWidget->cellWidget(0, 1))->setText(Values.value(1));
}

//---------------------------------------------------------------------------
void LuminanceDelegate::setModelData(QWidget* Editor,
                                         QAbstractItemModel* Model,
                                         const QModelIndex& Index) const
{
    QTableWidget* TableWidget = qobject_cast<QTableWidget*>(Editor);

    QString Min = qobject_cast<QLineEdit*>(TableWidget->cellWidget(0, 0))->text().trimmed();
    QString Max = qobject_cast<QLineEdit*>(TableWidget->cellWidget(0, 1))->text().trimmed();

    if (Min.isEmpty() && !Max.isEmpty())
        Min = "0";
    if (Max.isEmpty() && !Min.isEmpty())
        Max = "0";

    QStringList Values { Min, Max };
    Values.removeAll({});

    QString OldValue = Model->data(Index, Qt::EditRole).toString();
    QString Value = Values.join(',');

    if(Value != OldValue)
    {
        Model->setData(Index, Value);
        emit Value_Changed(Index.row());
    }
}

//---------------------------------------------------------------------------

//***************************************************************************
// TableWidget
//***************************************************************************

//---------------------------------------------------------------------------
TechTableWidget::TechTableWidget(QWidget* Parent) : QTableWidget(Parent)
{
}

//---------------------------------------------------------------------------
void TechTableWidget::Setup(Core *C)
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
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    ApertureDelegate* ApertureEditor = new ApertureDelegate(this, C);
    connect(ApertureEditor, SIGNAL(Value_Changed(int)), this, SLOT(On_Value_Changed(int)));

    RatioDelegate* RatioEditor = new RatioDelegate(this, C);
    connect(RatioEditor, SIGNAL(Value_Changed(int)), this, SLOT(On_Value_Changed(int)));

    DoubleDelegate* DoubleEditor = new DoubleDelegate(this, C);
    connect(DoubleEditor, SIGNAL(Value_Changed(int)), this, SLOT(On_Value_Changed(int)));

    FieldsDelegate* Comma2Editor = new FieldsDelegate(C, this);
    connect(Comma2Editor, SIGNAL(Value_Changed(int)), this, SLOT(On_Value_Changed(int)));

    ColorDelegate* Comma3Editor = new ColorDelegate(C, this);
    connect(Comma3Editor, SIGNAL(Value_Changed(int)), this, SLOT(On_Value_Changed(int)));

    CleanApertureDelegate* CleanApertureEditor = new CleanApertureDelegate(this, C);
    connect(CleanApertureEditor, SIGNAL(Value_Changed(int)), this, SLOT(On_Value_Changed(int)));

    PrimariesDelegate* PrimariesEditor = new PrimariesDelegate(this, C);
    connect(PrimariesEditor, SIGNAL(Value_Changed(int)), this, SLOT(On_Value_Changed(int)));

    LuminanceDelegate* LuminanceEditor = new LuminanceDelegate(this, C);
    connect(PrimariesEditor, SIGNAL(Value_Changed(int)), this, SLOT(On_Value_Changed(int)));

    setItemDelegateForColumn(CLEF_COLUMN, qobject_cast<QAbstractItemDelegate*>(ApertureEditor));
    setItemDelegateForColumn(PROF_COLUMN, qobject_cast<QAbstractItemDelegate*>(ApertureEditor));
    setItemDelegateForColumn(ENOF_COLUMN, qobject_cast<QAbstractItemDelegate*>(ApertureEditor));
    setItemDelegateForColumn(PASP_COLUMN, qobject_cast<QAbstractItemDelegate*>(RatioEditor));
    setItemDelegateForColumn(WSCL_COLUMN, qobject_cast<QAbstractItemDelegate*>(DoubleEditor));
    setItemDelegateForColumn(FIEL_COLUMN, qobject_cast<QAbstractItemDelegate*>(Comma2Editor));
    setItemDelegateForColumn(COLR_COLUMN, qobject_cast<QAbstractItemDelegate*>(Comma3Editor));
    setItemDelegateForColumn(GAMA_COLUMN, qobject_cast<QAbstractItemDelegate*>(DoubleEditor));
    setItemDelegateForColumn(CLAP_COLUMN, qobject_cast<QAbstractItemDelegate*>(CleanApertureEditor));
    setItemDelegateForColumn(DPRI_COLUMN, qobject_cast<QAbstractItemDelegate*>(PrimariesEditor));
    setItemDelegateForColumn(DLUM_COLUMN, qobject_cast<QAbstractItemDelegate*>(LuminanceEditor));
    setItemDelegateForColumn(MCLL_COLUMN, qobject_cast<QAbstractItemDelegate*>(DoubleEditor));
    setItemDelegateForColumn(MFAL_COLUMN, qobject_cast<QAbstractItemDelegate*>(DoubleEditor));
}

//---------------------------------------------------------------------------
void TechTableWidget::Set_Display(int Row, bool Valid)
{
   if(Row > this->rowCount())
        return;

    QString FileName = item(Row, FILE_COLUMN)->data(Qt::EditRole).toString();

    string str = this->item(Row, MFAL_COLUMN)->text().toStdString();

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

            item(Row, Col)->setToolTip("Unable to edit this field: unsupported or invalid file");
        }

        std::string Message((*C->Get_Files())[FileName].H->PerFile_Error.str());
        item(Row, FILE_COLUMN)->setToolTip(Message.c_str());
        item(Row, FILE_COLUMN)->setBackgroundColor(QColor(255, 63, 63, 127));
        return;
    }
    else
    {
        QStringList Fields=item(Row, FIEL_COLUMN)->data(Qt::UserRole).toString().split(",");
        if (Fields.size()>1)
            item(Row, FIEL_COLUMN)->setData(Qt::DisplayRole, mp4_fiel_Fields(Fields[0]) + ", " + mp4_fiel_Detail(Fields[1]));

        QStringList Color=item(Row, COLR_COLUMN)->data(Qt::UserRole).toString().split(",");
        if (Color.size()>2)
            item(Row, COLR_COLUMN)->setData(Qt::DisplayRole, mp4_colr_Primaries(Color[0]) + ", " + mp4_colr_Transfer(Color[1]) + ", " + mp4_colr_Matrix(Color[2]));

        FileInfo F=(*C->Get_Files())[FileName];
        if (F.MetaData.clef != F.Previous.clef)
        {
            QFont Font = this->item(Row, CLEF_COLUMN)->font();
            Font.setBold(true);
            this->item(Row, CLEF_COLUMN)->setFont(Font);
        }
        if (F.MetaData.prof != F.Previous.prof)
        {
            QFont Font = this->item(Row, PROF_COLUMN)->font();
            Font.setBold(true);
            this->item(Row, PROF_COLUMN)->setFont(Font);
        }
        if (F.MetaData.enof != F.Previous.enof)
        {
            QFont Font = this->item(Row, ENOF_COLUMN)->font();
            Font.setBold(true);
            this->item(Row, ENOF_COLUMN)->setFont(Font);
        }
        if (F.MetaData.pasp != F.Previous.pasp)
        {
            QFont Font = this->item(Row, PASP_COLUMN)->font();
            Font.setBold(true);
            this->item(Row, PASP_COLUMN)->setFont(Font);
        }
        if (F.MetaData.wscl != F.Previous.wscl)
        {
            QFont Font = this->item(Row, WSCL_COLUMN)->font();
            Font.setBold(true);
            this->item(Row, WSCL_COLUMN)->setFont(Font);
        }
        if (F.MetaData.fiel != F.Previous.fiel)
        {
            QFont Font = this->item(Row, FIEL_COLUMN)->font();
            Font.setBold(true);
            this->item(Row, FIEL_COLUMN)->setFont(Font);
        }
        if (F.MetaData.colr != F.Previous.colr)
        {
            QFont Font = this->item(Row, COLR_COLUMN)->font();
            Font.setBold(true);
            this->item(Row, COLR_COLUMN)->setFont(Font);
        }
        if (F.MetaData.gama != F.Previous.gama)
        {
            QFont Font = this->item(Row, GAMA_COLUMN)->font();
            Font.setBold(true);
            this->item(Row, GAMA_COLUMN)->setFont(Font);
        }
        if (F.MetaData.clap != F.Previous.clap)
        {
            QFont Font = this->item(Row, CLAP_COLUMN)->font();
            Font.setBold(true);
            this->item(Row, CLAP_COLUMN)->setFont(Font);
        }
        if (F.MetaData.mdcv_primaries != F.Previous.mdcv_primaries)
        {
            QFont Font = this->item(Row, DPRI_COLUMN)->font();
            Font.setBold(true);
            this->item(Row, DPRI_COLUMN)->setFont(Font);
        }
        if (F.MetaData.mdcv_luminance != F.Previous.mdcv_luminance)
        {
            QFont Font = this->item(Row, DLUM_COLUMN)->font();
            Font.setBold(true);
            this->item(Row, DLUM_COLUMN)->setFont(Font);
        }
        if (F.MetaData.clli_maxcll != F.Previous.clli_maxcll)
        {
            QFont Font = this->item(Row, MCLL_COLUMN)->font();
            Font.setBold(true);
            this->item(Row, MCLL_COLUMN)->setFont(Font);
        }
        if (F.MetaData.clli_maxfall != F.Previous.clli_maxfall)
        {
            QFont Font = this->item(Row, MFAL_COLUMN)->font();
            Font.setBold(true);
            this->item(Row, MFAL_COLUMN)->setFont(Font);
        }
        if (F.MetaData.chan != F.Previous.chan)
        {
            QFont Font = this->item(Row, CHAN_COLUMN)->font();
            Font.setBold(true);
            this->item(Row, CHAN_COLUMN)->setFont(Font);
        }
    }
}

//---------------------------------------------------------------------------
void TechTableWidget::Update_Table()
{
    size_t Valid = 0;

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
        QTableWidgetItem* clef = new QTableWidgetItem(It->MetaData.clef);
        clef->setToolTip("Double-click for editing this field, \"WxH\" format.");
        QTableWidgetItem* prof = new QTableWidgetItem(It->MetaData.prof);
        prof->setToolTip("Double-click for editing this field, \"WxH\" format.");
        QTableWidgetItem* enof = new QTableWidgetItem(It->MetaData.enof);
        enof->setToolTip("Double-click for editing this field, \"WxH\" format.");
        QTableWidgetItem* pasp = new QTableWidgetItem(It->MetaData.pasp);
        pasp->setToolTip("Double-click for editing this field, \"x:y\" format.");
        QTableWidgetItem* wscl = new QTableWidgetItem(It->MetaData.wscl);
        wscl->setToolTip("Double-click for editing this field, real number.");
        QTableWidgetItem* fiel = new QTableWidgetItem();
        fiel->setData(Qt::UserRole, It->MetaData.fiel);
        fiel->setToolTip("Double-click for editing this field, \"fields,detail\" format.");
        QTableWidgetItem* colr = new QTableWidgetItem();
        colr->setData(Qt::UserRole, It->MetaData.colr);
        colr->setToolTip("Double-click for editing this field, \"primaries,transfer,matrix\" format.");
        QTableWidgetItem* gama = new QTableWidgetItem(It->MetaData.gama);
        gama->setToolTip("Double-click for editing this field, rean number.");
        QTableWidgetItem* clap = new QTableWidgetItem(It->MetaData.clap);
        clap->setToolTip("Double-click for editing this field, \"with:d,height:d,hOffset:d,vOffset:d\" format.\n(':d' (denominator) are optionnal')");
        QTableWidgetItem* dpri = new QTableWidgetItem(It->MetaData.mdcv_primaries);
        dpri->setToolTip("Double-click for editing this field, \"red x,red y,green x,green y,blue x,blu y,white point x,white point y\" format.");
        QTableWidgetItem* dlum = new QTableWidgetItem(It->MetaData.mdcv_luminance);
        dlum->setToolTip("Double-click for editing this field, \"min,max\" format.");
        QTableWidgetItem* mcll = new QTableWidgetItem(It->MetaData.clli_maxcll);
        mcll->setToolTip("Double-click for editing this field, real number.");
        QTableWidgetItem* mfal = new QTableWidgetItem(It->MetaData.clli_maxfall);
        mfal->setToolTip("Double-click for editing this field, real number.");
        QTableWidgetItem* chan = new QTableWidgetItem("...");
        chan->setData(Qt::UserRole, It->MetaData.chan);
        chan->setFlags(chan->flags() ^ Qt::ItemIsEditable);
        chan->setTextAlignment(Qt::AlignHCenter);
        chan->setToolTip("Double-click for editing this field.");

        setItem(Row, FILE_COLUMN, Name);
        setItem(Row, CLEF_COLUMN, clef);
        setItem(Row, PROF_COLUMN, prof);
        setItem(Row, ENOF_COLUMN, enof);
        setItem(Row, PASP_COLUMN, pasp);
        setItem(Row, WSCL_COLUMN, wscl);
        setItem(Row, FIEL_COLUMN, fiel);
        setItem(Row, COLR_COLUMN, colr);
        setItem(Row, GAMA_COLUMN, gama);
        setItem(Row, CLAP_COLUMN, clap);
        setItem(Row, DPRI_COLUMN, dpri);
        setItem(Row, DLUM_COLUMN, dlum);
        setItem(Row, MCLL_COLUMN, mcll);
        setItem(Row, MFAL_COLUMN, mfal);
        setItem(Row, CHAN_COLUMN, chan);

        if (It->Valid)
            Valid++;

        Set_Display(Row, It->Valid);
    }

    if (Valid)
        setStatusTip(QString::number(Valid)+ " editable files found, double-click on fields for editing then save");
    else
        setStatusTip("Drag and drop some MOV/MP4 files");
}

//---------------------------------------------------------------------------
void TechTableWidget::resizeEvent(QResizeEvent* Event)
{
    int Target = size().width() > 1920 ? size().width() : 1920;

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
            ColumnSize_Ratio[i] = ((float)columnWidth(i)) / Target;
            t -= ColumnSize_Ratio[i];
        }
        ColumnSize_Ratio[MAX_COLUMN] = t;
    }

    //Update
    qreal Total_New = Target;
    for (int i = 0; i < MAX_COLUMN; i++)
    {
        ColumnSize[i] = (int)(Total_New * ColumnSize_Ratio[i]);
        setColumnWidth(i, ColumnSize[i]);
    }

    //Call base resizeEvent to handle the vertical resizing
    QTableView::resizeEvent(Event);
}

//---------------------------------------------------------------------------
bool TechTableWidget::edit(const QModelIndex& Index, EditTrigger Trigger, QEvent* Event)
{
    if ((Trigger==DoubleClicked || Trigger==AnyKeyPressed) && Index.column()==CHAN_COLUMN)
    {
        QString Current=item(Index.row(), Index.column())->data(Qt::UserRole).toString();
        ChannelDialog* Edit=new ChannelDialog(Current, this);
        if (Edit->exec()==QDialog::Accepted && Edit->Data()!=Current)
        {
            item(Index.row(), Index.column())->setData(Qt::UserRole, Edit->Data());
            On_Value_Changed(Index.row());
            delete Edit; //Edit=NULL;
            return true;
        }
        else
        {
            delete Edit; //Edit=NULL;
            return false; //No change
        }
    }

    return QTableWidget::edit(Index, Trigger, Event);
}

//---------------------------------------------------------------------------
void TechTableWidget::On_Value_Changed(int Row)
{
    if(!C)
        return;

    QString FileName = item(Row, FILE_COLUMN)->data(Qt::EditRole).toString();

    MetaDataType* MetaData = C->Get_MetaData(FileName);
    if(!MetaData)
        return;

    MetaData->clef = item(Row, CLEF_COLUMN)->data(Qt::EditRole).toString();
    MetaData->prof = item(Row, PROF_COLUMN)->data(Qt::EditRole).toString();
    MetaData->enof = item(Row, ENOF_COLUMN)->data(Qt::EditRole).toString();
    MetaData->pasp = item(Row, PASP_COLUMN)->data(Qt::EditRole).toString();
    MetaData->wscl = item(Row, WSCL_COLUMN)->data(Qt::EditRole).toString();
    MetaData->fiel = item(Row, FIEL_COLUMN)->data(Qt::UserRole).toString();
    MetaData->colr = item(Row, COLR_COLUMN)->data(Qt::UserRole).toString();
    MetaData->gama = item(Row, GAMA_COLUMN)->data(Qt::EditRole).toString();
    MetaData->clap = item(Row, CLAP_COLUMN)->data(Qt::EditRole).toString();
    MetaData->mdcv_primaries = item(Row, DPRI_COLUMN)->data(Qt::EditRole).toString();
    MetaData->mdcv_luminance = item(Row, DLUM_COLUMN)->data(Qt::EditRole).toString();
    MetaData->clli_maxcll = item(Row, MCLL_COLUMN)->data(Qt::EditRole).toString();
    MetaData->clli_maxfall = item(Row, MFAL_COLUMN)->data(Qt::EditRole).toString();
    MetaData->chan = item(Row, CHAN_COLUMN)->data(Qt::UserRole).toString();

    emit Enable_Save();
    Update_Table();
}
