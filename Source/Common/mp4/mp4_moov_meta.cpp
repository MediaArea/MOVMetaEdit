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
void mp4_moov_meta::Read_Internal ()
{
    SUBS_BEGIN();
        SUB_ELEMENT(moov_meta_hdlr);
        SUB_ELEMENT(moov_meta_keys);
        SUB_ELEMENT(moov_meta_ilst);
    SUBS_END();
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
size_t mp4_moov_meta::Insert_Internal (int32u Chunk_Name_Insert)
{
    mp4_Base* NewChunk;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_meta_hdlr :  NewChunk=new mp4_moov_meta_hdlr(Global); break;
        case Elements::moov_meta_keys :  NewChunk=new mp4_moov_meta_keys(Global); break;
        case Elements::moov_meta_ilst :  NewChunk=new mp4_moov_meta_ilst(Global); break;
        default                       :  return Subs.size();
    }

    size_t Subs_Pos;
    switch (Chunk_Name_Insert)
    {
        case Elements::moov_meta_hdlr :  Subs_Pos=0                                                                     ; break;
        case Elements::moov_meta_keys :  Subs_Pos=(size_t)-1                                                            ; break;
        case Elements::moov_meta_ilst :  Subs_Pos=(size_t)-1                                                            ; break;
        default                       :  return Subs.size();
    }

    NewChunk->Modify();
    if (!NewChunk->IsRemovable())
    {
        if (Subs_Pos<Subs.size())
        {
            Subs.insert(Subs.begin()+Subs_Pos+1, NewChunk); //First place after fmt, always
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
