/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifndef AdIDH
#define AdIDH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "CLI/Config.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class AdID
{
public:
    const string &Get();
    bool Set(const string &Data);
    const string &GetRegistry();
    bool SetRegistry(const string &Data);
    bool SetName(const string &Data);
    bool Validate();

    string ErrorMessage;

private:
    string Value;
    string Registry;
    string Name;
};

#endif
