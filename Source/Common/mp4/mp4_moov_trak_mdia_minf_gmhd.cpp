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
void mp4_moov_trak_mdia_minf_gmhd::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    SUBS_BEGIN();
        SUB_ELEMENT(moov_trak_mdia_minf_gmhd_gmin);
        SUB_ELEMENT(moov_trak_mdia_minf_gmhd_text);
        SUB_ELEMENT(moov_trak_mdia_minf_gmhd_tmcd);
    SUBS_END();
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
size_t mp4_moov_trak_mdia_minf_gmhd::Insert_Internal (int32u Chunk_Name_Insert)
{
    mp4_Base* NewChunk;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_trak_mdia_minf_gmhd_gmin :  NewChunk=new mp4_moov_trak_mdia_minf_gmhd_gmin(Global); break;
        case Elements::moov_trak_mdia_minf_gmhd_text :  NewChunk=new mp4_moov_trak_mdia_minf_gmhd_text(Global); break;
        case Elements::moov_trak_mdia_minf_gmhd_tmcd :  NewChunk=new mp4_moov_trak_mdia_minf_gmhd_tmcd(Global); break;
        default                                      :  return Subs.size();
    }

    size_t Subs_Pos=(size_t)-1;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_trak_mdia_minf_gmhd_gmin :  Subs_Pos=(size_t)-1; break;
        case Elements::moov_trak_mdia_minf_gmhd_text :  Subs_Pos=(size_t)-1; break;
        case Elements::moov_trak_mdia_minf_gmhd_tmcd :  Subs_Pos=(size_t)-1; break;
        default                                      :  break;
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
