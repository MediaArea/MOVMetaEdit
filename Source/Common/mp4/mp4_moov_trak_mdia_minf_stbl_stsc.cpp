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
void mp4_moov_trak_mdia_minf_stbl_stsc::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_mdia_minf_stbl_stsc.count(Chunk.trak_Index))
        throw exception_read_block("2 moov trak mdia minf stbl stsc blocks");

    Global->moov_trak_mdia_minf_stbl_stsc[Chunk.trak_Index]=new global::block_moov_trak_mdia_minf_stbl_stsc();

    Read_Internal_ReadAllInBuffer();

    Get_B1(Global->moov_trak_mdia_minf_stbl_stsc[Chunk.trak_Index]->Version);
    Get_B3(Global->moov_trak_mdia_minf_stbl_stsc[Chunk.trak_Index]->Flags);

    int32u EntryCount;
    Get_B4(EntryCount);

    Global->moov_trak_mdia_minf_stbl_stsc[Chunk.trak_Index]->Entries.reserve(EntryCount);
    for (size_t Pos=0; Pos<EntryCount; Pos++)
    {
        Global->moov_trak_mdia_minf_stbl_stsc[Chunk.trak_Index]->Entries.push_back(global::block_moov_trak_mdia_minf_stbl_stsc::entry());
        Get_B4(Global->moov_trak_mdia_minf_stbl_stsc[Chunk.trak_Index]->Entries.back().FirstChunk);
        Get_B4(Global->moov_trak_mdia_minf_stbl_stsc[Chunk.trak_Index]->Entries.back().SamplesPerChunk);
        Get_B4(Global->moov_trak_mdia_minf_stbl_stsc[Chunk.trak_Index]->Entries.back().SampleDescriptionIndex);
    }
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsc::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (!Global->moov_trak_mdia_minf_stbl_stsc.count(Chunk.trak_Index))
    {
        Chunk.Content.IsRemovable=true;
        return;
    }

    int32u EntryCount=(int32u)Global->moov_trak_mdia_minf_stbl_stsc[Chunk.trak_Index]->Entries.size();

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset=0;
    Chunk.Content.Size=4+4+(int64u)EntryCount*12; //Version+Flags, EntryCount, entries (12 bytes each)
    Chunk.Content.Buffer=new int8u[Chunk.Content.Size];

    Put_B1(Global->moov_trak_mdia_minf_stbl_stsc[Chunk.trak_Index]->Version);
    Put_B3(Global->moov_trak_mdia_minf_stbl_stsc[Chunk.trak_Index]->Flags);
    Put_B4(EntryCount);
    for (int32u Pos=0; Pos<EntryCount; Pos++)
    {
        Put_B4(Global->moov_trak_mdia_minf_stbl_stsc[Chunk.trak_Index]->Entries[Pos].FirstChunk);
        Put_B4(Global->moov_trak_mdia_minf_stbl_stsc[Chunk.trak_Index]->Entries[Pos].SamplesPerChunk);
        Put_B4(Global->moov_trak_mdia_minf_stbl_stsc[Chunk.trak_Index]->Entries[Pos].SampleDescriptionIndex);
    }

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsc::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
