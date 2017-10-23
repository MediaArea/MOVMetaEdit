/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include "Common/mp4_Handler.h"
#include "Common/mp4/mp4_.h"
#include <iostream>
#include <cstring>
using namespace std;
using namespace ZenLib;
//---------------------------------------------------------------------------

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
mp4_Base::mp4_Base (global* Global_In, size_t Level_In)
{
    //Global handler
    Global=Global_In;

    //Chunk
    Chunk.Header.Level=Level_In;
}

//---------------------------------------------------------------------------
mp4_Base::~mp4_Base ()
{
    for (size_t Pos=0; Pos<Subs.size(); Pos++)
        delete Subs[Pos]; //Subs[Pos]=NULL;
}

//***************************************************************************
// Read
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_Base::Read (block &Chunk_In)
{
    //Configuring block data
    Chunk.Header.Name=Chunk_In.Header.Name;
    Chunk.Header.Size= Chunk_In.Header.Size;
    Chunk.Content.Size=Chunk_In.Content.Size;
    Chunk.File_In_Position=Global->In.Position_Get()-Chunk.Header.Size;

    //Trace
    if (Chunk.Header.Level)
    {
        //Offset
        if (Global->In.Position_Get()-Chunk.Header.Size<0x10000000)
            Global->Trace<<'0';
        if (Global->In.Position_Get()-Chunk.Header.Size<0x1000000)
            Global->Trace<<'0';
        if (Global->In.Position_Get()-Chunk.Header.Size<0x100000)
            Global->Trace<<'0';
        if (Global->In.Position_Get()-Chunk.Header.Size<0x10000)
            Global->Trace<<'0';
        if (Global->In.Position_Get()-Chunk.Header.Size<0x1000)
            Global->Trace<<'0';
        if (Global->In.Position_Get()-Chunk.Header.Size<0x100)
            Global->Trace<<'0';
        if (Global->In.Position_Get()-Chunk.Header.Size<0x10)
            Global->Trace<<'0';
        Global->Trace<<Ztring::ToZtring(Global->In.Position_Get()-Chunk.Header.Size, 16).To_Local()<<' ';

        //Size
        int64u Size=Chunk.Content.Size+Chunk.Header.Size-8;
        if (Size<0x10000000)
            Global->Trace<<'0';
        if (Size<0x1000000)
            Global->Trace<<'0';
        if (Size<0x100000)
            Global->Trace<<'0';
        if (Size<0x10000)
            Global->Trace<<'0';
        if (Size<0x1000)
            Global->Trace<<'0';
        if (Size<0x100)
            Global->Trace<<'0';
        if (Size<0x10)
            Global->Trace<<'0';
        Global->Trace<<Ztring::ToZtring(Size, 16).To_Local()<<' ';

        //Chunk name
        for (size_t Pos=1; Pos<Chunk.Header.Level; Pos++)
            Global->Trace<<"     ";
        Global->Trace<<Ztring().From_CC4(Chunk.Header.Name).To_Local();
        
        //Size
        /*
        for (size_t Pos=Chunk.Header.Level; Pos<4; Pos++)
            Global->Trace<<"     ";
        Global->Trace<<"("<<Ztring::ToZtring(Chunk.Header.Size)<<"+"<<Ztring::ToZtring(Chunk.Content.Size);
        if (Chunk.Content.Size%2)
            Global->Trace<<"+1";
        Global->Trace<<" bytes)";
        */
        
        Global->Trace<<endl;
    }

    //Specific to a block
    Read_Internal();

    if (Chunk.Content.IsModified)
        Chunk_In.Content.IsModified=true;
    if (Chunk.Content.Size_IsModified)
        Chunk_In.Content.Size_IsModified=true;

    //Positioning
    Global->In.GoTo(Chunk.File_In_Position+Chunk.Header.Size+Chunk.Content.Size);

    if (Global->In.Position_Get()>=Global->In.Size_Get())
    {
        //We want at least 1 mdat atom
        if (!Global->mdat)
            throw exception_valid("no mdat atom");
        //We want at least 1 moov atom
        if (Global->moov.empty())
            throw exception_valid("no moov atom");
        //We want at least 1 moov atom
        if (Global->moov.size()>1)
            throw exception_valid("too many moov atoms");
        //moov must be after mdat
        if (Global->moov[0]->File_Offset < Global->mdat->File_Offset_Begin)
            throw exception_read_block("Faststart not yet supported");
        if (Global->moov[0]->File_Offset > Global->mdat->File_Offset_Begin && Global->moov[0]->File_Offset < Global->mdat->File_Offset_End)
            throw exception_read_block("moov atom between 2 mdat atoms not yet supported");
        //UniversalAdId fields already present not supported
        /*if (Global->moov_meta_keys)
            for (size_t i=0; i<Global->moov_meta_keys->Keys.size(); i++)
            {
                global::block_moov_meta_key &key = Global->moov_meta_keys->Keys[i];
                if (key.namespace_==0x6D647461)
                {
                    string value((char*)key.value, key.size-8);
                    if (value=="com.universaladid.idregistry"
                     || value=="com.universaladid.idvalue")
                        throw exception_read_block("UniversalAdId fields already present");
                }
            }*/
    }
}

