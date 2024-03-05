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
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv::Read_Internal()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv.count(Chunk.trak_Index))
        throw exception_read_block("2 moov trak mdia minf stbl stsd xxxx mdcv blocks");

    Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunk.trak_Index] = new global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv();

    Read_Internal_ReadAllInBuffer();

    for (size_t c = 0; c < 8; c++)
    {
        int16u Temp;
        Get_B2(Temp);
        Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunk.trak_Index]->Primaries[c] = (double)Temp/50000;
    }

    int32u Temp;
    Get_B4(Temp);
    Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunk.trak_Index]->Luminance[0] = (double)Temp/10000;
    Get_B4(Temp);
    Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunk.trak_Index]->Luminance[1] = (double)Temp/10000;
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (!Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv.count(Chunk.trak_Index))
    {
        Chunk.Content.IsRemovable = true;
        return;
    }

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset = 0;
    Chunk.Content.Size = 24;
    Chunk.Content.Buffer = new int8u[Chunk.Content.Size];

    for (size_t c = 0; c < 8; c++)
        Put_B2((int16u)round(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunk.trak_Index]->Primaries[c]*50000));

    Put_B4((int32u)round(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunk.trak_Index]->Luminance[0]*10000));
    Put_B4((int32u)round(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunk.trak_Index]->Luminance[1]*10000));

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
