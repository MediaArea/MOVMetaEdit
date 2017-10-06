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
void mp4_mdat::Read_Internal ()
{
    //Filling
    if (!Global->mdat)
    {
        Global->mdat=new mp4_Base::global::block_mdat;
        Global->mdat->File_Offset_Begin=Global->In.Position_Get();
    }
    Global->mdat->File_Offset_End=Global->In.Position_Get()+Chunk.Content.Size;
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_mdat::Modify_Internal ()
{
    //No modification is possible
}

//***************************************************************************
// Write
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_mdat::Write_Internal ()
{
    Global->Out_Buffer_WriteAtEnd=true;
    //Global->In.GoTo(Global->mdat->File_Offset_End);
}

