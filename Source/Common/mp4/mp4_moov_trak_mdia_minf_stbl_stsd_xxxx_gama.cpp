/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include "Common/mp4/mp4_.h"
//---------------------------------------------------------------------------

#include <iostream>

//***************************************************************************
// Read
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_gama::Read_Internal()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama.count(Chunk.trak_Index))
        throw exception_read_block("2 moov trak mdia minf stbl stsd xxxx gama blocks");

    Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama[Chunk.trak_Index]=new global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_gama();

    Read_Internal_ReadAllInBuffer();

    uint32_t Gamma;
    Get_B4(Gamma);
    Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama[Chunk.trak_Index]->Gamma=((double)Gamma/(double)(1<<16));
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_gama::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (!Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama.count(Chunk.trak_Index))
    {
        Chunk.Content.IsRemovable = true;
        return;
    }

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset = 0;
    Chunk.Content.Size = 4;
    Chunk.Content.Buffer = new int8u[Chunk.Content.Size];

    Put_B4((uint32_t)(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama[Chunk.trak_Index]->Gamma*(1<<16)));

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_gama::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
