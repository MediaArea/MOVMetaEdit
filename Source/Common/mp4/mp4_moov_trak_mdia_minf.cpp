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
    SUBS_BEGIN();
        SUB_ELEMENT(moov_trak_mdia_minf_stbl);
        SUB_ELEMENT(moov_trak_mdia_minf_vmhd);
        SUB_ELEMENT(moov_trak_mdia_minf_smhd);
        SUB_ELEMENT(moov_trak_mdia_minf_gmhd);
    SUBS_END();
}
