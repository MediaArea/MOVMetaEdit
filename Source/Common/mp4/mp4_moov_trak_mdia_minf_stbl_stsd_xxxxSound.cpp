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
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxxSound::Read_Internal ()
{

    //Reading
    if (Chunk.Content.Size<16)
        throw exception_read_block("Can not parse moov trak mdia minf stbl stsd xxxxAudio (too small)");
    Chunk.Content.Before_Subs_Content_Size=16;
    int64u Content_Size=Chunk.Content.Size;
    Chunk.Content.Size=Chunk.Content.Before_Subs_Content_Size; // Trick to read only node data
    Read_Internal_ReadAllInBuffer();
    Chunk.Content.Size=Content_Size;
    Chunk.trak_Index=Global->moov_trak.size();

    Skip_XX(6); // Reserved;
    Skip_XX(2); // Data reference index
    uint16_t Version;
    Get_B2(Version);
    if (Version>2)
          throw exception_read_block("Can not parse moov trak mdia minf stbl stsd xxxxSound (version not supported)");

    Skip_XX(2); // Revision level
    Skip_XX(4); // Vendor;

    size_t Extra_Size;
    if (Version==0)
        Extra_Size=12;
    if (Version==1)
        Extra_Size=28;
    else //Version==2
        Extra_Size=48;

    int8u* Temp;
    Chunk.Content.Before_Subs_Content_Size+=Extra_Size;
    try
    {
        Temp=new int8u[Chunk.Content.Before_Subs_Content_Size];
    }
    catch(...)
    {
        throw exception_read_block("non-audio data exceeds available memory");
    }

    std::copy(Chunk.Content.Buffer, Chunk.Content.Buffer+Chunk.Content.Before_Subs_Content_Size-Extra_Size, Temp);

    while(Chunk.Content.Buffer_Offset<Chunk.Content.Before_Subs_Content_Size)
    {
        size_t BytesRead=Global->In.Read(Temp+Chunk.Content.Buffer_Offset, Chunk.Content.Before_Subs_Content_Size-Chunk.Content.Buffer_Offset);
        if (BytesRead==0)
            break; //Read is finished
        Chunk.Content.Buffer_Offset+=BytesRead;
    }
    if (Chunk.Content.Buffer_Offset<Chunk.Content.Before_Subs_Content_Size)
        throw exception_read();

    delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer=Temp;

    Chunk.Content.Buffer_Offset-=Extra_Size;

    if (Version<2)
    {
        Skip_XX(2); // Number of channels
        Skip_XX(2); // Sample size
        Skip_XX(2); // Compression ID
        Skip_XX(2); // Packet size
        Skip_XX(2); // Sample rate
        Skip_XX(2); // Reserved

        if (Version==1)
        {
            Skip_XX(4); // Samples per packet
            Skip_XX(4); // Bytes per packet
            Skip_XX(4); // Bytes per frame
            Skip_XX(4); // Bytes per sample
        }
   }
   else// Version==2
   {
        Skip_XX(2); // Reserved
        Skip_XX(2); // Reserved
        Skip_XX(2); // Reserved
        Skip_XX(2); // Reserved
        Skip_XX(4); // Reserved
        Skip_XX(4); // Size of Struct
        Skip_XX(8); // Sample rate
        Skip_XX(4); // Number of channels
        Skip_XX(4); // Reserved
        Skip_XX(4); // SampleSize
        Skip_XX(4); // Flags
        Skip_XX(4); // Bytes per packet
        Skip_XX(4); // Frames per packet
   }

    Global->moov_trak.back()->moov_trak_mdia_minf_stbl_stsd_xxxxSound_Present=true;

    SUBS_BEGIN();
        SUB_ELEMENT(moov_trak_mdia_minf_stbl_stsd_xxxx_chan);
    SUBS_END();

    if (!Subs.empty() && Subs.back()->Chunk.File_In_Position+Subs.back()->Chunk.Header.Size+Subs.back()->Chunk.Content.Size<Chunk.File_In_Position+Chunk.Header.Size+Chunk.Content.Size)
        Chunk.Content.After_Subs_Content_Size=(Chunk.File_In_Position+Chunk.Header.Size+Chunk.Content.Size)-(Subs.back()->Chunk.File_In_Position+Subs.back()->Chunk.Header.Size+Subs.back()->Chunk.Content.Size);
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
size_t mp4_moov_trak_mdia_minf_stbl_stsd_xxxxSound::Insert_Internal (int32u Chunk_Name_Insert)
{
    mp4_Base* NewChunk;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_chan :  NewChunk=new mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_chan(Global); break;
        default                                                :  return Subs.size();
    }

    size_t Subs_Pos;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_chan :  Subs_Pos=(size_t)-1                                             ; break;
        default                                                :  return Subs.size();
    }

    NewChunk->Chunk.trak_Index=Chunk.trak_Index;
    NewChunk->Modify();

    if (!NewChunk->IsRemovable())
    {
        if (Subs_Pos<Subs.size())
        {
            Subs.insert(Subs.begin()+Subs_Pos+1, NewChunk); //First place
            return Subs_Pos+1;
        }
        else
        {
            Subs.push_back(NewChunk); //At the end
            return Subs.size()-1;
        }
    }
    else
    {
        delete NewChunk; //NewChunk=NULL;
        return Subs.size();
    }
}
