/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifndef Common_MacHelpersH
#define Common_MacHelpersH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "ZenLib/Ztring.h"

using namespace ZenLib;

//---------------------------------------------------------------------------
Ztring makeUniqueFileName();
Ztring makeTemporaryDirectoryForFile(const char *path);
bool deleteTemporaryDirectory(const char *path);
#endif
