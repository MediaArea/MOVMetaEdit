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
void mp4_moov_trak_edts_elst::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_edts_elst.count(Chunk.trak_Index))
        throw exception_read_block("2 moov trak edts elst blocks");

    Global->moov_trak_edts_elst[Chunk.trak_Index]=new global::block_moov_trak_edts_elst();

    if (Chunk.Content.Size<8)
        throw exception_read_block("Can not parse moov trak edts elst (too small)");

    Read_Internal_ReadAllInBuffer();

    Get_B1(Global->moov_trak_edts_elst[Chunk.trak_Index]->Version);
    Get_B3(Global->moov_trak_edts_elst[Chunk.trak_Index]->Flags);

    int32u EntryCount;
    Get_B4(EntryCount);

    Global->moov_trak_edts_elst[Chunk.trak_Index]->NumberOfEntries=EntryCount;
    Global->moov_trak_edts_elst[Chunk.trak_Index]->Entries.reserve(EntryCount);

    if (Global->moov_trak_edts_elst[Chunk.trak_Index]->Version!=0)
        throw exception_read_block("Can not parse moov trak edts elst (version not supported)");

    for (int32u Pos=0; Pos<EntryCount; Pos++)
    {
        int32u MediaTimeU;
        int16u MediaRateIntegerU;

        Global->moov_trak_edts_elst[Chunk.trak_Index]->Entries.push_back(global::block_moov_trak_edts_elst::entry());
        Get_B4(Global->moov_trak_edts_elst[Chunk.trak_Index]->Entries.back().TrackDuration);
        Get_B4(MediaTimeU);
        Get_B2(MediaRateIntegerU);
        Get_B2(Global->moov_trak_edts_elst[Chunk.trak_Index]->Entries.back().MediaRateFraction);

        Global->moov_trak_edts_elst[Chunk.trak_Index]->Entries.back().MediaTime=(int32s)MediaTimeU;
        Global->moov_trak_edts_elst[Chunk.trak_Index]->Entries.back().MediaRateInteger=(int16s)MediaRateIntegerU;
    }
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_edts_elst::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (!Global->moov_trak_edts_elst.count(Chunk.trak_Index))
    {
        Chunk.Content.IsRemovable=true;
        return;
    }

    int32u EntryCount=(int32u)Global->moov_trak_edts_elst[Chunk.trak_Index]->Entries.size();

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset=0;
    Chunk.Content.Size=4+4+(int64u)EntryCount*12; //Version+Flags, EntryCount, entries (12 bytes each)
    Chunk.Content.Buffer=new int8u[Chunk.Content.Size];

    Put_B1(Global->moov_trak_edts_elst[Chunk.trak_Index]->Version);
    Put_B3(Global->moov_trak_edts_elst[Chunk.trak_Index]->Flags);
    Put_B4(EntryCount);

    for (int32u Pos=0; Pos<EntryCount; Pos++)
    {
        Put_B4(Global->moov_trak_edts_elst[Chunk.trak_Index]->Entries[Pos].TrackDuration);
        Put_B4((int32u)Global->moov_trak_edts_elst[Chunk.trak_Index]->Entries[Pos].MediaTime);
        Put_B2((int16u)Global->moov_trak_edts_elst[Chunk.trak_Index]->Entries[Pos].MediaRateInteger);
        Put_B2(Global->moov_trak_edts_elst[Chunk.trak_Index]->Entries[Pos].MediaRateFraction);
    }

    Global->moov_trak_edts_elst[Chunk.trak_Index]->NumberOfEntries=EntryCount;

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_edts_elst::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
