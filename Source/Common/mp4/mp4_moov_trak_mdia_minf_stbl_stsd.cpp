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
void mp4_moov_trak_mdia_minf_stbl_stsd::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_mdia_minf_stbl_stsd.count(Chunk.trak_Index))
        throw exception_read_block("2 moov trak mdia minf stbl stsd blocks");

    Global->moov_trak_mdia_minf_stbl_stsd[Chunk.trak_Index]=new global::block_moov_trak_mdia_minf_stbl_stsd();

    //Reading
    if (Chunk.Content.Size<8)
        throw exception_read_block("Can not parse moov trak mdia minf stbl stsd (too small)");
    Chunk.Content.Before_Subs_Content_Size=8;
    int64u Content_Size=Chunk.Content.Size;
    Chunk.Content.Size=Chunk.Content.Before_Subs_Content_Size; // Trick to read only atom data
    Read_Internal_ReadAllInBuffer();
    Chunk.Content.Size=Content_Size;

    Get_B1(Global->moov_trak_mdia_minf_stbl_stsd[Chunk.trak_Index]->Version);
    if (Global->moov_trak_mdia_minf_stbl_stsd[Chunk.trak_Index]->Version>0)
          throw exception_read_block("Can not parse moov trak mdia minf stbl stsd (version not supported)");

    Get_B3(Global->moov_trak_mdia_minf_stbl_stsd[Chunk.trak_Index]->Flags);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd[Chunk.trak_Index]->EntryCount);

    if (Global->moov_trak.back()->IsVideo)
    {
        SUBS_BEGIN();
            SUB_ELEMENT(moov_trak_mdia_minf_stbl_stsd_tmcd);
            SUB_ELEMENT_DEFAULT(moov_trak_mdia_minf_stbl_stsd_xxxxVideo);
        SUBS_END_DEFAULT();
    }
    else if (Global->moov_trak.back()->IsSound)
    {
        SUBS_BEGIN();
            SUB_ELEMENT_DEFAULT(moov_trak_mdia_minf_stbl_stsd_xxxxSound);
        SUBS_END_DEFAULT();
    }
    else if (Global->moov_trak.back()->IsTimeCode)
    {
        SUBS_BEGIN();
            SUB_ELEMENT(moov_trak_mdia_minf_stbl_stsd_tmcd);
        SUBS_END();
    }
    else
    {
        SUBS_BEGIN();
            SUB_ELEMENT_DEFAULT(moov_trak_mdia_minf_stbl_stsd_xxxxUnknown);
        SUBS_END_DEFAULT();
    }
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (Global->moov_trak_mdia_minf_stbl_stsd.find(Chunk.trak_Index)==Global->moov_trak_mdia_minf_stbl_stsd.end())
        return;

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset=0;
    Chunk.Content.Before_Subs_Content_Size=8;
    Chunk.Content.Size=Chunk.Content.Before_Subs_Content_Size;
    Chunk.Content.Buffer=new int8u[Chunk.Content.Before_Subs_Content_Size];

    Put_B1(Global->moov_trak_mdia_minf_stbl_stsd[Chunk.trak_Index]->Version);
    Put_B3(Global->moov_trak_mdia_minf_stbl_stsd[Chunk.trak_Index]->Flags);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd[Chunk.trak_Index]->EntryCount);

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
size_t mp4_moov_trak_mdia_minf_stbl_stsd::Insert_Internal (int32u Chunk_Name_Insert)
{
    mp4_Base* NewChunk;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_trak_mdia_minf_stbl_stsd_tmcd :  NewChunk=new mp4_moov_trak_mdia_minf_stbl_stsd_tmcd(Global); break;
        default                                           :  return Subs.size();
    }

    NewChunk->Chunk.trak_Index=Chunk.trak_Index;

    NewChunk->Modify();
    if (!NewChunk->IsRemovable())
    {
        Subs.push_back(NewChunk);
        return Subs.size()-1;
    }
    else
    {
        delete NewChunk; //NewChunk=NULL;
        return Subs.size();
    }
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}