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
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxxVideo::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    if (Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo.count(Chunk.trak_Index))
        throw exception_read_block("2 moov trak mdia minf stbl stsd xxxxVideo blocks");

    Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]=new global::block_moov_trak_mdia_minf_stbl_stsd_xxxxVideo();

    //Reading
    if (Chunk.Content.Size<78)
        throw exception_read_block("Can not parse moov trak mdia minf stbl stsd xxxxVideo (too small)");
    Chunk.Content.Before_Subs_Content_Size=78;
    int64u Content_Size=Chunk.Content.Size;
    Chunk.Content.Size=Chunk.Content.Before_Subs_Content_Size; // Trick to read only node data
    Read_Internal_ReadAllInBuffer();
    Chunk.Content.Size=Content_Size;

    Get_B6(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->Reserved);
    Get_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->DataReferenceIndex);
    Get_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->Version);
    if (Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->Version>3)
          throw exception_read_block("Can not parse moov trak mdia minf stbl stsd xxxxVideo (version not supported)");
    Get_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->RevisionLevel);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->Vendor);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->TemporalQuality);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->SpatialQuality);
    Get_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->Width);
    Get_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->Height);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->HorizontalResolution);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->VerticalResolution);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->DataSize);
    Get_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->FrameCount);

    Get_String(32, Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->CompressorName);
    Get_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->Depth);
    Get_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->ColorTableID);

    uint16_t Depth=Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->Depth;
    uint16_t ColorTableID=Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->ColorTableID;
    bool IsGreyscale;
    if (Depth>0x20 && Depth<0x40)
    {
        Depth-=0x20;
        IsGreyscale=true;
    }
    else if (Depth==1)
        IsGreyscale=true;
    else
        IsGreyscale=false;

    if (!IsGreyscale && (Depth>1 && Depth<=8) && !ColorTableID)
        throw exception_read_block("Can not parse moov trak mdia minf stbl stsd xxxxVideo (custom color table not supported)");

    Global->moov_trak.back()->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present=true;

    SUBS_BEGIN();
        SUB_ELEMENT(moov_trak_mdia_minf_stbl_stsd_xxxx_clap);
        SUB_ELEMENT(moov_trak_mdia_minf_stbl_stsd_xxxx_colr);
        SUB_ELEMENT(moov_trak_mdia_minf_stbl_stsd_xxxx_fiel);
        SUB_ELEMENT(moov_trak_mdia_minf_stbl_stsd_xxxx_gama);
        SUB_ELEMENT(moov_trak_mdia_minf_stbl_stsd_xxxx_pasp);
        SUB_ELEMENT(moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv);
        SUB_ELEMENT(moov_trak_mdia_minf_stbl_stsd_xxxx_clli);
    SUBS_END();

    if (Subs.size() && Subs.back()->Chunk.File_In_Position+Subs.back()->Chunk.Header.Size+Subs.back()->Chunk.Content.Size<Chunk.File_In_Position+Chunk.Header.Size+Chunk.Content.Size)
        Chunk.Content.After_Subs_Content_Size=(Chunk.File_In_Position+Chunk.Header.Size+Chunk.Content.Size)-(Subs.back()->Chunk.File_In_Position+Subs.back()->Chunk.Header.Size+Subs.back()->Chunk.Content.Size);
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
size_t mp4_moov_trak_mdia_minf_stbl_stsd_xxxxVideo::Insert_Internal (int32u Chunk_Name_Insert)
{
    mp4_Base* NewChunk;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_clap :  NewChunk=new mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_clap(Global); break;
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_colr :  NewChunk=new mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_colr(Global); break;
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_fiel :  NewChunk=new mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_fiel(Global); break;
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_gama :  NewChunk=new mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_gama(Global); break;
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_pasp :  NewChunk=new mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_pasp(Global); break;
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv :  NewChunk=new mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv(Global); break;
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_clli :  NewChunk=new mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_clli(Global); break;
        default                                                :  return Subs.size();
    }

    size_t Subs_Pos;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_clap :  Subs_Pos=(size_t)-1                                             ; break;
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_colr :  Subs_Pos=(size_t)-1                                             ; break;
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_fiel :  Subs_Pos=(size_t)-1                                             ; break;
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_gama :  Subs_Pos=(size_t)-1                                             ; break;
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_pasp :  Subs_Pos=(size_t)-1                                             ; break;
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv :  Subs_Pos=(size_t)-1                                             ; break;
        case Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_clli :  Subs_Pos=(size_t)-1                                             ; break;
        default                                                :  return Subs.size();
    }

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

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxxVideo::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (!Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr.count(Chunk.trak_Index))
        return;

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset = 0;
    Chunk.Content.Buffer = new int8u[Chunk.Content.Before_Subs_Content_Size];

    Put_B6(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->Reserved);
    Put_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->DataReferenceIndex);
    Put_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->Version);
    Put_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->RevisionLevel);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->Vendor);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->TemporalQuality);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->SpatialQuality);
    Put_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->Width);
    Put_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->Height);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->HorizontalResolution);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->VerticalResolution);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->DataSize);
    Put_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->FrameCount);
    Put_String(32, Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->CompressorName);
    Put_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->Depth);
    Put_B2(Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunk.trak_Index]->ColorTableID);

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxxVideo::Write_Internal()
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}