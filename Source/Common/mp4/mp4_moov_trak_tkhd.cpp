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
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
double Fixed16_16ToDouble(int32u Value)
{
    return (double)(int32s)Value / (double)(1<<16);
}

//---------------------------------------------------------------------------
int32u DoubleToFixed16_16(double Value)
{
    double Scaled=Value*(double)(1<<16);
    int32s Result;
    if (Scaled>=0)
        Result=(int32s)(Scaled+0.5);
    else
        Result=(int32s)(Scaled-0.5);
    return (int32u)Result;
}

//---------------------------------------------------------------------------
double Fixed2_30ToDouble(int32u Value)
{
    return (double)(int32s)Value / (double)(1<<30);
}

//---------------------------------------------------------------------------
int32u DoubleToFixed2_30(double Value)
{
    double Scaled=Value*(double)(1<<30);
    int32s Result;
    if (Scaled>=0)
        Result=(int32s)(Scaled+0.5);
    else
        Result=(int32s)(Scaled-0.5);
    return (int32u)Result;
}

//---------------------------------------------------------------------------
inline bool Is2_30(size_t Pos)
{
    return Pos==2 || Pos==5 || Pos==8;
}

//***************************************************************************
// Read
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_tkhd::Read_Internal()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_tkhd.count(Chunk.trak_Index))
        throw exception_read_block("2 moov trak tkhd blocks");

    global::block_moov_trak_tkhd* tkhd=new global::block_moov_trak_tkhd();
    Global->moov_trak_tkhd[Chunk.trak_Index]=tkhd;

    Read_Internal_ReadAllInBuffer();

    int16u Temp16;
    int32u Temp32;

    Get_B1(tkhd->Version);
    if (tkhd->Version>1)
        throw exception_read_block("moov trak tkhd version unsupported");

    if (Chunk.Content.Size!=(tkhd->Version==0?84:96))
        throw exception_read_block("moov trak tkhd invalid size");

    Get_B3(tkhd->Flags);
    if (tkhd->Version==0)
    {
        Get_B4(Temp32);
        tkhd->CreationTime=Temp32;
        Get_B4(Temp32);
        tkhd->ModificationTime=Temp32;
    }
    else
    {
        Get_B8(tkhd->CreationTime);
        Get_B8(tkhd->ModificationTime);
    }
    Get_B4(tkhd->TrackID);
    Get_B4(tkhd->Reserved_1);
    if (tkhd->Version==0)
    {
        Get_B4(Temp32);
        tkhd->Duration=Temp32;
    }
    else
    {
        Get_B8(tkhd->Duration);
    }
    Get_B4(tkhd->Reserved_2[0]);
    Get_B4(tkhd->Reserved_2[1]);
    Get_B2(Temp16);
    tkhd->Layer=(int16s)Temp16;
    Get_B2(Temp16);
    tkhd->AlternateGroup=(int16s)Temp16;
    Get_B2(Temp16);
    tkhd->Volume=(int16s)Temp16;
    Get_B2(tkhd->Reserved_3);
    for (size_t Pos=0; Pos<9; Pos++)
    {
        Get_B4(Temp32);
        tkhd->Matrix[Pos]=Is2_30(Pos) ? Fixed2_30ToDouble(Temp32) : Fixed16_16ToDouble(Temp32);
    }
    Get_B4(tkhd->Width);
    Get_B4(tkhd->Height);
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_tkhd::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (Global->moov_trak_tkhd.find(Chunk.trak_Index)==Global->moov_trak_tkhd.end())
    {
        Chunk.Content.IsRemovable=true;
        return;
    }

    global::block_moov_trak_tkhd* tkhd=Global->moov_trak_tkhd[Chunk.trak_Index];

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset=0;
    Chunk.Content.Size=tkhd->Version==0?84:96;
    Chunk.Content.Buffer=new int8u[Chunk.Content.Size];

    Put_B1(tkhd->Version);
    Put_B3(tkhd->Flags);
    if (tkhd->Version==0)
    {
        Put_B4((int32u)tkhd->CreationTime);
        Put_B4((int32u)tkhd->ModificationTime);
    }
    else
    {
        Put_B8(tkhd->CreationTime);
        Put_B8(tkhd->ModificationTime);
    }
    Put_B4(tkhd->TrackID);
    Put_B4(tkhd->Reserved_1);
    if (tkhd->Version==0)
        Put_B4((int32u)tkhd->Duration);
    else
        Put_B8(tkhd->Duration);
    Put_B4(tkhd->Reserved_2[0]);
    Put_B4(tkhd->Reserved_2[1]);
    Put_B2((int16u)tkhd->Layer);
    Put_B2((int16u)tkhd->AlternateGroup);
    Put_B2((int16u)tkhd->Volume);
    Put_B2(tkhd->Reserved_3);
    for (size_t Pos=0; Pos<9; Pos++)
        Put_B4(Is2_30(Pos) ? DoubleToFixed2_30(tkhd->Matrix[Pos]) : DoubleToFixed16_16(tkhd->Matrix[Pos]));
    Put_B4(tkhd->Width);
    Put_B4(tkhd->Height);

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_tkhd::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
