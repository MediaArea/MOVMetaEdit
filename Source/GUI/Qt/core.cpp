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

//---------------------------------------------------------------------------
Core::Core()
{
}

//---------------------------------------------------------------------------
void Core::Dummy_Handler(const QString &FileName)
{

    File Current;
    MetaDataList MetaData;

    Current.Modified = false;

    MetaData.insert("ad-id.org", "");

    QString BaseName = QFileInfo(FileName).baseName();
    int Pos = 0;
    if(AdIdValidator().validate(BaseName, Pos) == QValidator::Acceptable)
    {
        MetaData["ad-id.org"] = BaseName;
        Current.Modified = true;
    }

    Current.MetaData = MetaData;
    Files.insert(FileName, Current);
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
        Dummy_Handler(List[Pos]);

    return List.size();
}

//---------------------------------------------------------------------------
MetaDataList* Core::Get_MetaData(const QString& FileName)
{
    if(Files.contains(FileName))
        return &Files[FileName].MetaData;

    return NULL;
}
