/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifndef HelpH
#define HelpH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "CLI/Config.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void Set_Program_Name(const char* Name);

//---------------------------------------------------------------------------
ReturnValue Help();
ReturnValue Help_AdID(bool WithExamples=true);
ReturnValue Help_PAR(bool WithExamples=true);
ReturnValue Usage();

#endif
