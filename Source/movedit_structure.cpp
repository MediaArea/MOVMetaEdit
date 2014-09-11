/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include "movedit_structure.h"

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <tchar.h>
#include "ZenLib/Dir.h"
#include "ZenLib/File.h"
using namespace std;
using namespace ZenLib;

Structure::Structure(File* F_, const Ztring &Name_)
    :
    IsOk(true),
    mdatIsPresent(false),
    moovIsPresent(false),
    VideoTrackCount(0),
    Width(0),
    Height(0),
    WidthScale(0),
    WidthScalePos((uint64_t)-1),
    pasp_h(0),
    pasp_v(0),
    paspOffset((uint64_t)-1),
    Name(Name_),
    F(F_)
{
    Root_ = new atom(F, NULL);
}

Structure::~Structure()
{
    delete Root_;
}

void Structure::Parse()
{
    Root_->TotalSize = F->Size_Get();

    for (;;)
    {
        atom* Item = Root_->Next();
        if (Item == NULL)
            break;
        if (!Item->IsOk)
        {
            IsOk = false;
            break;
        }

        if (Root_->Subs.size() <= 1)
            switch (Item->Code)
            {
                case Element::ftyp:
                case Element::mdat:
                case Element::moov:
                case Element::wide:
                    break;
                default:
                    cout << " Can not detect MOV header " << endl;
                    IsOk = false;
                    return;
            }

        switch (Item->Code)
        {
            case Element::free: free(Item); break;
            case Element::mdat: mdat(Item); break;
            case Element::moov: moov(Item); break;
            default:;
        }
    }

    if (!moovIsPresent)
    {
        cout << " Can not find the moov atom" << endl;
        IsOk = false; //At least 1 moov
    }
}

void Structure::free(atom* Up)
{
    if (Up->StreamOffset == moovOffsetMax && Up->TotalSize >= 16)
    {
        freeIsPresent = true;
        freeOffset = Up->StreamOffset;
        freeTotalSize = Up->TotalSize;
    }
}

void Structure::mdat(atom* Up)
{
    mdatIsPresent = true;
    mdatOffset = Up->StreamOffset;
}

void Structure::moov(atom* Up)
{
    if (moovIsPresent)
    {
        cout << " 2 moov atoms, not supported" << endl;
        IsOk = false; // 2 moov rejected
    }
    else
        moovIsPresent = true;
    moovOffsetMax = Up->StreamOffset + Up->TotalSize;

    for (;;)
    {
        atom* Item = Up->Next();
        if (Item == NULL)
            break;
        if (!Item->IsOk)
        {
            IsOk = false;
            break;
        }

        switch (Item->Code)
        {
            case Element::trak: moov_trak(Item); break;
            default:;
        }
    }
}

void Structure::moov_trak(atom* Up)
{
    for (;;)
    {
        atom* Item = Up->Next();
        if (Item == NULL)
            break;
        if (!Item->IsOk)
        {
            IsOk = false;
            break;
        }

        switch (Item->Code)
        {
            case Element::tkhd: moov_trak_tkhd(Item); break;
            case Element::mdia: moov_trak_mdia(Item); break;
            default:;
        }
    }
}

void Structure::moov_trak_tkhd(atom* Up)
{
    TrackIsVideo = false;

    if (!Up->DataLoad())
    {
        IsOk = false;
        return;
    }

    if (Up->DataSize < 1)
    {
        cout << " Can not parse moov_tkhd (no version)" << endl;
        return;
    }
    uint8_t Version = BigEndian2int8u(Up->DataContent);
    if (Version>1)
    {
        cout << " Can not parse moov_tkhd (version not supported)" << endl;
        return;
    }
    if ((Version == 0 && Up->DataSize < 0x54)
        || (Version == 1 && Up->DataSize < 0x5C))
    {
        cout << " Can not parse moov_tkhd (too small)" << endl;
        return;
    }
    size_t ExtraBytes = Version == 0 ? 0 : 8;

    uint16_t Width_Temp = BigEndian2int16u(Up->DataContent + ExtraBytes + 0x4C);
    uint16_t Height_Temp = BigEndian2int16u(Up->DataContent + ExtraBytes + 0x50);
    if (Width_Temp && Height_Temp > 20) // Test if it is video
    {
        VideoTrackCount++;
        if (VideoTrackCount == 1)
        {
            double WidthScale_BeforeComma = (double)BigEndian2int16u(Up->DataContent + ExtraBytes + 0x28);
            double WidthScale_AfterComma = (double)BigEndian2int16u(Up->DataContent + ExtraBytes + 0x2A);
            Width = Width_Temp;
            Height = Height_Temp;
            WidthScale = WidthScale_BeforeComma + WidthScale_AfterComma / 65536;
            WidthScalePos = Up->StreamOffset + Up->HeaderSize + ExtraBytes + 0x28;

            cout << " Width = " << Width << ", Height = " << Height << ", Width scale = " << setprecision(3) << fixed << setw(7) << WidthScale << endl;
        }
    }
}

void Structure::moov_trak_mdia(atom* Up)
{
    for (;;)
    {
        atom* Item = Up->Next();
        if (Item == NULL)
            break;
        if (!Item->IsOk)
        {
            IsOk = false;
            break;
        }

        switch (Item->Code)
        {
        case Element::minf: moov_trak_mdia_minf(Item); break;
        default:;
        }
    }
}

void Structure::moov_trak_mdia_minf(atom* Up)
{
    for (;;)
    {
        atom* Item = Up->Next();
        if (Item == NULL)
            break;
        if (!Item->IsOk)
        {
            IsOk = false;
            break;
        }

        switch (Item->Code)
        {
            case Element::stbl: moov_trak_mdia_minf_stbl(Item); break;
            case Element::vmhd: moov_trak_mdia_minf_vmhd(Item); break;
            default:;
        }
    }
}

