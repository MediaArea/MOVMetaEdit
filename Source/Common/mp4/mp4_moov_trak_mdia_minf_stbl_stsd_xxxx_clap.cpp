/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include "Common/mp4/mp4_.h"
//---------------------------------------------------------------------------

#include <iostream>

//***************************************************************************
// Read
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_clap::Read_Internal()
{
    if (Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap)
        throw exception_read_block("2 moov trak mdia minf stbl stsd xxxx clap blocks");

    Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap=new global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_clap();

    Read_Internal_ReadAllInBuffer();

    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Width_Num);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Width_Den);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Height_Num);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Height_Den);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Horizontal_Offset_Num);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Horizontal_Offset_Den);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Vertical_Offset_Num);
    Get_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Vertical_Offset_Den);
}

//***************************************************************************
// Modify
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_clap::Modify_Internal()
{
    if (Chunk.Content.IsModified)
        return;

    if (!Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap)
    {
        Chunk.Content.IsRemovable = true;
        return;
    }

    if (Chunk.Content.Buffer)
        delete[] Chunk.Content.Buffer;
    Chunk.Content.Buffer_Offset = 0;
    Chunk.Content.Size = 32;
    Chunk.Content.Buffer = new int8u[Chunk.Content.Size];

    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Width_Num);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Width_Den);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Height_Num);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Height_Den);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Horizontal_Offset_Num);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Horizontal_Offset_Den);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Vertical_Offset_Num);
    Put_B4(Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Vertical_Offset_Den);

    Chunk.Content.IsModified=true;
    Chunk.Content.Size_IsModified=true;
}

//---------------------------------------------------------------------------
void mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_clap::Write_Internal() 
{
    mp4_Base::Write_Internal(Chunk.Content.Buffer, (size_t)Chunk.Content.Size);
}
