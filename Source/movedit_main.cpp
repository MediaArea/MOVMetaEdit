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

const size_t BUFFER_SIZE_MAX = 0x10000000;

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 2)
    {
        cout << "Usage: " << argv[0] << " FileName [options]" << endl;
        cout << "Options" << endl;
        cout << " -p, --par: modify PAR to 9:10 (--> DAR of 4:3)" << endl;
        cout << " -w, --width-scale: modify width scale to 0.9" << endl;
        cout << " -s, --simulate: No modification" << endl;
        cout << endl;
        cout << "if no option, information is displayed, no modification" << endl;
        cout << "limitation: 720x480 or 720x486 or 720x576" << endl;
        cout << endl;
        cout << "return value:" << endl;
        cout << ">=0: OK" << endl;
        cout << "-1: Warning" << endl;
        cout << "-2: Error" << endl;
        return -1;
    }

    cout << "Args:" << endl;
    wcout << argv[0] << endl;
    wcout << argv[1] << endl;

    double wscale_New = 0;
    uint16_t par_h_New = 0;
    uint16_t par_v_New = 0;
    bool simulate = false;
    for (int argp = 2; argp < argc; ++argp)
    {
        wcout << argv[argp] << endl;
        
        if (Ztring(argv[argp]) == _T("-p")
         || Ztring(argv[argp]) == _T("-par"))
        {
            par_h_New = 9;
            par_v_New = 10;
            if (par_h_New == 0)
            {
                cout << "Can not understand PAR value " << argv[argp] << ", it must be a integer:integer value (e.g. '9:10')" << endl;
                return -1;
            }
        }
        else if (Ztring(argv[argp]) == _T("-w")
              || Ztring(argv[argp]) == _T("-width-scale"))
        {
            //stringstream Scale_Xss;
            //Scale_Xss << argv[2];
            //Scale_Xss >> wscale_New;
            wscale_New = 0.9;
            if (wscale_New == 0)
            {
                cout << "Can not understand width scale value " << argv[argp] << ", it must be a real number" << endl;
                return -1;
            }
        }
        else if (Ztring(argv[argp]) == _T("-s")
              || Ztring(argv[argp]) == _T("--simulate"))
        {
            simulate = true;
        }
        else
        {
            cout << "Unknown option " << endl;
            return -1;
        }
    }
    cout << endl;
    cout << endl;

    ZtringList List = Dir::GetAllFileNames(argv[1]);
    std::vector<Structure*> Structures;
    for (ZtringList::iterator Item = List.begin(); Item != List.end(); Item++)
    {
        File F;
        cout << "Parsing " << Item->To_Local() << std::endl;
        if (F.Open(*Item, File::Access_Read))
        {

            Structure* S = new Structure(&F, *Item);
            S->Parse();
            Structures.push_back(S);

            F.Close();
        }
        else
            cout << " Can not open file " << endl;
    }

    // Removing common data in file name
    if (!List.empty())
    {
        size_t Max = (size_t)-1;
        for (ZtringList::iterator Item = List.begin(); Item != List.end(); Item++)
        {
            if (Item->empty())
            {
                Max = (size_t)-1;
                break; // Problem
            }
            if (Max > Item->size())
                Max = Item->size() - 1;
            for (size_t Pos = 0; Pos < Max; ++Pos)
            {
                if ((*Item)[Pos] != List[0][Pos])
                {
                    Max = Pos;
                    break;
                }
            }
        }
        if (Max && Max != (size_t)-1)
        {
            if (List[0].find(PathSeparator, Max) == string::npos)
            {
                do
                {
                    --Max;
                }
                while (Max && List[0][Max] != PathSeparator);
                ++Max;
            }

            for (ZtringList::iterator Item = List.begin(); Item != List.end(); Item++)
                Item->erase(0, Max);
        }
    }
    size_t Max = 0;
    for (ZtringList::iterator Item = List.begin(); Item != List.end(); Item++)
        if (Max < Item->size())
            Max = Item->size();
    string FileNameFake;
    FileNameFake.resize(Max, __T(' '));

    cout << endl;
    cout << "Summary:" << endl;
    cout << "OK = file is correctly detected and does not need to be modified ('Yes') or is correctly detected and need to be modified ('Mod') or is correctly detected and need to be modified but not supported ('No') or there was a problem with it (empty)" << endl;
    cout << "PAR = PAR atom is not present ('No'), not present but can be (or is) inserted ('Can'), present and can be (or is) modified (value of the old PAR)" << endl;
    cout << "PAR|M = PAR will be modified ('Y') or should be modified but it is not possible due to feature not implemented ('N')" << endl;
    cout << "w-scale = (old) width scale of the picture" << endl;
    cout << "w-scale|M = w-scale will be modified ('Y')" << endl;
    cout << FileNameFake << "| OK|  PAR|M|Width|w-scale|M|" << endl;
    ZtringList::iterator ItemName = List.begin();
    ZtringList ListNotDetected;
    ZtringList ListNotCorrected;
    ZtringList ListOpenError;
    ZtringList ListSeekError;
    ZtringList ListWritingError;
    for (std::vector<Structure*>::iterator Item = Structures.begin(); Item != Structures.end(); Item++)
    {
        // Name
        string Name = ItemName->To_Local();
        Name.resize(Max, __T(' '));
        cout << Name << "|";

        if ((*Item)->IsOk)
        {
            // Test if must be modified
            char pasp_ToModify = ' ';
            char wscale_ToModify = ' ';
            if ((*Item)->Width == 720 && ((*Item)->Height == 480 || (*Item)->Height == 486 || (*Item)->Height == 576 || (*Item)->Height == 1080))
            {
                if (par_h_New && !((*Item)->pasp_h == 9 && (*Item)->pasp_v == 10))
                    pasp_ToModify = 'Y';
                if (wscale_New && !((*Item)->WidthScale >= wscale_New*0.999 && (*Item)->WidthScale <= wscale_New*1.001))
                    wscale_ToModify = 'Y';

                if (pasp_ToModify == 'Y')
                {
                    if ((*Item)->mdatOffset >= (*Item)->moovOffsetMax && !(*Item)->freeIsPresent)
                    {
                        pasp_ToModify = 'N';
                        ListNotCorrected.push_back((*Item)->Name);
                    }
                }

                if (pasp_ToModify != ' ' || wscale_ToModify != ' ')
                {
                    if (pasp_ToModify == 'N')
                        cout << " No|";
                    else
                        cout << "Mod|";
                }
                else
                    cout << "Yes|";
            }
            else
            {
                cout << "Yes|";
            }

            // OK
            File F;
            if ((pasp_ToModify || wscale_ToModify) && !simulate)
            {
                File::access_t Access = File::Access_Write;
                if (pasp_ToModify && !((*Item)->pasp_h || (*Item)->pasp_v))
                    Access = File::Access_Read_Write;
                if (!F.Open((*Item)->Name, Access))
                    ListOpenError.push_back((*Item)->Name);
            }

            // PAR
            if ((*Item)->pasp_h || (*Item)->pasp_v)
            {
                stringstream Temp;
                Temp << (*Item)->pasp_h << ":" << (*Item)->pasp_v;
                for (size_t Pos = 0; Pos < 5 - Temp.str().size(); ++Pos)
                    cout << " ";
                cout << Temp.str();
            }
            else
                cout << "     ";
            cout << "|";
            if (pasp_ToModify == 'Y' && !simulate)
            {
                if ((*Item)->pasp_h || (*Item)->pasp_v)
                {
                    if (F.GoTo((*Item)->paspOffset))
                    {
                        uint32_t h = par_h_New;
                        uint32_t v = par_v_New;
                        uint8_t Buffer[8];
                        int32u2BigEndian(Buffer, h);
                        int32u2BigEndian(Buffer + 4, v);
                        if (F.Write(Buffer, 8) != 8)
                        {
                            pasp_ToModify = 'N';
                            ListWritingError.push_back((*Item)->Name);
                        }
                    }
                    else
                    {
                        pasp_ToModify = 'N';
                        ListSeekError.push_back((*Item)->Name);
                    }
                }
                else
                {
                    size_t Buffer_Size;
                    if ((*Item)->mdatOffset < (*Item)->moovOffsetMax)
                    {
                        if (F.Size_Get() - (*Item)->paspOffset <= BUFFER_SIZE_MAX)
                            Buffer_Size = (size_t)(F.Size_Get() - (*Item)->paspOffset);
                        else
                        {
                            pasp_ToModify = 'N';
                            ListNotCorrected.push_back((*Item)->Name);
                        }
                    }
                    else if ((*Item)->freeIsPresent)
                    {
                        if ((*Item)->freeOffset - (*Item)->paspOffset <= BUFFER_SIZE_MAX)
                            Buffer_Size = (size_t)((*Item)->freeOffset - (*Item)->paspOffset);
                        else
                        {
                            pasp_ToModify = 'N';
                            ListNotCorrected.push_back((*Item)->Name);
                        }
                    }
                    
                    uint8_t* Buffer = NULL;
                    if (pasp_ToModify == 'Y')
                    {
                        if (Buffer_Size < 16)
                            Buffer_Size = 16;
                        Buffer = new uint8_t[Buffer_Size];

                        //Free
                        if ((*Item)->mdatOffset >= (*Item)->moovOffsetMax && (*Item)->freeIsPresent)
                        {
                            int64_t freeTotalSize_New64 = (*Item)->freeTotalSize - 16;
                            if (freeTotalSize_New64 <= (uint32_t)-1)
                            {
                                int32_t freeTotalSize_New32 = (int32_t)freeTotalSize_New64;
                                int32u2BigEndian(Buffer + 0, freeTotalSize_New32);
                                int32u2BigEndian(Buffer + 4, Element::free);
                            }
                            else
                            {
                                pasp_ToModify = 'N';
                                ListWritingError.push_back((*Item)->Name);
                            }
                            
                            if (pasp_ToModify == 'Y')
                            {
                                if (!F.GoTo((*Item)->freeOffset + 16))
                                {
                                    pasp_ToModify = 'N';
                                    ListSeekError.push_back((*Item)->Name);
                                }
                            }
                            if (pasp_ToModify == 'Y')
                            {
                                if (F.Write(Buffer, 8) != 8)
                                {
                                    pasp_ToModify = 'N';
                                    ListWritingError.push_back((*Item)->Name);
                                }
                            }
                        }
                    }

                    // Move
                    if (pasp_ToModify == 'Y')
                    {
                        if (!F.GoTo((*Item)->paspOffset))
                        {
                            pasp_ToModify = 'N';
                            ListWritingError.push_back((*Item)->Name);
                        }
                    }
                    if (pasp_ToModify == 'Y')
                    {
                        if (F.Read(Buffer, Buffer_Size) != Buffer_Size)
                        {
                            pasp_ToModify = 'N';
                            ListWritingError.push_back((*Item)->Name);
                        }
                    }
                    if (pasp_ToModify == 'Y')
                    {
                        if (!F.GoTo((*Item)->paspOffset + 16))
                        {
                            pasp_ToModify = 'N';
                            ListWritingError.push_back((*Item)->Name);
                        }
                    }
                    if (pasp_ToModify == 'Y')
                    {
                        if (F.Write(Buffer, Buffer_Size) != Buffer_Size)
                        {
                            pasp_ToModify = 'N';
                            ListWritingError.push_back((*Item)->Name);
                        }
                    }

                    // Write
                    if (pasp_ToModify == 'Y')
                    {
                        // Write pasp
                        int32u2BigEndian(Buffer + 0, 16);
                        int32u2BigEndian(Buffer + 4, Element::pasp);
                        int32u2BigEndian(Buffer + 8, 9);
                        int32u2BigEndian(Buffer + 12, 10);
                        if (!F.GoTo((*Item)->paspOffset))
                        {
                            pasp_ToModify = 'N';
                            ListWritingError.push_back((*Item)->Name);
                        }
                    }
                    if (pasp_ToModify == 'Y')
                    {
                        if (F.Write(Buffer, 16) != 16)
                        {
                            pasp_ToModify = 'N';
                            ListWritingError.push_back((*Item)->Name);
                        }
                    }

                    if (pasp_ToModify == 'Y')
                    {
                        vector<uint64_t>::iterator paspUpTotalSize = (*Item)->paspUpTotalSizes.begin();
                        for (vector<uint64_t>::iterator paspOffset = (*Item)->paspUpOffsets.begin(); paspOffset != (*Item)->paspUpOffsets.end(); paspOffset++)
                        {
                            int64_t paspUpTotalSize_New64 = *paspUpTotalSize + 16;
                            if (paspUpTotalSize_New64 <= (uint32_t)-1)
                            {
                                int32_t paspUpTotalSize_New32 = (int32_t)paspUpTotalSize_New64;
                                int32u2BigEndian(Buffer, paspUpTotalSize_New32);
                            }
                            else
                            {
                                pasp_ToModify = 'N';
                                ListWritingError.push_back((*Item)->Name);
                            }

                            if (pasp_ToModify == 'Y')
                            {
                                if (!F.GoTo(*paspOffset))
                                {
                                    pasp_ToModify = 'N';
                                    ListWritingError.push_back((*Item)->Name);
                                    break;
                                }
                                if (F.Write(Buffer, 4) != 4)
                                {
                                    pasp_ToModify = 'N';
                                    ListWritingError.push_back((*Item)->Name);
                                    break;
                                }

                                paspUpTotalSize++;
                            }
                        }
                    }
                }
            }
            cout << pasp_ToModify;
            cout << "|";

            // Width
            if ((*Item)->Width)
            {
                cout << setw(5) << (*Item)->Width;
            }
            else
                cout << "     ";
            cout << "|";

            // w-scale
            if ((*Item)->WidthScale)
            {
                cout << setprecision(3) << fixed << setw(7) << (*Item)->WidthScale;
                cout << "|";
                if (wscale_ToModify == 'Y' && !simulate)
                {
                    if (F.GoTo((*Item)->WidthScalePos))
                    {
                        uint16_t Before = 0;
                        uint16_t After = (uint16_t)float64_int64s(0.9 * 0x10000);
                        uint8_t Buffer[4];
                        int16u2BigEndian(Buffer, Before);
                        int16u2BigEndian(Buffer + 2, After);
                        if (F.Write(Buffer, 4) != 4)
                        {
                            wscale_ToModify = 'N';
                            ListWritingError.push_back((*Item)->Name);
                        }
                    }
                    else
                    {
                        wscale_ToModify = 'N';
                        ListSeekError.push_back((*Item)->Name);
                    }
                }
                cout << wscale_ToModify;
            }
            else
                cout << "       | ";
            cout << "|" << endl;
        }
        else
        {
            ListNotDetected.push_back((*Item)->Name);
            cout << "   |     | |     |       | |" << endl;
        }

        ItemName++;
    }

    if (!ListNotDetected.empty())
    {
        cout << endl;
        cout << "Warning, PAR for these files was not corrected (file not well detected):" << endl;
        for (ZtringList::iterator Item = ListNotDetected.begin(); Item != ListNotDetected.end(); Item++)
            cout << Item->To_Local() << endl;
    }

    if (!ListNotCorrected.empty())
    {
        cout << endl;
        cout << "Warning, PAR for these files was not corrected (feature not implemented):" << endl;
        for (ZtringList::iterator Item = ListNotCorrected.begin(); Item != ListNotCorrected.end(); Item++)
            cout << Item->To_Local() << endl;
    }

    if (!ListOpenError.empty())
    {
        cout << endl;
        cout << "Error, unable to open these files for writing:" << endl;
        for (ZtringList::iterator Item = ListOpenError.begin(); Item != ListOpenError.end(); Item++)
            cout << Item->To_Local() << endl;
    }

    if (!ListSeekError.empty())
    {
        cout << endl;
        cout << "Error, unable to seek in these files:" << endl;
        for (ZtringList::iterator Item = ListSeekError.begin(); Item != ListSeekError.end(); Item++)
            cout << Item->To_Local() << endl;
    }

    if (!ListWritingError.empty())
    {
        cout << endl;
        cout << "Error, write error (files may be corrupted) with these files:" << endl;
        for (ZtringList::iterator Item = ListWritingError.begin(); Item != ListWritingError.end(); Item++)
            cout << Item->To_Local() << endl;
    }

    if (!ListOpenError.empty() || !ListSeekError.empty() || !ListWritingError.empty())
        return -2; //Error
    if (!ListNotDetected.empty() || !ListNotCorrected.empty())
        return -1; //Warning
    return (int)Structures.size();
}