void Structure::moov_trak_mdia_minf_stbl(atom* Up)
{
    for (;;)
    {
        atom* Item = Up->Next();
        if (Item == NULL)
            break;
        if (!Item->IsOk)
        {
            IsOk = false;
            break;
        }

        switch (Item->Code)
        {
        case Element::stsd: moov_trak_mdia_minf_stbl_stsd(Item); break;
        default:;
        }
    }
}

void Structure::moov_trak_mdia_minf_stbl_stsd(atom* Up)
{
    if (!Up->DataLoad())
    {
        IsOk = false;
        return;
    }

    if (Up->DataSize < 1)
    {
        cout << " Can not parse moov_trak_mdia_minf_stbl_stsd (no version)" << endl;
        return;
    }
    uint8_t Version = BigEndian2int8u(Up->DataContent);
    if (Version>0)
    {
        cout << " Can not parse moov_trak_mdia_minf_stbl_stsd (version not supported)" << endl;
        return;
    }
    if (Up->DataSize < 4)
    {
        cout << " Can not parse moov_trak_mdia_minf_stbl_stsd (too small)" << endl;
        return;
    }
    Up->DataSize = 8;

    if (!TrackIsVideo)
        return; //No need

    for (;;)
    {
        atom* Item = Up->Next();
        if (Item == NULL)
            break;
        if (!Item->IsOk)
        {
            IsOk = false;
            break;
        }

        moov_trak_mdia_minf_stbl_stsd_xxxxVideo(Item);
    }
    TrackIsVideo = false; // No more need
}

void Structure::moov_trak_mdia_minf_stbl_stsd_xxxxVideo(atom* Up)
{
    if (!Up->DataLoad())
    {
        IsOk = false;
        return;
    }

    if (Up->DataSize < 1)
    {
        cout << " Can not parse moov_trak_mdia_minf_stbl_stsd_xxxxVideo (no version)" << endl;
        return;
    }
    uint8_t Version = BigEndian2int8u(Up->DataContent);
    if (Version>0)
    {
        cout << " Can not parse moov_trak_mdia_minf_stbl_stsd_xxxxVideo (version not supported)" << endl;
        return;
    }
    if (Up->DataSize < 4)
    {
        cout << " Can not parse moov_trak_mdia_minf_stbl_stsd_xxxxVideo (too small)" << endl;
        return;
    }
    Up->DataSize = 0x4E;

    paspOffset = Up->StreamOffset + Up->HeaderSize + Up->DataSize;

    for (;;)
    {
        atom* Item = Up->Next();
        if (Item == NULL)
            break;
        if (!Item->IsOk)
        {
            IsOk = false;
            break;
        }

        switch (Item->Code)
        {
        case Element::pasp: moov_trak_mdia_minf_stbl_stsd_xxxx_pasp(Item); break;
        default:;
        }
    }

    //Saving offsets
    paspUpOffsets.push_back(Up->Up->Up->Up->Up->Up->Up->StreamOffset); //moov
    paspUpTotalSizes.push_back(Up->Up->Up->Up->Up->Up->Up->TotalSize);
    paspUpOffsets.push_back(Up->Up->Up->Up->Up->Up->StreamOffset); //moov_trak
    paspUpTotalSizes.push_back(Up->Up->Up->Up->Up->Up->TotalSize);
    paspUpOffsets.push_back(Up->Up->Up->Up->Up->StreamOffset); //moov_trak_mdia
    paspUpTotalSizes.push_back(Up->Up->Up->Up->Up->TotalSize);
    paspUpOffsets.push_back(Up->Up->Up->Up->StreamOffset); //moov_trak_mdia_minf
    paspUpTotalSizes.push_back(Up->Up->Up->Up->TotalSize);
    paspUpOffsets.push_back(Up->Up->Up->StreamOffset); //moov_trak_mdia_minf_stbl
    paspUpTotalSizes.push_back(Up->Up->Up->TotalSize);
    paspUpOffsets.push_back(Up->Up->StreamOffset); //moov_trak_mdia_minf_stbl_stsd
    paspUpTotalSizes.push_back(Up->Up->TotalSize);
    paspUpOffsets.push_back(Up->StreamOffset); //moov_trak_mdia_minf_stbl_stsd_xxxx
    paspUpTotalSizes.push_back(Up->TotalSize);
}

void Structure::moov_trak_mdia_minf_stbl_stsd_xxxx_pasp(atom* Up)
{
    if (!Up->DataLoad())
    {
        IsOk = false;
        return;
    }

    if (Up->DataSize != 8)
    {
        cout << " Can not parse moov_trak_mdia_minf_stbl_stsd_xxxx_pasp (wrong size)" << endl;
        return;
    }

    paspOffset = Up->StreamOffset + Up->HeaderSize;
    pasp_h = BigEndian2int32u(Up->DataContent + 0);
    pasp_v = BigEndian2int32u(Up->DataContent + 4);
}

void Structure::moov_trak_mdia_minf_vmhd(atom* Up)
{
    if (!Up->DataLoad())
    {
        IsOk = false;
        return;
    }

    if (Up->DataSize < 1)
    {
        cout << " Can not parse moov_trak_mdia_vmhd (no version)" << endl;
        return;
    }
    uint8_t Version = BigEndian2int8u(Up->DataContent);
    if (Version>0)
    {
        cout << " Can not parse moov_trak_mdia_vmhd (version not supported)" << endl;
        return;
    }
    if (Up->DataSize < 4)
    {
        cout << " Can not parse moov_trak_mdia_vmhd (too small)" << endl;
        return;
    }

    TrackIsVideo = true;
}
