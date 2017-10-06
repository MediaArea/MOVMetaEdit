/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <stdint.h>
#include "ZenLib/Dir.h"
#include "ZenLib/File.h"
using namespace std;
using namespace ZenLib;

class atom
{
    File*           F;

public:
    bool            IsOk;
    uint64_t        StreamOffset;
    uint64_t        HeaderSize;
    uint64_t        TotalSize;
    uint32_t        Code;
    uint8_t*        DataContent;
    size_t          DataSize;
    vector<atom*>   Subs;
    atom*           Up;

    atom(File* F_, atom* Up_);
    ~atom();

    bool Parse();
    atom* Next();
    bool DataLoad();
};
