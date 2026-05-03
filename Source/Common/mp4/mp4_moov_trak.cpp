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
void mp4_moov_trak::Read_Internal ()
{

    //Filling
    mp4_Base::global::block_moov_trak* trak=new mp4_Base::global::block_moov_trak;
    Global->moov_trak.push_back(trak);
    Chunk.trak_Index=Global->moov_trak.size()-1;

    SUBS_BEGIN();
        SUB_ELEMENT(moov_trak_tkhd);
        SUB_ELEMENT(moov_trak_tapt);
        SUB_ELEMENT(moov_trak_edts);
        SUB_ELEMENT(moov_trak_tref);
        SUB_ELEMENT(moov_trak_mdia);
    SUBS_END();
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
size_t mp4_moov_trak::Insert_Internal (int32u Chunk_Name_Insert)
{
    mp4_Base* NewChunk;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_trak_tkhd :  NewChunk=new mp4_moov_trak_tkhd(Global); break;
        case Elements::moov_trak_tapt :  NewChunk=new mp4_moov_trak_tapt(Global); break;
        case Elements::moov_trak_edts :  NewChunk=new mp4_moov_trak_edts(Global); break;
        case Elements::moov_trak_tref :  NewChunk=new mp4_moov_trak_tref(Global); break;
        case Elements::moov_trak_mdia :  NewChunk=new mp4_moov_trak_mdia(Global); break;
        default                       :  return Subs.size();
    }

    size_t Subs_Pos=(size_t)-1;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_trak_tkhd :  Subs_Pos=0; break; // First place
        case Elements::moov_trak_tapt :  Subs_Pos=(size_t)-1; break;
        case Elements::moov_trak_edts :  Subs_Pos=(size_t)-1; break;
        case Elements::moov_trak_tref :  Subs_Pos=(size_t)-1; break;
        case Elements::moov_trak_mdia :  Subs_Pos=(size_t)-1; break;
        default                       :  break;
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
