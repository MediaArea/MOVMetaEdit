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
void mp4_moov_trak_mdia_minf_stbl_stsd_tmcd::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_mdia_minf_stbl_stsd_tmcd.find(Chunk.trak_Index)!=Global->moov_trak_mdia_minf_stbl_stsd_tmcd.end())
        throw exception_read_block("2 moov trak mdia minf stbl stsd tmcd blocks");

    Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]=new global::block_moov_trak_mdia_minf_stbl_stsd_tmcd();

    if (Chunk.Content.Size<26)
        throw exception_read_block("Can not parse moov trak mdia minf stbl stsd tmcd (too small)");

    Read_Internal_ReadAllInBuffer();

    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Reserved_1);
    Get_B2(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Reserved_2);
    Get_B2(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->DataReferenceIndex);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Reserved_3);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Flags);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->TimeScale);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->FrameDuration);
    Get_B1(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->NumberOfFrames);
    if (Chunk.Content.Size>=28)
    {
        Get_B3(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Reserved_4);
        Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Reserved_4_Size_Is_Triple=true;
    }
    else
    {
        int8u Temp;
        Get_B1(Temp);
        Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Reserved_4=Temp;
        Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Reserved_4_Size_Is_Triple=false;
    }
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_tmcd::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (Global->moov_trak_mdia_minf_stbl_stsd_tmcd.find(Chunk.trak_Index)==Global->moov_trak_mdia_minf_stbl_stsd_tmcd.end())
    {
        Chunk.Content.IsRemovable=true;
        return;
    }

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset=0;
    if (Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Reserved_4_Size_Is_Triple)
        Chunk.Content.Size=28;
    else
        Chunk.Content.Size=26;
    Chunk.Content.Buffer=new int8u[Chunk.Content.Size];

    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Reserved_1);
    Put_B2(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Reserved_2);
    Put_B2(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->DataReferenceIndex);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Reserved_3);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Flags);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->TimeScale);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->FrameDuration);
    Put_B1(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->NumberOfFrames);

    if (Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Reserved_4_Size_Is_Triple)
        Put_B3(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Reserved_4);
    else
        Put_B1(Global->moov_trak_mdia_minf_stbl_stsd_tmcd[Chunk.trak_Index]->Reserved_4);

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_tmcd::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
