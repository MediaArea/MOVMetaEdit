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
#define mp4_moov_trak_tapt_(name) \
void mp4_moov_trak_tapt_##name::Read_Internal() \
{ \
    if (Global->moov_trak_tapt_##name) \
        throw exception_read_block("2 moov trak tapt " #name " blocks"); \
\
    Global->moov_trak_tapt_##name=new global::block_moov_trak_tapt_xxxx(); \
\
    Read_Internal_ReadAllInBuffer(); \
\
    uint8_t Version; \
    Get_B1(Version); \
    Skip_XX(3); \
\
    uint32_t Width, Height; \
    Get_B4(Width); \
    Global->moov_trak_tapt_##name->Width=((double)Width/(double)(1<<16)); \
    Get_B4(Height); \
    Global->moov_trak_tapt_##name->Height=((double)Height/(double)(1<<16)); \
} \
\
void mp4_moov_trak_tapt_##name::Modify_Internal() \
{ \
    if (Chunk.Content.IsModified) \
        return; \
\
    if (!Global->moov_trak_tapt_##name) \
    { \
        Chunk.Content.IsRemovable = true; \
        return; \
    } \
\
    if (Chunk.Content.Buffer) \
        delete[] Chunk.Content.Buffer; \
    Chunk.Content.Buffer_Offset = 0; \
    Chunk.Content.Size = 12; \
    Chunk.Content.Buffer = new int8u[Chunk.Content.Size]; \
\
    Put_B1(0x00); \
    Put_B3(0x000000); \
    Put_B4((uint32_t)(Global->moov_trak_tapt_##name->Width*(1<<16))); \
    Put_B4((uint32_t)(Global->moov_trak_tapt_##name->Height*(1<<16))); \
\
    Chunk.Content.IsModified=true; \
    Chunk.Content.Size_IsModified=true; \
} \
\
void mp4_moov_trak_tapt_##name::Write_Internal() \
{ \
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size); \
}

mp4_moov_trak_tapt_(clef)
mp4_moov_trak_tapt_(prof)
mp4_moov_trak_tapt_(enof)
