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
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxxUnknown::Read_Internal ()
{
    if (Chunk.Header.Name==Elements::moov_trak_mdia_minf_stbl_stsd_tmcd)
    {
        Global->moov_trak.back()->moov_trak_mdia_minf_stbl_stsd_tmcd_Present=true;
        Global->TimeCode_Track_Present=true;
    }
}
