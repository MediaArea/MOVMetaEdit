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
void mp4_moov_trak_mdia_minf_stbl_stsd::Read_Internal ()
{
    //Reading
    if (Chunk.Content.Size<8)
        throw exception_read_block("Can not parse moov trak mdia minf stbl stsd (too small)");
    Chunk.Content.Before_Subs_Content_Size=8;
    int64u Content_Size=Chunk.Content.Size;
    Chunk.Content.Size=Chunk.Content.Before_Subs_Content_Size; // Trick to read only atom data
    Read_Internal_ReadAllInBuffer();
    Chunk.Content.Size=Content_Size;

    uint8_t Version;
    Get_B1(Version);
    if (Version>0)
          throw exception_read_block("Can not parse moov trak mdia minf stbl stsd (version not supported)");

    if (Global->moov_trak.back()->IsVideo)
    {
        SUBS_BEGIN();
            SUB_ELEMENT_DEFAULT(moov_trak_mdia_minf_stbl_stsd_xxxxVideo);
        SUBS_END_DEFAULT();
    }
    else if (Global->moov_trak.back()->IsSound)
    {
        SUBS_BEGIN();
            SUB_ELEMENT_DEFAULT(moov_trak_mdia_minf_stbl_stsd_xxxxSound);
        SUBS_END_DEFAULT();
    }
}
