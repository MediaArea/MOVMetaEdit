/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include <QDirIterator>
#include <QStringList>
#include <QFileInfo>
#include <QString>
#include <QPair>
#include <QDir>

#include "tablewidget.h"
#include "core.h"
#include "ZenLib/Ztring.h"
using namespace ZenLib;

//---------------------------------------------------------------------------
Core::Core()
{
}

//---------------------------------------------------------------------------
FileInfo Core::Read_Data(const QString &FileName, bool CheckFileName)
{
    FileInfo Current;

    //Paring the file
    Current.H = new mp4_Handler();
    Current.H->Open(FileName.toLocal8Bit().constData());
    if (Current.H->PerFile_Error.str().empty())
        Current.Valid = true;

    //UniversalAdId values
    string idregistry = Current.H->Get("com.universaladid.idregistry");
    string idvalue = Current.H->Get("com.universaladid.idvalue");
    Current.Previous = qMakePair(QString::fromUtf8(idregistry.c_str()), QString::fromUtf8(idvalue.c_str()));;

    if (Current.Valid)
    {
        if (CheckFileName && idregistry.empty() && idvalue.empty())
        {
            idregistry = "ad-id.org";

            //Trying from file name
            QString BaseName = QFileInfo(FileName).baseName();
            int Pos = 0;
            if (AdIdValidator().validate(BaseName, Pos) == QValidator::Acceptable)
            {
                idvalue = BaseName.toUtf8().constData();
                Current.Modified = true;
                Current.ValueValid = true;
            }
        }
        else
        {
            QString Value = QString::fromUtf8(idvalue.c_str());
            QValidator::State State = QValidator::Invalid;
            int Pos = 0;
            if (idregistry == "ad-id.org")
                State = AdIdValidator().validate(Value, Pos);
            else if (idregistry.empty())
                State = NoneValidator().validate(Value, Pos);
            else
                State = OtherValidator().validate(Value, Pos);

            if (State == QValidator::Acceptable)
                Current.ValueValid = true;
        }
    }

    Current.MetaData = qMakePair(QString::fromUtf8(idregistry.c_str()), QString::fromUtf8(idvalue.c_str()));;

    return Current;
}

//---------------------------------------------------------------------------
void Core::Add_File(const QString &FileName)
{
    //Adding file to the list
    Files.insert(FileName, Read_Data(FileName, true));
}

//---------------------------------------------------------------------------
size_t Core::Open_Files(const QString &FileName)
{
    if(!QFileInfo(FileName).exists())
        return 0;

    QStringList List;
    if(QFileInfo(FileName).isFile())
        List.push_back(FileName);
    else
    {
        QDirIterator It(FileName, QDir::Files, QDirIterator::Subdirectories);
        while(It.hasNext())
            List.push_back(It.next());
    }

    for(int Pos = 0; Pos < List.size(); Pos++)
        Add_File(List[Pos]);

    return List.size();
}

//---------------------------------------------------------------------------
MetaDataType* Core::Get_MetaData(const QString& FileName)
{
    if(Files.contains(FileName))
        return &Files[FileName].MetaData;

    return NULL;
}

//---------------------------------------------------------------------------
bool Core::Save_File(const QString& FileName)
{
    if (Files.contains(FileName))
    {
        FileInfo &F=Files[FileName];

        if (!F.ValueValid)
            return false;

        Ztring Registry, Value;
        Registry.From_UTF8(F.MetaData.first.toUtf8().constData());
        Value.From_UTF8(F.MetaData.second.toUtf8().constData());
        F.H->Set("com.universaladid.idregistry", Registry.To_Local());
        F.H->Set("com.universaladid.idvalue", Value.To_Local());
        F.H->Save();

        Files.insert(FileName, Read_Data(FileName, false));
    }

    return true;
}
