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
void mp4_moov_trak_mdia_minf_stbl_stsz::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_mdia_minf_stbl_stsz.count(Chunk.trak_Index))
        throw exception_read_block("2 moov trak mdia minf stbl stsz blocks");

    Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]=new global::block_moov_trak_mdia_minf_stbl_stsz();

    Read_Internal_ReadAllInBuffer();

    Get_B1(Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->Version);
    Get_B3(Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->Flags);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->SampleSize);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->SampleCount);

    if (!Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->SampleSize)
    {
        Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->Entries.reserve(Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->SampleCount);
        for (size_t Pos=0; Pos<Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->SampleCount; Pos++)
        {
            Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->Entries.push_back(0);
            Get_B4(Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->Entries.back());
        }
    }
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsz::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (!Global->moov_trak_mdia_minf_stbl_stsz.count(Chunk.trak_Index))
    {
        Chunk.Content.IsRemovable=true;
        return;
    }

    int64u EntryDataSize=(Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->SampleSize==0)?((int64u)Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->SampleCount*4):0;

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset=0;
    Chunk.Content.Size=4+4+4+EntryDataSize; //Version+Flags, SampleSize, SampleCount, [entries]
    Chunk.Content.Buffer=new int8u[Chunk.Content.Size];

    Put_B1(Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->Version);
    Put_B3(Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->Flags);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->SampleSize);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->SampleCount);
    if (!Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->SampleSize)
    {
        for (int32u Pos=0; Pos<Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->SampleCount; Pos++)
            Put_B4(Global->moov_trak_mdia_minf_stbl_stsz[Chunk.trak_Index]->Entries[Pos]);
    }

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsz::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
