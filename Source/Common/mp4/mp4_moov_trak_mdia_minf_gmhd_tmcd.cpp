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
void mp4_moov_trak_mdia_minf_gmhd_tmcd::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    Global->moov_trak.back()->IsTimeCode=true;

    SUBS_BEGIN();
        SUB_ELEMENT(moov_trak_mdia_minf_gmhd_tmcd_tcmi);
    SUBS_END();
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
size_t mp4_moov_trak_mdia_minf_gmhd_tmcd::Insert_Internal (int32u Chunk_Name_Insert)
{
    mp4_Base* NewChunk;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_trak_mdia_minf_gmhd_tmcd_tcmi :  NewChunk=new mp4_moov_trak_mdia_minf_gmhd_tmcd_tcmi(Global); break;
        default                                           :  return Subs.size();
    }

    NewChunk->Chunk.trak_Index=Chunk.trak_Index;

    NewChunk->Modify();
    if (!NewChunk->IsRemovable())
    {
        Subs.push_back(NewChunk); //At the end
        return Subs.size()-1;
    }
    else
    {
        delete NewChunk; //NewChunk=NULL;
        return Subs.size();
    }
}