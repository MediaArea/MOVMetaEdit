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
void mp4_moov_trak_mdia_minf_dinf_dref_url::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_mdia_minf_dinf_dref_url.count(Chunk.trak_Index))
        throw exception_read_block("2 moov trak mdia minf dinf dref url blocks");

    Global->moov_trak_mdia_minf_dinf_dref_url[Chunk.trak_Index]=new global::block_moov_trak_mdia_minf_dinf_dref_url();

    if (Chunk.Content.Size<4)
        throw exception_read_block("Can not parse moov trak mdia minf dinf dref url (too small)");

    Read_Internal_ReadAllInBuffer();

    Get_B1(Global->moov_trak_mdia_minf_dinf_dref_url[Chunk.trak_Index]->Version);
    Get_B3(Global->moov_trak_mdia_minf_dinf_dref_url[Chunk.trak_Index]->Flags);
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_dinf_dref_url::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (Global->moov_trak_mdia_minf_dinf_dref_url.find(Chunk.trak_Index)==Global->moov_trak_mdia_minf_dinf_dref_url.end())
    {
        Chunk.Content.IsRemovable=true;
        return;
    }

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset=0;
    Chunk.Content.Size=4;
    Chunk.Content.Buffer=new int8u[Chunk.Content.Size];

    Put_B1(Global->moov_trak_mdia_minf_dinf_dref_url[Chunk.trak_Index]->Version);
    Put_B3(Global->moov_trak_mdia_minf_dinf_dref_url[Chunk.trak_Index]->Flags);

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_dinf_dref_url::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
