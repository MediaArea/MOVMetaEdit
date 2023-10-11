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
    Current.Previous.AdIDRegistry = QString::fromStdString(idregistry);
    Current.Previous.AdIDValue = QString::fromStdString(idvalue);
    Current.Previous.clef = QString::fromStdString(Current.H->Get("clef"));
    Current.Previous.prof = QString::fromStdString(Current.H->Get("prof"));
    Current.Previous.enof = QString::fromStdString(Current.H->Get("enof"));
    Current.Previous.pasp = QString::fromStdString(Current.H->Get("pasp"));
    Current.Previous.wscl = QString::fromStdString(Current.H->Get("wscale"));
    Current.Previous.fiel = QString::fromStdString(Current.H->Get("fiel"));
    Current.Previous.colr = QString::fromStdString(Current.H->Get("colr"));
    Current.Previous.gama = QString::fromStdString(Current.H->Get("gama"));
    Current.Previous.clap = QString::fromStdString(Current.H->Get("clap"));
    Current.Previous.chan = QString::fromStdString(Current.H->Get("chan"));
    Current.Previous.mdcv_primaries = QString::fromStdString(Current.H->Get("display_primaries"));
    Current.Previous.mdcv_luminance = QString::fromStdString(Current.H->Get("luminance"));
    Current.Previous.clli_maxcll = QString::fromStdString(Current.H->Get("maximum_content_light_level"));
    Current.Previous.clli_maxfall = QString::fromStdString(Current.H->Get("maximum_frame_average_light_level"));

    bool Valid = false;

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
                Valid = true;
                idvalue = BaseName.toUtf8().constData();
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
                Valid = true;
        }
    }

    Current.MetaData.AdIDRegistry = QString::fromStdString(idregistry);
    Current.MetaData.AdIDValue = QString::fromStdString(idvalue);
    Current.MetaData.AdIDValid = Valid;
    Current.MetaData.clef = Current.Previous.clef;
    Current.MetaData.prof = Current.Previous.prof;
    Current.MetaData.enof = Current.Previous.enof;
    Current.MetaData.pasp = Current.Previous.pasp;
    Current.MetaData.wscl = Current.Previous.wscl;
    Current.MetaData.fiel = Current.Previous.fiel;
    Current.MetaData.colr = Current.Previous.colr;
    Current.MetaData.gama = Current.Previous.gama;
    Current.MetaData.clap = Current.Previous.clap;
    Current.MetaData.chan = Current.Previous.chan;
    Current.MetaData.mdcv_primaries = Current.Previous.mdcv_primaries;
    Current.MetaData.mdcv_luminance = Current.Previous.mdcv_luminance;
    Current.MetaData.clli_maxcll = Current.Previous.clli_maxcll;
    Current.MetaData.clli_maxfall = Current.Previous.clli_maxfall;

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

        if (F.MetaData.AdIDValid && !F.MetaData.AdIDRegistry.isEmpty() && !F.MetaData.AdIDValue.isEmpty() && (F.MetaData.AdIDRegistry != F.Previous.AdIDRegistry || F.MetaData.AdIDValue != F.Previous.AdIDValue))
        {
            Ztring Registry, Value;
            Registry.From_UTF8(F.MetaData.AdIDRegistry.toUtf8().constData());
            Value.From_UTF8(F.MetaData.AdIDValue.toUtf8().constData());
            F.H->Set("com.universaladid.idregistry", Registry.To_Local());
            F.H->Set("com.universaladid.idvalue", Value.To_Local());
        }
        if (F.MetaData.clef!=F.Previous.clef)
        {
            if (F.MetaData.clef.isEmpty())
                F.H->Remove("clef");
            else
                F.H->Set("clef", F.MetaData.clef.toStdString());
        }
        if (F.MetaData.prof!=F.Previous.prof)
        {
            if (F.MetaData.prof.isEmpty())
                F.H->Remove("prof");
            else
                F.H->Set("prof", F.MetaData.prof.toStdString());
        }
        if (F.MetaData.enof!=F.Previous.enof)
        {
            if (F.MetaData.enof.isEmpty())
                F.H->Remove("enof");
            else
                F.H->Set("enof", F.MetaData.enof.toStdString());
        }
        if (F.MetaData.pasp!=F.Previous.pasp)
        {
            if (F.MetaData.pasp.isEmpty())
                F.H->Remove("pasp");
            else
                F.H->Set("pasp", F.MetaData.pasp.toStdString());
        }
        if (F.MetaData.wscl!=F.Previous.wscl)
        {
            F.H->Set("wscale", F.MetaData.wscl.toStdString());
        }
        if (F.MetaData.fiel!=F.Previous.fiel)
        {
            if (F.MetaData.fiel.isEmpty())
                F.H->Remove("fiel");
            else
                F.H->Set("fiel", F.MetaData.fiel.toStdString());
        }
        if (F.MetaData.colr!=F.Previous.colr)
        {
            if (F.MetaData.colr.isEmpty())
                F.H->Remove("colr");
            else
                F.H->Set("colr", F.MetaData.colr.toStdString());
        }
        if (F.MetaData.gama!=F.Previous.gama)
        {
            if (F.MetaData.gama.isEmpty())
                F.H->Remove("gama");
            else
                F.H->Set("gama", F.MetaData.gama.toStdString());
        }
        if (F.MetaData.clap!=F.Previous.clap)
        {
            if (F.MetaData.clap.isEmpty())
                F.H->Remove("clap");
            else
                F.H->Set("clap", F.MetaData.clap.toStdString());
        }
        if (F.MetaData.chan!=F.Previous.chan)
        {
            if (F.MetaData.chan.isEmpty())
                F.H->Remove("chan");
            else
                F.H->Set("chan", F.MetaData.chan.toStdString());
        }
        if (F.MetaData.mdcv_primaries!=F.Previous.mdcv_primaries)
        {
            if (F.MetaData.mdcv_primaries.isEmpty() && F.MetaData.mdcv_luminance.isEmpty())
                F.H->Remove("mdcv");
            else if (F.MetaData.mdcv_primaries.isEmpty())
                F.H->Set("display_primaries", "0,0,0,0,0,0,0,0");
            else
                F.H->Set("display_primaries", F.MetaData.mdcv_primaries.toStdString());
        }
        if (F.MetaData.mdcv_luminance!=F.Previous.mdcv_luminance)
        {
            if (F.MetaData.mdcv_primaries.isEmpty() && F.MetaData.mdcv_luminance.isEmpty())
                F.H->Remove("mdcv");
            else if (F.MetaData.mdcv_primaries.isEmpty())
                F.H->Set("luminance", "-1,-1");
            else
                F.H->Set("luminance", F.MetaData.mdcv_luminance.toStdString());
        }
        if (F.MetaData.clli_maxcll!=F.Previous.clli_maxcll)
        {
            if (F.MetaData.clli_maxcll.isEmpty() && F.MetaData.clli_maxfall.isEmpty())
                F.H->Remove("clli");
            else if (F.MetaData.clli_maxcll.isEmpty())
                F.H->Set("maximum_content_light_level", "0");
            else
                F.H->Set("maximum_content_light_level", F.MetaData.clli_maxcll.toStdString());
        }
        if (F.MetaData.clli_maxfall!=F.Previous.clli_maxfall)
        {
            if (F.MetaData.clli_maxfall.isEmpty() && F.MetaData.clli_maxcll.isEmpty())
                F.H->Remove("clli");
            else if (F.MetaData.clli_maxfall.isEmpty())
                F.H->Set("maximum_frame_average_light_level", "0");
            else
                F.H->Set("maximum_frame_average_light_level", F.MetaData.clli_maxfall.toStdString());
        }

        F.H->Save();

        Files.insert(FileName, Read_Data(FileName, false));
    }

    return true;
}
