/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

 //---------------------------------------------------------------------------
#ifndef ConfigH
#define ConfigH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <string>
#include <iostream>
using namespace std;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//Write to terminal
inline void STRINGOUT(const string &Value)
{
    cout << Value << endl;
}

inline void TEXTOUT(const char* Value)
{
    cout << Value << endl;
}

enum ReturnValue
{
    ReturnValue_OK    = 0,
    ReturnValue_ERROR,
    ReturnValue_STOP,
    ReturnValue_ADD,
};

#endif

