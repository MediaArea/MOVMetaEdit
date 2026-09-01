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
void mp4_moov_trak_mdia::Read_Internal ()
{
    Chunk.trak_Index=Global->moov_trak.size()-1;

    SUBS_BEGIN();
        SUB_ELEMENT(moov_trak_mdia_mdhd);
        SUB_ELEMENT(moov_trak_mdia_hdlr);
        SUB_ELEMENT(moov_trak_mdia_minf);
//        SUB_ELEMENT(moov_meta_keys);
//        SUB_ELEMENT(moov_meta_ilst);
    SUBS_END();
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
size_t mp4_moov_trak_mdia::Insert_Internal (int32u Chunk_Name_Insert)
{
    mp4_Base* NewChunk;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_trak_mdia_mdhd :  NewChunk=new mp4_moov_trak_mdia_mdhd(Global); break;
        case Elements::moov_trak_mdia_hdlr :  NewChunk=new mp4_moov_trak_mdia_hdlr(Global); break;
        case Elements::moov_trak_mdia_minf :  NewChunk=new mp4_moov_trak_mdia_minf(Global); break;
        default                            :  return Subs.size();
    }

    size_t Subs_Pos=(size_t)-1;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_trak_mdia_mdhd :  Subs_Pos=0; break; // First place
        case Elements::moov_trak_mdia_hdlr :  Subs_Pos=(size_t)-1; break;
        case Elements::moov_trak_mdia_minf :  Subs_Pos=(size_t)-1; break;
        default : break;
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
