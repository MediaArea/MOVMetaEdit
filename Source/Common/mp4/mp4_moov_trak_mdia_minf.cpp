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
void mp4_moov_trak_mdia_minf::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    SUBS_BEGIN();
        SUB_ELEMENT(moov_trak_mdia_minf_dinf);
        SUB_ELEMENT(moov_trak_mdia_minf_stbl);
        SUB_ELEMENT(moov_trak_mdia_minf_hdlr);
        SUB_ELEMENT(moov_trak_mdia_minf_vmhd);
        SUB_ELEMENT(moov_trak_mdia_minf_smhd);
        SUB_ELEMENT(moov_trak_mdia_minf_gmhd);
    SUBS_END();
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
size_t mp4_moov_trak_mdia_minf::Insert_Internal (int32u Chunk_Name_Insert)
{
    mp4_Base* NewChunk;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_trak_mdia_minf_gmhd :  NewChunk=new mp4_moov_trak_mdia_minf_gmhd(Global); break;
        case Elements::moov_trak_mdia_minf_hdlr :  NewChunk=new mp4_moov_trak_mdia_minf_hdlr(Global); break;
        case Elements::moov_trak_mdia_minf_dinf :  NewChunk=new mp4_moov_trak_mdia_minf_dinf(Global); break;
        case Elements::moov_trak_mdia_minf_stbl :  NewChunk=new mp4_moov_trak_mdia_minf_stbl(Global); break;
        default                                 :  return Subs.size();
    }

    size_t Subs_Pos=(size_t)-1;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_trak_mdia_minf_gmhd :  Subs_Pos=0; break; // First place
        case Elements::moov_trak_mdia_minf_hdlr :  Subs_Pos=(size_t)-1; break;
        case Elements::moov_trak_mdia_minf_dinf :  Subs_Pos=(size_t)-1; break;
        case Elements::moov_trak_mdia_minf_stbl :  Subs_Pos=(size_t)-1; break;
        default                                 :  break;
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
