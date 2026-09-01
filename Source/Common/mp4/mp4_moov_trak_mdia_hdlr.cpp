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
void mp4_moov_trak_mdia_hdlr::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_mdia_hdlr.count(Chunk.trak_Index))
        throw exception_read_block("2 moov trak mdia hdlr blocks");

    Global->moov_trak_mdia_hdlr[Chunk.trak_Index]=new global::block_moov_trak_xxxx_hdlr();

    if (Chunk.Content.Size<25)
        throw exception_read_block("Can not parse moov trak mdia hdlr (too small)");

    Read_Internal_ReadAllInBuffer();

    Get_B1(Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->Version);
    if (Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->Version!=0)
        throw exception_read_block("Can not parse moov trak mdia hdlr (version not supported)");

    Get_B3(Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->Flags);
    Get_B4(Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentType);
    Get_B4(Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentSubtype);
    Get_B4(Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentManufacturer);
    Get_B4(Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentFlags);
    Get_B4(Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentFlagsMask);

    int8u NameSize;
    Get_B1(NameSize);
    if (Chunk.Content.Size!=25+(int64u)NameSize)
        throw exception_read_block("Can not parse moov trak mdia hdlr (invalid size)");

    Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentName.clear();
    Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentName.reserve(NameSize);
    for (int8u Pos=0; Pos<NameSize; ++Pos)
    {
        int8u Value;
        Get_B1(Value);
        Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentName.push_back((char)Value);
    }
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_hdlr::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (Global->moov_trak_mdia_hdlr.find(Chunk.trak_Index)==Global->moov_trak_mdia_hdlr.end())
    {
        Chunk.Content.IsRemovable=true;
        return;
    }

    int64u NameSize=Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentName.size();

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset=0;
    Chunk.Content.Size=25+NameSize;
    Chunk.Content.Buffer=new int8u[Chunk.Content.Size];

    Put_B1(Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->Version);
    Put_B3(Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->Flags);
    Put_B4(Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentType);
    Put_B4(Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentSubtype);
    Put_B4(Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentManufacturer);
    Put_B4(Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentFlags);
    Put_B4(Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentFlagsMask);
    Put_B1((int8u)NameSize);

    for (size_t Pos=0; Pos<Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentName.size(); ++Pos)
        Put_B1(Global->moov_trak_mdia_hdlr[Chunk.trak_Index]->ComponentName[Pos]);

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_hdlr::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}