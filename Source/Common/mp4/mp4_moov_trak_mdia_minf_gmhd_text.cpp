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
void mp4_moov_trak_mdia_minf_gmhd_text::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_mdia_minf_gmhd_text.count(Chunk.trak_Index))
        throw exception_read_block("2 moov trak mdia minf gmhd text blocks");

    Global->moov_trak_mdia_minf_gmhd_text[Chunk.trak_Index]=new global::block_moov_trak_mdia_minf_gmhd_text();

    if (Chunk.Content.Size)
        Read_Internal_ReadAllInBuffer();

    Global->moov_trak_mdia_minf_gmhd_text[Chunk.trak_Index]->Text.clear();
    Global->moov_trak_mdia_minf_gmhd_text[Chunk.trak_Index]->Text.reserve((size_t)Chunk.Content.Size);
    for (size_t Pos=0; Pos<(size_t)Chunk.Content.Size; ++Pos)
        Global->moov_trak_mdia_minf_gmhd_text[Chunk.trak_Index]->Text.push_back((char)Chunk.Content.Buffer[Pos]);
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_gmhd_text::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (Global->moov_trak_mdia_minf_gmhd_text.find(Chunk.trak_Index)==Global->moov_trak_mdia_minf_gmhd_text.end() ||
        Global->moov_trak_mdia_minf_gmhd_text[Chunk.trak_Index]->Text.empty())
    {
        Chunk.Content.IsRemovable=true;
        return;
    }

    int64u TextSize=Global->moov_trak_mdia_minf_gmhd_text[Chunk.trak_Index]->Text.size();

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset=0;
    Chunk.Content.Size=TextSize;
    Chunk.Content.Buffer=new int8u[Chunk.Content.Size];

    for (size_t Pos=0; Pos<TextSize; ++Pos)
        Put_B1(Global->moov_trak_mdia_minf_gmhd_text[Chunk.trak_Index]->Text[Pos]);

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_gmhd_text::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
