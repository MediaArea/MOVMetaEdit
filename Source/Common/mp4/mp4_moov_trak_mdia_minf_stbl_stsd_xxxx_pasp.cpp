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
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_pasp::Read_Internal()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp.count(Chunk.trak_Index))
        throw exception_read_block("2 moov trak mdia minf stbl stsd xxxx pasp blocks");

    Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp[Chunk.trak_Index]=new global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_pasp();

    Read_Internal_ReadAllInBuffer();

    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp[Chunk.trak_Index]->hSpacing);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp[Chunk.trak_Index]->vSpacing);
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_pasp::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (!Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp.count(Chunk.trak_Index))
    {
        Chunk.Content.IsRemovable = true;
        return;
    }

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset = 0;
    Chunk.Content.Size = 8;
    Chunk.Content.Buffer = new int8u[Chunk.Content.Size];

    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp[Chunk.trak_Index]->hSpacing);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp[Chunk.trak_Index]->vSpacing);

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_pasp::Write_Internal() 
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
