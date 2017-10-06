/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Common/AdID.h"
#include "ZenLib/FileName.h"
#include <sstream>
using namespace ZenLib;
//---------------------------------------------------------------------------

//***************************************************************************
//
//***************************************************************************

//---------------------------------------------------------------------------
const string &AdID::Get()
{
    return Value;
}

bool AdID::Set(const string &Data)
{
    Value=Data;

    return true;
}

//---------------------------------------------------------------------------
const string &AdID::GetRegistry()
{
    return Registry;
}

//---------------------------------------------------------------------------
bool AdID::SetRegistry(const string &Data)
{
    Registry=Data;

    return true;
}

//---------------------------------------------------------------------------
bool AdID::SetName(const string &Data)
{
    Name=Data;

    return true;
}

//---------------------------------------------------------------------------
bool AdID::Validate()
{
    if (Registry.empty())
        Registry="ad-id.org";

    if (Registry=="ad-id.org")
    {
        if (Value=="auto")
            Value=FileName(Ztring().From_UTF8(Name.c_str())).Name_Get().To_UTF8();

        if (Value.size()<11 || Value.size()>12)
        {
            stringstream t;
            t << "For registry ad-id.org, UniversalAdId length must be between 11 and 12 (actual: " << Value.size() << ")";
            ErrorMessage = t.str();
            return false;
        }

        for(size_t i=0; i<Value.size(); i++)
            if (!((Value[i]>='A' && Value[i]<='Z') || (Value[i]>='0' && Value[i]<='9')))
            {
                stringstream t;
                t << "For registry ad-id.org, UniversalAdId length must contain character in range A-Z 0-9 (not the case for pos " << i << ")";
                ErrorMessage = t.str();
                return false;
            }
    }
    else
    {
        if (Value.size()>255)
        {
            stringstream t;
            t << "UniversalAdId length should not be more than 255 (actual: " << Value.size() << ")";
            ErrorMessage = t.str();
            return false;
        }
    }

    return true;
}
