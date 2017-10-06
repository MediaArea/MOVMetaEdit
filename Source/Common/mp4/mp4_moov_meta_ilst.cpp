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
void mp4_moov_meta_ilst::Read_Internal ()
{
    //Integrity
    if (Global->moov_meta_ilst)
        throw exception_read_block("2 moov meta ilst blocks");
    if (!Global->moov_meta_keys)
        throw exception_read_block("moov meta ilst is expected after moov meta keys");
    Global->moov_meta_ilst=new mp4_Base::global::block_moov_meta_ilst;

    //Reading
    Read_Internal_ReadAllInBuffer();

    //Parsing
    while (Chunk.Content.Buffer_Offset < Chunk.Content.Size)
    {
        int32u Key_size, Key_Pos;
        Get_B4(Key_size);
        if (Key_size<8)
            throw exception_read_block("moov meta ilst atom size invalid");
        Get_B4(Key_Pos);
        if (!Key_Pos || Key_Pos>Global->moov_meta_keys->Keys.size())
            throw exception_read_block("moov meta ilst atom name invalid");
        global::block_moov_meta_key &key = Global->moov_meta_keys->Keys[Key_Pos-1];
        size_t PreviousSize=Global->moov_meta_ilst->Items.size();
        if (key.namespace_==0x6D647461) //mdta
        {
            string value((char*)key.value, key.size-8) ;
            if (value=="com.universaladid.idregistry"
             || value=="com.universaladid.idvalue")
            {
                size_t BufferOffset_StartOfBlock=(size_t)Chunk.Content.Buffer_Offset;
                int64u SizeMax=Chunk.Content.Buffer_Offset+Key_size-8;
                while (Chunk.Content.Buffer_Offset<SizeMax)
                {
                    int32u size, name;
                    Get_B4(size);
                    if (size<8)
                        throw exception_read_block("moov meta ilst atom size invalid");
                    Get_B4(name);
                    if (name==0x64617461) //data
                    {
                        if (size<16)
                            throw exception_read_block("moov meta ilst data atom size invalid");
                        int32u format, locale;
                        Get_B4(format);
                        if (format!=1)
                            throw exception_read_block("universaladid format out of specs");
                        Get_B4(locale);
                        if (locale)
                            throw exception_read_block("universaladid locale out of specs");
                        Skip_XX(size-16);
                        Global->moov_meta_ilst->Items.push_back(global::block_moov_meta_list(Key_size, Key_Pos, Chunk.Content.Buffer+BufferOffset_StartOfBlock, (size_t)Chunk.Content.Buffer_Offset-BufferOffset_StartOfBlock-size, size));
                    }
                }
            }
            else
                Skip_XX(Key_size-8);
        }

        if (PreviousSize==Global->moov_meta_ilst->Items.size())
        {
            //Skip
            Global->moov_meta_ilst->Items.push_back(global::block_moov_meta_list(Key_size, Key_Pos, Chunk.Content.Buffer+(size_t)Chunk.Content.Buffer_Offset-(Key_size-8)));
        }
    }
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_meta_ilst::Modify_Internal()
{
    if (!Global->moov_meta_ilst_AlreadyPresent && Global->moov_meta_ilst_NewValues.empty())
    {
        //Chunk.Content.IsRemovable = true; PROBEM to fix!
        //return;
    }

    //if (Global->moov_meta_ilst_NewValues.empty())
    //    return; //No change

    if (Chunk.Content.IsModified)
        return; //Already done, can be done once only

    // Creating buffer
    int8u* OldBuffer=Chunk.Content.Buffer;
    Chunk.Content.Size=0;
    if (Global->moov_meta_ilst)
        for (size_t i=0; i<Global->moov_meta_ilst->Items.size(); i++)
        {
            global::block_moov_meta_list& Item=Global->moov_meta_ilst->Items[i];
            Chunk.Content.Size+=Item.size;
            if (Item.data_Size && Item.ToBeReplacedBy_Modified)
            {
                Chunk.Content.Size-=Item.data_Size-16;
                Chunk.Content.Size+=Item.ToBeReplacedBy.size();
            }
        }
    for (size_t i=0; i<Global->moov_meta_ilst_NewValues.size(); i++)
        Chunk.Content.Size+=8+8+8+Global->moov_meta_ilst_NewValues[i].size();

    // Copy or replace data
    int8u* t=Chunk.Content.Buffer;;
    Chunk.Content.Buffer=new int8u[Chunk.Content.Size];
    Chunk.Content.Buffer_Offset=0;
    if (Global->moov_meta_ilst)
        for (size_t i=0; i<Global->moov_meta_ilst->Items.size(); i++)
        {
            global::block_moov_meta_list& Item=Global->moov_meta_ilst->Items[i];
            if (Item.data_Size && Item.ToBeReplacedBy_Modified)
            {
                Put_B4((int32u)(Item.size-(Item.data_Size-16)+Item.ToBeReplacedBy.size()));
                Put_B4((int32u)(Item.name));
                if (Item.data_Pos)
                {
                    memcpy(Chunk.Content.Buffer+Chunk.Content.Buffer_Offset, Item.value, Item.data_Pos);
                    Chunk.Content.Buffer_Offset+=Item.data_Pos;
                }
                Put_B4(16+Item.ToBeReplacedBy.size()); //data
                Put_B4(0x64617461); //data
                Put_B4(0x00000001); //UTF-8
                Put_B4(0x00000000); //No locale
                Put_String(Item.ToBeReplacedBy.size(), Item.ToBeReplacedBy);
                if (Item.size-8-(Item.data_Pos+Item.data_Size))
                {
                    memcpy(Chunk.Content.Buffer+Chunk.Content.Buffer_Offset, Item.value+(Item.data_Pos+Item.data_Size), Item.size-8-(Item.data_Pos+Item.data_Size));
                    Chunk.Content.Buffer_Offset+=Item.size-8-(Item.data_Pos+Item.data_Size);
                }
                Item.ToBeReplacedBy.clear();
                Item.ToBeReplacedBy_Modified=false;
            }
            else
            {
                Put_B4((int32u)(Item.size));
                Put_B4((int32u)(Item.name));
                memcpy(Chunk.Content.Buffer+Chunk.Content.Buffer_Offset, Item.value, Item.size-8);
                Chunk.Content.Buffer_Offset+=Item.size-8;
            }
        }

    // Add data
    for (size_t i=0; i<Global->moov_meta_ilst_NewValues.size(); i++)
    {
        Put_B4((int32u)(8+8+8+Global->moov_meta_ilst_NewValues[i].size()));
        Put_B4((int32u)(1+Global->moov_meta_keys_AlreadyPresent-Global->moov_meta_ilst_NewValues.size()+i)); //1-ordered key position
        Put_B4((int32u)(8+8+Global->moov_meta_ilst_NewValues[i].size()));
        Put_B4(0x64617461); //data
        Put_B4(0x00000001); //UTF-8
        Put_B4(0x00000000); //No locale
        Put_String(Global->moov_meta_ilst_NewValues[i].size(), Global->moov_meta_ilst_NewValues[i]);
    }
    Global->moov_meta_ilst_AlreadyPresent+=Global->moov_meta_ilst_NewValues.size();
    Global->moov_meta_ilst_NewValues.clear();
    delete[] t;



    /*
    Chunk.Content.Buffer_Offset=Chunk.Content.Size;
    size_t Size_ToAdd=0;
    for (size_t i=0; i<Global->moov_meta_ilst_NewValues.size(); i++)
        Size_ToAdd+=8+8+8+Global->moov_meta_ilst_NewValues[i].size();
    int8u* t=new int8u[Chunk.Content.Size+Size_ToAdd];
    memcpy(t, Chunk.Content.Buffer, Chunk.Content.Size);
    delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer=t;
    Chunk.Content.Size+=Size_ToAdd;

    for (size_t i=0; i<Global->moov_meta_ilst_NewValues.size(); i++)
    {
        Put_B4((int32u)(8+8+8+Global->moov_meta_ilst_NewValues[i].size()));
        Put_B4((int32u)(1+Global->moov_meta_keys_AlreadyPresent-Global->moov_meta_ilst_NewValues.size()+i)); //1-ordered key position
        Put_B4((int32u)(8+8+Global->moov_meta_ilst_NewValues[i].size()));
        Put_B4(0x64617461); //data
        Put_B4(0x00000001); //UTF-8
        Put_B4(0x00000000); //No locale
        Put_String(Global->moov_meta_ilst_NewValues[i].size(), Global->moov_meta_ilst_NewValues[i]);
    }
    Global->moov_meta_ilst_AlreadyPresent+=Global->moov_meta_ilst_NewValues.size();
    Global->moov_meta_ilst_NewValues.clear();
    */

    Chunk.Content.IsModified = true;
    Chunk.Content.Size_IsModified = true;
}

//***************************************************************************
// Write
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_meta_ilst::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
