/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#ifndef CORE_H
#define CORE_H

#include <QMap>
#include <QString>

typedef QMap<QString, QString> MetaDataList;

struct File
{
    bool         Modified;
    MetaDataList MetaData;
};

typedef QMap<QString, File> FileList;

class Core
{
public:
    Core();

    // Fill Files list with FileName and fake data
    void Dummy_Handler(const QString& FileName);

    size_t Open_Files(const QString& FileName);

    MetaDataList* Get_MetaData(const QString& FileName);

    size_t Files_Count() { return Files.size(); }

    FileList* Get_Files() { return &Files; }

private:
    FileList Files;
};

#endif // CORE_H
