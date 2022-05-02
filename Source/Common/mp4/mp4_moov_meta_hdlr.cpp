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
void mp4_moov_meta_hdlr::Read_Internal ()
{
    //Integrity
    if (Global->moov_meta_hdlr)
        throw exception_read_block("2 moov meta hdlr blocks");
    Global->moov_meta_hdlr=new mp4_Base::global::block_moov_meta_hdlr;
    Global->moov_meta_hdlr->Present=true;

    //Reading
    Read_Internal_ReadAllInBuffer();

    int32u Handler_type;
    int8u Version;
    Get_B1( Version);
    if (Version)
        throw exception_read_block("moov meta version unsupported");
    Skip_XX(3+4); //Flags+Predefined
    Get_B4( Handler_type);
    if (Handler_type!=0x6D647461) //mdta
        throw exception_read_block("moov meta Handler type unsupported");
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_meta_hdlr::Modify_Internal()
{
    if (!Global->moov_meta_hdlr && (
        !Global->moov_meta_ilst &&
        !Global->moov_meta_keys &&
        Global->moov_meta_keys_NewKeys.empty() &&
        Global->moov_meta_ilst_NewValues.empty()))
    {
        Chunk.Content.IsRemovable = true;
        return;
    }

    if (Chunk.Content.Size)
    {
        return; //Nothing to do
    }

    //Creating buffer
    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset = 0;
    Chunk.Content.Size = 24;
    Chunk.Content.Buffer = new int8u[Chunk.Content.Size];

    Put_B4(0x00000000);
    Put_B4(0x00000000);
    Put_B4(0x6D647461);
    Put_B4(0x00000000);
    Put_B4(0x00000000);
    Put_B4(0x00000000);

    Chunk.Content.IsModified = true;
    Chunk.Content.Size_IsModified = true;
}

//***************************************************************************
// Write
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_meta_hdlr::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
