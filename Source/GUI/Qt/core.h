/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#ifndef CORE_H
#define CORE_H

#include "Common/mp4_Handler.h"
#include <QMap>
#include <QPair>
#include <QString>

//typedef QPair<QString, QString> MetaDataType;
struct MetaDataType
{
    QString AdIDRegistry;
    QString AdIDValue;
    bool AdIDValid;
    QString clef;
    QString prof;
    QString enof;
    QString pasp;
    QString wscl;
    QString fiel;
    QString colr;
    QString gama;
    QString clap;
    QString chan;

    MetaDataType()
        : AdIDValid(false)
    {}
};

struct FileInfo
{
    bool         Valid;
    MetaDataType Previous;
    MetaDataType MetaData;
    mp4_Handler* H;

    FileInfo()
        : Valid(false)
        , H(NULL)
    {}

    bool Modified()
    {
        if (MetaData.AdIDValid && 
           (MetaData.AdIDRegistry != Previous.AdIDRegistry ||
            MetaData.AdIDValue != Previous.AdIDValue))
            return true;

        if (MetaData.clef != Previous.clef ||
            MetaData.prof != Previous.prof ||
            MetaData.enof != Previous.enof ||
            MetaData.pasp != Previous.pasp ||
            MetaData.wscl != Previous.wscl ||
            MetaData.fiel != Previous.fiel ||
            MetaData.colr != Previous.colr ||
            MetaData.gama != Previous.gama ||
            MetaData.clap != Previous.clap ||
            MetaData.chan != Previous.chan)
            return true;

        return false;
    }
};

typedef QMap<QString, FileInfo> FileList;

class Core
{
public:
    Core();
    FileInfo Read_Data(const QString &FileName, bool CheckFileName=false);
    void Add_File(const QString &FileName);

    size_t Open_Files(const QString& FileName);

    MetaDataType* Get_MetaData(const QString& FileName);

    size_t Files_Count() { return Files.size(); }

    FileList* Get_Files() { return &Files; }

    bool Save_File(const QString& FileName);

private:
    FileList Files;
};

#endif // CORE_H
