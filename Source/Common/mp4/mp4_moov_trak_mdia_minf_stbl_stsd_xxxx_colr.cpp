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
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_colr::Read_Internal()
{
    if (Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr)
        throw exception_read_block("2 moov trak mdia minf stbl stsd xxxx colr blocks");

    Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr=new global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_colr();

    Read_Internal_ReadAllInBuffer();

    uint32_t Type;
    Get_B4(Type);

    if (Type!=0x6E636C63) //nclc
        throw exception_read_block("unsupported colr type");

    Get_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr->Primaries);
    Get_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr->Transfer);
    Get_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr->Matrix);
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_colr::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (!Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr)
    {
        Chunk.Content.IsRemovable = true;
        return;
    }

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset = 0;
    Chunk.Content.Size = 10;
    Chunk.Content.Buffer = new int8u[Chunk.Content.Size];

    Put_B4(0x6E636C63); // nclc
    Put_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr->Primaries);
    Put_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr->Transfer);
    Put_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr->Matrix);

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_colr::Write_Internal() 
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
