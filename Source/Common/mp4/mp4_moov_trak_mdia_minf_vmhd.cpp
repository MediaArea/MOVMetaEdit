/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include "Common/mp4/mp4_.h"
//---------------------------------------------------------------------------

//***************************************************************************
// Read
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_vmhd::Read_Internal ()
{
    //Reading
    Read_Internal_ReadAllInBuffer();


    uint8_t Version;
    Get_B1(Version);

    if (Version>0)
          throw exception_read_block("Can not parse moov trak mdia vmhd (version not supported)");

    Global->moov_trak.back()->IsVideo=true;
    if (Global->moov_trak_FirstVideoIndex==(size_t)-1)
        Global->moov_trak_FirstVideoIndex=Global->moov_trak.size()-1;
}
