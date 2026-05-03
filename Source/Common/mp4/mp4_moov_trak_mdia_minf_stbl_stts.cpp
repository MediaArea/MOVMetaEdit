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
void mp4_moov_trak_mdia_minf_stbl_stts::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_mdia_minf_stbl_stts.find(Chunk.trak_Index)!=Global->moov_trak_mdia_minf_stbl_stts.end())
        throw exception_read_block("2 moov trak mdia minf stbl stts blocks");

    Global->moov_trak_mdia_minf_stbl_stts[Chunk.trak_Index]=new global::block_moov_trak_mdia_minf_stbl_stts();

    if (Chunk.Content.Size<8)
        throw exception_read_block("Can not parse moov trak mdia minf stbl stts (too small)");

    Read_Internal_ReadAllInBuffer();

    Skip_XX(4); //Version+Flags

    int32u EntryCount;
    Get_B4(EntryCount);

    if (Chunk.Content.Size<8+(int64u)EntryCount*8)
        throw exception_read_block("Can not parse moov trak mdia minf stbl stts (invalid entry count)");

    Global->moov_trak_mdia_minf_stbl_stts[Chunk.trak_Index]->Entries.reserve(EntryCount);
    for (size_t Pos=0; Pos<EntryCount; Pos++)
    {
        Global->moov_trak_mdia_minf_stbl_stts[Chunk.trak_Index]->Entries.push_back(global::block_moov_trak_mdia_minf_stbl_stts::entry());
        Get_B4(Global->moov_trak_mdia_minf_stbl_stts[Chunk.trak_Index]->Entries.back().SampleCount);
        Get_B4(Global->moov_trak_mdia_minf_stbl_stts[Chunk.trak_Index]->Entries.back().SampleDuration);
    }
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stts::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (Global->moov_trak_mdia_minf_stbl_stts.find(Chunk.trak_Index)==Global->moov_trak_mdia_minf_stbl_stts.end())
    {
        Chunk.Content.IsRemovable=true;
        return;
    }

    int32u EntryCount=(int32u)Global->moov_trak_mdia_minf_stbl_stts[Chunk.trak_Index]->Entries.size();

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset=0;
    Chunk.Content.Size=4+4+(int64u)EntryCount*8; //Version+Flags, EntryCount, entries (8 bytes each)
    Chunk.Content.Buffer=new int8u[Chunk.Content.Size];

    Put_B4(0); //Version+Flags
    Put_B4(EntryCount);
    for (int32u Pos=0; Pos<EntryCount; Pos++)
    {
        Put_B4(Global->moov_trak_mdia_minf_stbl_stts[Chunk.trak_Index]->Entries[Pos].SampleCount);
        Put_B4(Global->moov_trak_mdia_minf_stbl_stts[Chunk.trak_Index]->Entries[Pos].SampleDuration);
    }

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stts::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