//---------------------------------------------------------------------------
bool mp4_Base::Read_Header (block &NewChunk)
{
    int8u Temp[8];

    //Atom size
    if (Global->In.Position_Get() + 4>Chunk.File_In_Position + Chunk.Header.Size + Chunk.Content.Size)
        throw exception_valid("small");
    if (Global->In.Read(Temp, 4)<4)
        throw exception_read();
    NewChunk.Content.Size = BigEndian2int32u(Temp);
    if (NewChunk.Content.Size<8 && NewChunk.Content.Size != 1)
    {
        throw exception_valid("invalid atom size");
    }
    if (Global->In.Position_Get() - 4 + NewChunk.Content.Size>Chunk.File_In_Position + Chunk.Header.Size + Chunk.Content.Size)
        throw exception_valid("truncated");

    //Atom name
    if (Global->In.Position_Get()>Chunk.File_In_Position + Chunk.Header.Size + Chunk.Content.Size)
        throw exception_valid("small");
    if (Global->In.Read(Temp, 4)<4)
        throw exception_read();
    NewChunk.Header.Name = CC4(Temp);

    if (NewChunk.Content.Size == 1)
    {
        //64-bit size
        if (Global->In.Position_Get() + 8>Chunk.File_In_Position + Chunk.Header.Size + Chunk.Content.Size)
            throw exception_valid("small");
        if (Global->In.Read(Temp, 8)<8)
            throw exception_read();
        NewChunk.Content.Size = BigEndian2int64u(Temp);
        NewChunk.Header.Size = 16;
    }
    else
        NewChunk.Header.Size = 8;

    NewChunk.Content.Size -= NewChunk.Header.Size;

    return true;
}

