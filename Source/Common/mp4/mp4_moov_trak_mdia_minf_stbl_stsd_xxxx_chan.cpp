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
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_chan::Read_Internal()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]=new global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_chan();

    Read_Internal_ReadAllInBuffer();

    Get_B1(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Version);
    Get_B3(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Flags);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->ChannelLayoutTag);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->ChannelBitmap);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->NumberChannelDescriptions);

    if (Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->NumberChannelDescriptions)
    {
        Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Descriptions.push_back(global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_chan::description());
        for (size_t Pos=0; Pos<Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->NumberChannelDescriptions; Pos++)
        {
            Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Descriptions.back().ChannelLabel);
            Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Descriptions.back().ChannelFlags);
            Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Descriptions.back().Coordinates0);
            Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Descriptions.back().Coordinates1);
            Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Descriptions.back().Coordinates2);
        }
    }
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_chan::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.find(Chunk.trak_Index)==Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.end())
    {
        Chunk.Content.IsRemovable = true;
        return;
    }

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset = 0;
    Chunk.Content.Size = 16 + (20 * Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->NumberChannelDescriptions);
    Chunk.Content.Buffer = new int8u[Chunk.Content.Size];

    size_t i = Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->NumberChannelDescriptions;

    Put_B1(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Version);
    Put_B3(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Flags);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->ChannelLayoutTag);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->ChannelBitmap);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->NumberChannelDescriptions);
    for (size_t Pos=0; Pos<Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Descriptions.size(); Pos++)
    {
        Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Descriptions[Pos].ChannelLabel);
        Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Descriptions[Pos].ChannelFlags);
        Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Descriptions[Pos].Coordinates0);
        Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Descriptions[Pos].Coordinates1);
        Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Chunk.trak_Index]->Descriptions[Pos].Coordinates2);
    }

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_chan::Write_Internal() 
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
