/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include "movedit_structure.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include "ZenLib/Dir.h"
#include "ZenLib/File.h"
using namespace std;
using namespace ZenLib;

Structure::Structure(File* F_, const Ztring &Name_)
    :
    IsOk(true),
    mdatIsPresent(false),
    moovIsPresent(false),
    freeIsPresent(false),
    moovOffsetMax((uint64_t)-1),
    mdatOffset((uint64_t)-1),
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

    if (!mdatIsPresent)
    {
        cout << " Can not find the mdat atom" << endl;
        IsOk = false; //At least 1 mdat
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
    Tracks.resize(Tracks.size() + 1); //Add 1 track
    Track = &Tracks[Tracks.size() - 1];

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
    if (Width_Temp && Height_Temp)
    {
            double WidthScale_BeforeComma = (double)BigEndian2int16u(Up->DataContent + ExtraBytes + 0x28);
            double WidthScale_AfterComma = (double)BigEndian2int16u(Up->DataContent + ExtraBytes + 0x2A);
            Track->Width = Width_Temp;
            Track->Height = Height_Temp;
            Track->WidthScale = WidthScale_BeforeComma + WidthScale_AfterComma / 65536;
            Track->WidthScalePos = Up->StreamOffset + Up->HeaderSize + ExtraBytes + 0x28;

            //cout << " Width = " << Track->Width << ", Height = " << Track->Height << ", Width scale = " << setprecision(3) << fixed << setw(7) << Track->WidthScale << endl;
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

    if (!Track->IsVideo)
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

    Track->Videos.resize(Track->Videos.size() + 1);
    Video = &Track->Videos[Track->Videos.size() - 1];

    Video->pasp.Offset = Up->StreamOffset + Up->HeaderSize + Up->DataSize;

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
    Video->UpOffsets.push_back(Up->Up->Up->Up->Up->Up->Up->StreamOffset); //moov
    Video->UpTotalSizes.push_back(Up->Up->Up->Up->Up->Up->Up->TotalSize);
    Video->UpOffsets.push_back(Up->Up->Up->Up->Up->Up->StreamOffset); //moov_trak
    Video->UpTotalSizes.push_back(Up->Up->Up->Up->Up->Up->TotalSize);
    Video->UpOffsets.push_back(Up->Up->Up->Up->Up->StreamOffset); //moov_trak_mdia
    Video->UpTotalSizes.push_back(Up->Up->Up->Up->Up->TotalSize);
    Video->UpOffsets.push_back(Up->Up->Up->Up->StreamOffset); //moov_trak_mdia_minf
    Video->UpTotalSizes.push_back(Up->Up->Up->Up->TotalSize);
    Video->UpOffsets.push_back(Up->Up->Up->StreamOffset); //moov_trak_mdia_minf_stbl
    Video->UpTotalSizes.push_back(Up->Up->Up->TotalSize);
    Video->UpOffsets.push_back(Up->Up->StreamOffset); //moov_trak_mdia_minf_stbl_stsd
    Video->UpTotalSizes.push_back(Up->Up->TotalSize);
    Video->UpOffsets.push_back(Up->StreamOffset); //moov_trak_mdia_minf_stbl_stsd_xxxxVideo
    Video->UpTotalSizes.push_back(Up->TotalSize);
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

    Video->pasp.Offset = Up->StreamOffset + Up->HeaderSize;
    Video->pasp.h = BigEndian2int32u(Up->DataContent + 0);
    Video->pasp.v = BigEndian2int32u(Up->DataContent + 4);
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

    Tracks[Tracks.size() - 1].IsVideo = true;
}