//---------------------------------------------------------------------------
void mp4_Base::Read_Internal_ReadAllInBuffer ()
{
    //Creating buffer
    if (Chunk.Content.Size==0) //Chunk.Content.Size may be set to something else for customizing
        Chunk.Content.Size=Chunk.Content.Size;
    if (Chunk.Content.Size>(size_t)-1)
        throw exception_read_block("non-audio data exceeds available memory");

    delete Chunk.Content.Buffer; Chunk.Content.Buffer=NULL;

    try
    {
        Chunk.Content.Buffer=new int8u[(size_t)Chunk.Content.Size];
    }
    catch(...)
    {
        throw exception_read_block("non-audio data exceeds available memory");
    }

    //Reading
    while(Chunk.Content.Buffer_Offset<Chunk.Content.Size)
    {
        size_t BytesRead=Global->In.Read(Chunk.Content.Buffer+Chunk.Content.Buffer_Offset, (size_t)Chunk.Content.Size-Chunk.Content.Buffer_Offset);
        if (BytesRead==0)
            break; //Read is finished
        Global->CS.Enter();
        Global->Progress=(float)Global->In.Position_Get()/Global->In.Size_Get();
        if (Global->Canceling)
        {
            Global->CS.Leave();
            throw exception_canceled();
        }
        Global->CS.Leave();
        //SleeperThread::msleep(20);
        Chunk.Content.Buffer_Offset+=BytesRead;
    }
    if (Chunk.Content.Buffer_Offset<Chunk.Content.Size)
        throw exception_read();
    Chunk.Content.Buffer_Offset=0;
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_Base::Modify (int32u Chunk_Name_1, int32u Chunk_Name_2, int32u Chunk_Name_3)
{
    if (Chunk_Name_1==0x00000000)
        Modify_Internal();
    else
        Modify_Internal_Subs(Chunk_Name_1, Chunk_Name_2, Chunk_Name_3);
}

//---------------------------------------------------------------------------
void mp4_Base::Modify_Internal_Subs (int32u Chunk_Name_1, int32u Chunk_Name_2, int32u Chunk_Name_3)
{
    //Parsing subs
    bool Sub_IsFound=false;
    size_t Sub_Pos=0;
    for (; Sub_Pos<Subs.size(); Sub_Pos++)
        if (Subs[Sub_Pos]->Chunk.Header.Name==Chunk_Name_1)
            break;
    
    //Handling when sub is not present
    if (Sub_Pos>=Subs.size())
    {
        Sub_Pos=Insert_Internal(Chunk_Name_1);
        if (Sub_Pos>=Subs.size())
            return; //Inserting is not supported
        Chunk.Content.IsModified=true;
        Chunk.Content.Size_IsModified=true;
    }

    //Modifying
    Subs[Sub_Pos]->Modify(Chunk_Name_2, Chunk_Name_3, 0x00000000);
    if (Subs[Sub_Pos]->Chunk.Content.IsModified)
        Chunk.Content.IsModified=true;
    if (Subs[Sub_Pos]->Chunk.Content.Size_IsModified)
        Chunk.Content.Size_IsModified=true;
    if (Subs[Sub_Pos]->Chunk.Content.IsRemovable)
    {
        Chunk.Content.IsModified=true;
        Chunk.Content.Size_IsModified=true;
        Subs.erase(Subs.begin()+Sub_Pos);
        Sub_Pos--;
        if (Subs.empty())
            Chunk.Content.IsRemovable=true;
    }
}

//***************************************************************************
// Write
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_Base::Write ()
{
    //Header
    if (Chunk.Header.Level)
    {
        //Calculating block size
        int64u Block_Size=Block_Size_Get();
        if (Block_Size>0xFFFFFFFF || Chunk.Header.Size == 16)
        {
            int8u Header[16];
            int32u2BigEndian(Header, 1);
            int32u2BigEndian(Header + 4, Chunk.Header.Name);
            int64u2BigEndian(Header + 8, Block_Size);
            Write_Internal(Header, 16);
        }
        else
        {
            int8u Header[8];
            int32u2BigEndian(Header, (int32u)Block_Size);
            int32u2BigEndian(Header+4, Chunk.Header.Name);
            Write_Internal(Header, 8);
        }
    }
    else if (!IsModified())
        return; //Nothing to do if the file is not modifed (Level 0)

    //Testing if block order is valid from user preferences
    if (Chunk.Header.Level==0 && Global->NewChunksAtTheEnd)
    {
        bool DataChunkMustBeMoved;
        do
        {
            //Calculating the size of block up to data block
            int64u File_Begin_Offset_Theory=Subs[0]->Chunk.Header.Size;
            size_t Pos=0;
            for (; Pos<Subs[0]->Subs.size(); Pos++)
            {
                if (Subs[0]->Subs[Pos]->Chunk.Header.Name==Elements::mdat)
                    break;
                File_Begin_Offset_Theory+=Subs[0]->Subs[Pos]->Block_Size_Get();
            }
            if (Pos==Subs[0]->Subs.size())
                throw exception_write("Should never happen, please contact the developper (mdat block not found)");

            /*
            if (File_Begin_Offset_Theory+8>Global->mdat->File_Offset-8 && File_Begin_Offset_Theory!=Global->mdat->File_Offset-8) //if data block must be moved
            {
                //Moving a block at the end
                while (Pos>0)
                {
                    Pos--;
                    switch (Subs[0]->Subs[Pos]->Chunk.Header.Name)
                    {
                        case Elements::moov :
                                                    {
                                                    mp4_Base* Temp=Subs[0]->Subs[Pos];
                                                    Subs[0]->Subs.erase(Subs[0]->Subs.begin()+Pos);
                                                    Subs[0]->Subs.push_back(Temp);
                                                    DataChunkMustBeMoved=true;
                                                    break;
                                                    }
                        default                  :  ;
                                                    DataChunkMustBeMoved=false;
                    }
                    if (DataChunkMustBeMoved)
                        break;
                }
            }
            else
            */
                DataChunkMustBeMoved=false;
        }
        while (DataChunkMustBeMoved);
    }
    
    //Content
    if (Chunk.Content.IsModified)
    {
        if (!Subs.empty())
        {
            for (size_t Pos = 0; Pos < Subs.size(); Pos++)
            {
                bool Out_Buffer_WriteAtEnd_Old = Global->Out_Buffer_WriteAtEnd;
                Subs[Pos]->Write();

                // Going after data part
                if (!Out_Buffer_WriteAtEnd_Old && Global->Out_Buffer_WriteAtEnd)
                {
                    while (Pos < Subs.size() && Subs[Pos]->Chunk.File_In_Position+Subs[Pos]->Chunk.Header.Size+Subs[Pos]->Chunk.Content.Size < Global->mdat->File_Offset_End)
                        Pos++;
                }
            }
        }
        else
        {
            Write_Internal();
        }
    }
    else
    {
        Write_Internal();
    }

    if (Chunk.Header.Level==0 && Global->Out_Buffer_File_TryModification)
    {
        //Testing if all is OK.
        if (Global->mdat)
        {
            //Test if we can write over the file
            Global->Out_Buffer_File_IsModified=true;
            //if (Global->Out_Buffer_End.Data && Global->mdat->File_Offset+Global->mdat->Size+Global->Out_Buffer_End.Size<Global->In.Size_Get())
            //    Global->Out_Buffer_File_IsModified=false;
            if (Global->Out_Buffer_Begin.Size!=Global->mdat->File_Offset_Begin)
            {
                throw exception_write("(Not yet implemented)"); 
                Global->Out_Buffer_File_IsModified=false;
            }
        }

        if (Global->Out_Buffer_File_IsModified)
        {
            //Real writing
            Global->In.Close();
            if (!Global->Out.Open(Global->File_Name, File::Access_Read_Write))
                throw exception_write("Can not open input file in read/write mode");

            //Begin
            if (Global->Out_Buffer_Begin.Data)
            {
                if (Global->Out.Write(Global->Out_Buffer_Begin.Data, Global->Out_Buffer_Begin.Size)!=Global->Out_Buffer_Begin.Size)
                    throw exception_write("Can not write input file, file may be CORRUPTED");
            }

            //Middle
            if (!Global->Out.GoTo(Global->mdat->File_Offset_End))
                throw exception_write("Can not seek input file, file may be CORRUPTED");

            //End
            if (Global->Out_Buffer_End.Data)
            {
                if (Global->Out.Write(Global->Out_Buffer_End.Data, Global->Out_Buffer_End.Size)!=Global->Out_Buffer_End.Size)
                    throw exception_write("Can not write the file, file may be CORRUPTED");
            }

            //Padding
            int64u Remaining=Global->Out.Size_Get()-Global->Out.Position_Get();
            if (Remaining)
            {
                if (!Global->Out.Truncate()) //Trying to truncate, and fallback to add a free atom if it is not possible
                {
                    if (Remaining<8)
                        Remaining=8;
                    if (Remaining>0xFFFFFFFF)
                        throw exception_write("Content to put with free atom is too big, file may be CORRUPTED");
                    int8u* Header=new int8u[Remaining];
                    int32u2BigEndian(Header, Remaining);
                    int32u2BigEndian(Header + 4, Elements::free);
                    memset(Header + 8, 0x00, Remaining-8);
                    if (Global->Out.Write(Header, Remaining)!=Remaining)
                        throw exception_write("Can not write the file, file may be CORRUPTED");
                }
            }

            //Cleanup
            Global->Out.Close();
        }
        else
        {
            throw exception_write("Not yet implemented");

            //Real writing
            if (!Global->Out.Create(Global->File_Name+__T(".tmp"), false))
                throw exception_write("Can not create temporary file");

            //Begin
            if (Global->Out_Buffer_Begin.Data)
            {
                if (Global->Out.Write(Global->Out_Buffer_Begin.Data, Global->Out_Buffer_Begin.Size)!=Global->Out_Buffer_Begin.Size)
                    throw exception_write("Can not write temporary file");
            }

            //Middle
            bool Out_Buffer_File_TryModification_Temp=Global->Out_Buffer_File_TryModification;
            int64u Chunk_Content_Size_Temp=Chunk.Content.Size;
            Global->Out_Buffer_File_TryModification=false;
            Chunk.File_In_Position=Global->mdat->File_Offset_Begin-Chunk.Header.Size;
            if (!Global->In.GoTo(Chunk.File_In_Position))
                throw exception_write("Can not seek input file");
            Chunk.Content.Size=Global->mdat->File_Offset_End-Global->mdat->File_Offset_Begin-Chunk.Header.Size;
            Write_Internal(); //We use the already made method, with some configuring before and after
            Global->Out_Buffer_File_TryModification=Out_Buffer_File_TryModification_Temp;
            Chunk.Content.Size=Chunk_Content_Size_Temp;

            //End
            if (Global->Out_Buffer_End.Data)
            {
                if (Global->Out.Write(Global->Out_Buffer_End.Data, Global->Out_Buffer_End.Size)!=Global->Out_Buffer_End.Size)
                    throw exception_write("Can not write temporary file");
            }

            //Cleanup
            Global->In.Close();
            Global->Out.Close();

            //Renaming files
            if (!File::Delete(Global->File_Name))
                throw exception_write("Original file can't be deleted");
            if (!File::Move(Global->File_Name+__T(".tmp"), Global->File_Name))
                throw exception_write("Temporary file can't be renamed");
        }
    }
}

//---------------------------------------------------------------------------
void mp4_Base::Read_Internal ()
{
    Read_Internal_ReadAllInBuffer();
}

//---------------------------------------------------------------------------
void mp4_Base::Write_Internal ()
{
    Global->In.GoTo(Chunk.File_In_Position+Chunk.Header.Size);
    
    int8u Temp[32768];
    while(Global->In.Position_Get()<Chunk.File_In_Position+Chunk.Header.Size+Chunk.Content.Size)
    {
        //Reading
        size_t Temp_Offset=0;
        while(Temp_Offset<32768)
        {
            size_t BytesRead=Global->In.Read(Temp+Temp_Offset, (Chunk.File_In_Position+Chunk.Header.Size+Chunk.Content.Size-Global->In.Position_Get()>32768)?32768:(size_t)(Chunk.File_In_Position+Chunk.Header.Size+Chunk.Content.Size-Global->In.Position_Get()));
            if (BytesRead==0)
                break; //Read is finished
            Global->CS.Enter();
            Global->Progress=(float)Global->In.Position_Get()/Global->In.Size_Get();
            if (Global->Canceling)
            {
                Global->CS.Leave();
                throw exception_canceled();
            }
            Global->CS.Leave();
            //SleeperThread::msleep(20);
            Temp_Offset+=BytesRead;
        }

        Write_Internal(Temp, Temp_Offset);
    }
}

//---------------------------------------------------------------------------
void mp4_Base::Write_Internal (const int8u* Temp, size_t Temp_Offset)
{
        //Writing
        if (Global->Out_Buffer_File_TryModification)
        {
            if (!Global->Out_Buffer_WriteAtEnd)
            {
                if (Global->Out_Buffer_Begin.Data==NULL)
                {
                    if (Global->mdat==NULL)
                        throw exception_write(); //TODO: change it
                    Global->Out_Buffer_Begin.Size_Maximum=(size_t)Global->mdat->File_Offset_Begin;
                    Global->Out_Buffer_Begin.Data=new int8u[Global->Out_Buffer_Begin.Size_Maximum];
                }

                if (Global->Out_Buffer_Begin.Size+Temp_Offset>Global->Out_Buffer_Begin.Size_Maximum)
                {
                    //Increasing the buffer size
                    Global->Out_Buffer_Begin.Size_Maximum+=Temp_Offset;
                    Global->Out_Buffer_Begin.Size_Maximum*=2;
                    int8u* Buffer_New=new int8u[Global->Out_Buffer_Begin.Size_Maximum];
                    memcpy(Buffer_New, Global->Out_Buffer_Begin.Data, Global->Out_Buffer_Begin.Size);
                    delete[] Global->Out_Buffer_Begin.Data; Global->Out_Buffer_Begin.Data=Buffer_New;
                }

                memcpy(Global->Out_Buffer_Begin.Data+Global->Out_Buffer_Begin.Size, Temp, Temp_Offset);
                Global->Out_Buffer_Begin.Size+=Temp_Offset;
            }
            else
            {
                if (Global->Out_Buffer_End.Data==NULL)
                {
                    if (Global->mdat==NULL)
                        throw exception_write(); //TODO: change it
                    Global->Out_Buffer_End.Size_Maximum=(size_t)(Global->In.Size_Get()-(Global->mdat->File_Offset_End));
                    Global->Out_Buffer_End.Data=new int8u[Global->Out_Buffer_End.Size_Maximum];
                }

                if (Global->Out_Buffer_End.Size+Temp_Offset>Global->Out_Buffer_End.Size_Maximum)
                {
                    //Increasing the buffer size
                    Global->Out_Buffer_End.Size_Maximum+=Temp_Offset;
                    Global->Out_Buffer_End.Size_Maximum*=2;
                    int8u* Buffer_New=new int8u[Global->Out_Buffer_End.Size_Maximum];
                    memcpy(Buffer_New, Global->Out_Buffer_End.Data, Global->Out_Buffer_End.Size);
                    delete[] Global->Out_Buffer_End.Data; Global->Out_Buffer_End.Data=Buffer_New;
                }

                memcpy(Global->Out_Buffer_End.Data+Global->Out_Buffer_End.Size, Temp, Temp_Offset);
                Global->Out_Buffer_End.Size+=Temp_Offset;
            }
        }
        else
        {
            if (!Global->Out.Opened_Get())
            {
                throw exception_write("Not yet implemented");

                if (!Global->Out.Create(Global->File_Name+__T(".tmp")))
                    throw exception_write(Global->File_Name.To_UTF8()+": temporary file can not be created");
            }
            if (Global->Out.Write(Temp, Temp_Offset)<Temp_Offset)
                throw exception_write();
        }
}

//---------------------------------------------------------------------------
void mp4_Base::Write_Internal_Subs ()
{
    //Parsing subs
    for (size_t Pos=0; Pos<Subs.size(); Pos++)
        Subs[Pos]->Write();
}

//***************************************************************************
// Size
//***************************************************************************

//---------------------------------------------------------------------------
int64u mp4_Base::Block_Size_Get ()
{
    if (!Chunk.Content.Size_IsModified || Subs.empty())
    {
        if (!Chunk.Header.Size)
            Chunk.Header.Size = ((Chunk.Header.Size + Chunk.Content.Size) <= 0xFFFFFFFF ? 8 : 16);
        return Chunk.Header.Size + Chunk.Content.Size;
    }
    
    //Parsing subs
    int64u Size=0;
    for (size_t Pos=0; Pos<Subs.size(); Pos++)
    {
        /*
        if (Pos+1<Subs.size() && Subs[Pos]->Chunk.Header.Name==Elements::free && Subs[Pos+1]->Chunk.Header.Name==Elements::mdat)
            Subs.erase(Subs.begin()+Pos);
        */
        if (Pos>0 && Subs[Pos]->Chunk.Header.Name==Elements::mdat && Subs[Pos-1]->Chunk.Header.Name!=Elements::free)
        {
            //Padding if we can
            //if (12+Size+8+8<=Global->mdat->File_Offset || Size>Global->mdat->File_Offset)
            {
                /*
                if (Subs[Pos]->Chunk.Header.Name!=Elements::free)
                    Subs.insert(Subs.begin()+Pos, new mp4_free(Global));
                Subs[Pos]->Chunk.Header.Level=Subs[Pos+1]->Chunk.Header.Level;
                Subs[Pos]->Chunk.Header.Name=Elements::free;
                if (12+Size+8+8<=Global->data->File_Offset)
                    Subs[Pos]->Chunk.Content.Size=Global->data->File_Offset-(12+Size+8+8); //Header + Size + free header + data header
                if (Size>Global->data->File_Offset)
                    Subs[Pos]->Chunk.Content.Size=mp4_free_DefaultSise; //Additional padding of mp4_free_DefaultSise.
                Subs[Pos]->Chunk.Content.Buffer=new int8u[(size_t)Subs[Pos]->Chunk.Content.Size];
                memset(Subs[Pos]->Chunk.Content.Buffer, 0x00, (size_t)Subs[Pos]->Chunk.Content.Size);
                Subs[Pos]->Chunk.Content.IsModified=true;
                Subs[Pos]->Chunk.Content.Size_IsModified=true;
                */
            }
            //else if (Subs[Pos]->Chunk.Header.Name==Elements::free)
            {
                /*
                Subs.erase(Subs.begin()+Pos);
                if (Pos>=Subs.size())
                    break;
                */
            }
        }
        Size+=Subs[Pos]->Block_Size_Get();
    }

    if (!Chunk.Header.Size)
        Chunk.Header.Size = ((Chunk.Header.Size + Chunk.Content.Size) <= 0xFFFFFFFF ? 8 : 16);
    return Chunk.Header.Size+Size;
}

//---------------------------------------------------------------------------
int64u mp4_Base::Block_Size_Get (int32u Element)
{
    size_t Pos=Subs_Pos_Get(Element);
    if (Pos==(size_t)-1)
        return 0;

    return Subs[Pos]->Block_Size_Get();
}

//---------------------------------------------------------------------------
size_t mp4_Base::Subs_Pos_Get (int32u Element)
{
    size_t Subs_Pos=0;
    for (; Subs_Pos<Subs.size(); Subs_Pos++)
        if (Subs[Subs_Pos]->Chunk.Header.Name==Element)
            break;
    if (Subs_Pos<Subs.size())
        return Subs_Pos;
    else
        return (size_t)-1;
}
