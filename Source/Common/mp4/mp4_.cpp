/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include "Common/mp4/mp4_.h"
//---------------------------------------------------------------------------

//***************************************************************************
// Top level element
//***************************************************************************

//---------------------------------------------------------------------------
mp4::mp4()
    :mp4_Base(NULL, 0)
{
    Global=new global();
}

//---------------------------------------------------------------------------
mp4::~mp4()
{
    delete Global; //Global=NULL
}

//---------------------------------------------------------------------------
void mp4::Read_Internal()
{
    SUBS_BEGIN();
        SUB_ELEMENT(mdat);
        SUB_ELEMENT(moov);
    SUBS_END();
}

