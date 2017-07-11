/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include "movedit_atomread.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include "ZenLib/Dir.h"
#include "ZenLib/File.h"
using namespace std;
using namespace ZenLib;

atom::atom(File* F_, atom* Up_) :
    IsOk(true),
    StreamOffset(0),
    HeaderSize(0),
    TotalSize(0),
    Code(0),
    DataContent(NULL),
    DataSize(0),
    Up(Up_),
    F(F_)
{
}

atom::~atom()
{
    for (vector<atom*>::iterator Sub = Subs.begin(); Sub != Subs.end(); Sub++)
        delete *Sub;
    delete[] DataContent;
}

bool atom::Parse()
{
    StreamOffset = F->Position_Get() - HeaderSize;

    uint8_t Header[8];
    if (F->Read(Header, 8) != 8)
    {
        cout << " Error while reading file (Header)" << endl;
        IsOk = false;
        return false;
    }

    TotalSize = BigEndian2int32u(Header);
    Code = BigEndian2int32u(Header + 4);
    if (TotalSize == 0)
    {
        TotalSize = 8 + F->Size_Get() - F->Position_Get();
        HeaderSize = 8;
    }
    else if (TotalSize == 1)
    {
        if (F->Read(Header, 8) != 8)
        {
            cout << " Error while reading file (Header)" << endl;
            IsOk = false;
            return false;
        }
        TotalSize = BigEndian2int64u(Header);
        HeaderSize = 16;
    }
    else if (TotalSize < 8)
    {
        cout << " Error while analyzing atom header" << endl;
        IsOk = false;
        return false;
    }
    else
        HeaderSize = 8;

    return true;
}

atom* atom::Next()
{
    int64u CurrentOffset = StreamOffset + HeaderSize + DataSize;
    for (vector<atom*>::iterator Sub = Subs.begin(); Sub != Subs.end(); Sub++)
        CurrentOffset += (*Sub)->TotalSize;
    if (CurrentOffset + 8 > StreamOffset + TotalSize)
        return NULL; //No more space
    F->GoTo(CurrentOffset);
    if (F->Position_Get() >= F->Size_Get())
        return NULL;

    atom* Atom = new atom(F, this);
    Subs.push_back(Atom);
    Atom->Parse();
    return Atom;
}

bool atom::DataLoad()
{
    if (TotalSize >= 0x1000000)
    {
        cout << "Unsupported atom size" << endl;
        return false;
    }

    DataSize = (size_t)(TotalSize - HeaderSize);
    DataContent = new uint8_t[DataSize];
    if (F->Read(DataContent, DataSize) != DataSize)
    {
        cout << " Error while reading file (Data)" << endl;
        return false;
    }

    return true;
}
