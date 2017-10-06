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
void mp4_moov_meta_keys::Read_Internal ()
{
    //Integrity
    if (Global->moov_meta_keys)
        throw exception_read_block("2 moov meta keys blocks");
    Global->moov_meta_keys=new mp4_Base::global::block_moov_meta_keys;

    //Reading
    Read_Internal_ReadAllInBuffer();

    int32u Entry_count;
    int8u Version;
    Get_B1(Version);
    if (Version)
        throw exception_read_block("moov meta keys version unsupported");
    Skip_XX(3); //Flags
    Get_B4(Entry_count);
    while (Chunk.Content.Buffer_Offset<Chunk.Content.Size)
    {
        int32u Key_size, Key_namespace;
        Get_B4(Key_size);
        if (Key_size<8)
            throw exception_read_block("moov meta keys Key_size invalid");
        string Key_value;
        Get_B4(Key_namespace);
        Get_String(Key_size-8, Key_value);
        if (Key_namespace!=0x6D647461 && (Key_value=="com.universaladid.idregistry" || Key_value=="com.universaladid.idvalue")) //mdta
            throw exception_read_block("UniversalAdId fields not mdta unsupported");

        Global->moov_meta_keys->Keys.push_back(global::block_moov_meta_key(Key_size, Key_namespace, Chunk.Content.Buffer+(size_t)Chunk.Content.Buffer_Offset-(Key_size-8)));
        Global->moov_meta_keys_AlreadyPresent++;
    }

    if (Entry_count!=Global->moov_meta_keys_AlreadyPresent)
        throw exception_read_block("moov meta keys incoherant");
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_meta_keys::Modify_Internal()
{
    if (!Global->moov_meta_keys_AlreadyPresent && Global->moov_meta_keys_NewKeys.empty())
    {
        Chunk.Content.IsRemovable = true;
        return;
    }

    if (Global->moov_meta_keys_NewKeys.empty())
        return; //No change

    //Compute of content size to add
    size_t Size_ToAdd=0;
    for (size_t i=0; i<Global->moov_meta_keys_NewKeys.size(); i++)
        Size_ToAdd+=8+Global->moov_meta_keys_NewKeys[i].size();

    //Changing count
    if (Chunk.Content.Size)
    {
        //Version+Flags+Count
        Chunk.Content.Buffer_Offset=4; //Skipping Version+Flags
        Put_B4((int32u)(Global->moov_meta_keys_AlreadyPresent+Global->moov_meta_keys_NewKeys.size())); //Count

        //Creating buffer
        Chunk.Content.Buffer_Offset=Chunk.Content.Size;
        int8u* t=new int8u[Chunk.Content.Size+Size_ToAdd];
        memcpy(t, Chunk.Content.Buffer, Chunk.Content.Size);
        delete[] Chunk.Content.Buffer;
        Chunk.Content.Buffer=t;
        Chunk.Content.Size+=Size_ToAdd;
    }
    else
    {
        //Creating buffer
        Chunk.Content.Buffer_Offset=0;
        Chunk.Content.Size=8+Size_ToAdd;
        Chunk.Content.Buffer=new int8u[Chunk.Content.Size];

        //Version+Flags+Count
        Put_B4(0x00000000); //Version+Flags
        Put_B4((int32u)(Global->moov_meta_keys_NewKeys.size())); //Count
    }

    //New keys
    for (size_t i=0; i<Global->moov_meta_keys_NewKeys.size(); i++)
    {
        Put_B4((int32u)(8+Global->moov_meta_keys_NewKeys[i].size()));
        Put_B4(0x6D647461); //mdta
        Put_String(Global->moov_meta_keys_NewKeys[i].size(), Global->moov_meta_keys_NewKeys[i]);
    }
    Global->moov_meta_keys_AlreadyPresent+=Global->moov_meta_keys_NewKeys.size();
    Global->moov_meta_keys_NewKeys.clear();

    Chunk.Content.IsModified = true;
    Chunk.Content.Size_IsModified = true;
}

//***************************************************************************
// Write
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_meta_keys::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
