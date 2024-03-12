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
void mp4_moov_trak_mdia_mdhd::Read_Internal()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;
    Global->moov_trak_mdia_mdhd[Chunk.trak_Index]=new global::block_moov_trak_mdia_mdhd();

    Read_Internal_ReadAllInBuffer();

    int32u Temp32;

    Get_B1(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Version);

    if (Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Version>1)
        throw exception_read_block("moov trak mdia mdhd version unsupported");

    if (Chunk.Content.Size!=(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Version==0?24:36))
        throw exception_read_block("moov trak mdia mdhd invalid size");

    Get_B3(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Flags);
    if (Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Version==0)
    {
        Get_B4(Temp32);
        Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->CreationTime=Temp32;
        Get_B4(Temp32);
        Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->ModificationTime=Temp32;
    }
    else
    {
        Get_B8(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->CreationTime);
        Get_B8(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->ModificationTime);
    }
    Get_B4(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->TimeScale);
    if (Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Version==0)
    {
        Get_B4(Temp32);
        Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Duration=Temp32;
    }
    else
    {
        Get_B8(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Duration);
    }
    Get_B2(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Language);
    Get_B2(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Quality);
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_mdhd::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (Global->moov_trak_mdia_mdhd.find(Chunk.trak_Index)==Global->moov_trak_mdia_mdhd.end())
        return;

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset=0;
    Chunk.Content.Size=Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Version==0?24:36;
    Chunk.Content.Buffer=new int8u[Chunk.Content.Size];

    Put_B1(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Version);
    Put_B3(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Flags);
    if (Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Version==0)
    {
        Put_B4(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->CreationTime);
        Put_B4(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->ModificationTime);
    }
    else
    {
        Put_B8(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->CreationTime);
        Put_B8(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->ModificationTime);
    }
    Put_B4(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->TimeScale);
    if (Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Version==0)
        Put_B4(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Duration);
    else
        Put_B8(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Duration);
    Put_B2(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Language);
    Put_B2(Global->moov_trak_mdia_mdhd[Chunk.trak_Index]->Quality);

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_mdhd::Write_Internal() 
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
