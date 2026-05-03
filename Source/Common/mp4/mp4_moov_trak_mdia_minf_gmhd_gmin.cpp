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
void mp4_moov_trak_mdia_minf_gmhd_gmin::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_mdia_minf_gmhd_gmin.count(Chunk.trak_Index))
        throw exception_read_block("2 moov trak mdia minf gmhd gmin blocks");

    Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]=new global::block_moov_trak_mdia_minf_gmhd_gmin();

    if (Chunk.Content.Size!=16)
        throw exception_read_block("Can not parse moov trak mdia minf gmhd gmin (invalid size)");

    Read_Internal_ReadAllInBuffer();

    Get_B1(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->Version);
    if (Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->Version!=0)
        throw exception_read_block("Can not parse moov trak mdia minf gmhd gmin (version not supported)");

    Get_B3(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->Flags);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->GraphicsMode);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->OpColorRed);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->OpColorGreen);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->OpColorBlue);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->Balance);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->Reserved);
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_gmhd_gmin::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (Global->moov_trak_mdia_minf_gmhd_gmin.find(Chunk.trak_Index)==Global->moov_trak_mdia_minf_gmhd_gmin.end())
    {
        Chunk.Content.IsRemovable=true;
        return;
    }

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset=0;
    Chunk.Content.Size=16;
    Chunk.Content.Buffer=new int8u[Chunk.Content.Size];

    Put_B1(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->Version);
    Put_B3(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->Flags);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->GraphicsMode);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->OpColorRed);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->OpColorGreen);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->OpColorBlue);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->Balance);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_gmin[Chunk.trak_Index]->Reserved);

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_gmhd_gmin::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
