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
void mp4_moov_trak_tkhd::Read_Internal()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    Read_Internal_ReadAllInBuffer();

    uint8_t Version;
    Get_B1(Version);

    size_t ExtraBytes = Version == 0 ? 0 : 8;

    uint32_t Width_Scale, Width, Height;
    Skip_XX(ExtraBytes+39);
    Get_B4(Width_Scale);
    Skip_XX(30);
    Get_B4(Width);
    Get_B4(Height);

    if (Chunk.trak_Index==0 && Version<= 1 &&Width && Height)
    {
        Global->moov_trak_tkhd=new global::block_moov_trak_tkhd();
        Global->moov_trak_tkhd->Width_Scale=((double)Width_Scale/(double)(1<<16));
        Global->moov_trak_tkhd->Width_Scale_Pos=ExtraBytes+40;
    }
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_tkhd::Modify_Internal()
{
    if (Chunk.Content.IsModified ||
        Chunk.trak_Index!=0 ||
        !Chunk.Content.Size ||
        !Global->moov_trak_tkhd ||
        Chunk.Content.Size<Global->moov_trak_tkhd->Width_Scale_Pos+4)
        return;

    Chunk.Content.Buffer_Offset = 0;

    Skip_XX(Global->moov_trak_tkhd->Width_Scale_Pos);
    Put_B4((uint32_t)(Global->moov_trak_tkhd->Width_Scale*(1<<16)));

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_tkhd::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
