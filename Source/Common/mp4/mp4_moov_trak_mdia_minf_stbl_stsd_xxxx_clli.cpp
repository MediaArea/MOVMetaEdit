/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include "Common/mp4/mp4_.h"
//---------------------------------------------------------------------------

#include <iostream>
#include <cmath>

//***************************************************************************
// Read
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_clli::Read_Internal()
{
    if (Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli)
        throw exception_read_block("2 moov trak mdia minf stbl stsd xxxx clli blocks");

    Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli = new global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_clli();

    Read_Internal_ReadAllInBuffer();

    int16u Temp;
    Get_B2(Temp);
    Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli->maximum_content_light_level=(double)Temp;
    Get_B2(Temp);
    Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli->maximum_frame_average_light_level=(double)Temp;
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_clli::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (!Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli)
    {
        Chunk.Content.IsRemovable = true;
        return;
    }

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset = 0;
    Chunk.Content.Size = 4;
    Chunk.Content.Buffer = new int8u[Chunk.Content.Size];

    Put_B2((int16u)round(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli->maximum_content_light_level));
    Put_B2((int16u)round(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli->maximum_frame_average_light_level));

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_clli::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
