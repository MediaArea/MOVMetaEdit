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
void mp4_moov_trak_mdia_minf_gmhd_tmcd_tcmi::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi.count(Chunk.trak_Index))
        throw exception_read_block("2 moov trak mdia minf gmhd tmcd tcmi blocks");

    Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]=new global::block_moov_trak_mdia_minf_gmhd_tmcd_tcmi();

    if (Chunk.Content.Size<26)
        throw exception_read_block("Can not parse moov trak mdia minf gmhd tmcd tcmi (too small)");

    Read_Internal_ReadAllInBuffer();

    Get_B1(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->Version);
    if (Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->Version!=0)
        throw exception_read_block("Can not parse moov trak mdia minf gmhd tmcd tcmi (version not supported)");

    Get_B3(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->Flags);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->TextFont);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->TextFace);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->TextSize);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->Reserved);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->TextColorRed);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->TextColorGreen);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->TextColorBlue);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->BackgroundColorRed);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->BackgroundColorGreen);
    Get_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->BackgroundColorBlue);

    int8u FontNameSize;
    Get_B1(FontNameSize);
    if (Chunk.Content.Size!=25+(int64u)FontNameSize)
        throw exception_read_block("Can not parse moov trak mdia minf gmhd tmcd tcmi (invalid size)");

    Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->FontName.clear();
    Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->FontName.reserve(FontNameSize);
    for (int8u Pos=0; Pos<FontNameSize; ++Pos)
    {
        int8u Value;
        Get_B1(Value);
        Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->FontName.push_back((char)Value);
    }
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_gmhd_tmcd_tcmi::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi.find(Chunk.trak_Index)==Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi.end())
    {
        Chunk.Content.IsRemovable=true;
        return;
    }

    int64u FontNameSize=Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->FontName.size();
    if (FontNameSize>0xFF)
        throw exception_write("moov trak mdia minf gmhd tmcd tcmi font name too long");

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset=0;
    Chunk.Content.Size=25+FontNameSize;
    Chunk.Content.Buffer=new int8u[Chunk.Content.Size];

    Put_B1(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->Version);
    Put_B3(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->Flags);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->TextFont);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->TextFace);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->TextSize);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->Reserved);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->TextColorRed);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->TextColorGreen);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->TextColorBlue);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->BackgroundColorRed);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->BackgroundColorGreen);
    Put_B2(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->BackgroundColorBlue);
    Put_B1((int8u)FontNameSize);

    for (size_t Pos=0; Pos<Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->FontName.size(); ++Pos)
        Put_B1(Global->moov_trak_mdia_minf_gmhd_tmcd_tcmi[Chunk.trak_Index]->FontName[Pos]);

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_gmhd_tmcd_tcmi::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
