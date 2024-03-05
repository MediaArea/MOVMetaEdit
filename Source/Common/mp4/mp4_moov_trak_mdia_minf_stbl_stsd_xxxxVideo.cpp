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

    //Reading
    if (Chunk.Content.Size<78)
        throw exception_read_block("Can not parse moov trak mdia minf stbl stsd xxxxVideo (too small)");
    Chunk.Content.Before_Subs_Content_Size=78;
    int64u Content_Size=Chunk.Content.Size;
    Chunk.Content.Size=Chunk.Content.Before_Subs_Content_Size; // Trick to read only node data
    Read_Internal_ReadAllInBuffer();
    Chunk.Content.Size=Content_Size;
    Chunk.trak_Index=Global->moov_trak.size();

    uint16_t Version, Depth, ColorTableID;

    Skip_XX(6); // Reserved
    Skip_XX(2); // Data reference index
    Get_B2(Version);
    if (Version>3)
          throw exception_read_block("Can not parse moov trak mdia minf stbl stsd xxxxVideo (version not supported)");
    Skip_XX(2); // Revision level
    Skip_XX(4); // Vendor
    Skip_XX(4); // Temporal quality
    Skip_XX(4); // Spatial quality
    Skip_XX(2); // Width
    Skip_XX(2); // Height
    Skip_XX(4); // Horizontal resolution
    Skip_XX(4); // Vertical resolution
    Skip_XX(4); // Data size
    Skip_XX(2); //Frame count
    Skip_XX(32); // CompressorName
    Get_B2(Depth);
    Get_B2(ColorTableID);

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
