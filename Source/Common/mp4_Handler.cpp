/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include "Common/mp4_Handler.h"
#include "Common/mp4/mp4_.h"
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include "ZenLib/File.h"
#include "ZenLib/Dir.h"
#include "ThirdParty/json/json.hpp"

#ifdef MACSTORE
#include "Common/Mac_Helpers.h"
#include "mp4_Handler.h"
#endif

using namespace std;
using namespace ZenLib;
using namespace nlohmann;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
const string mp4_Handler_EmptyZtring_Const; //Use it when we can't return a reference to a true Ztring, const version
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
static const vector<string> mp4_mdhd_Languages=
{
    "en", // English
    "fr", // French
    "de", // German
    "it", // Italian
    "nl", // Dutch
    "sv", // Swedish
    "es", // Spanish
    "da", // Danish
    "pt", // Portuguese
    "no", // Norwegian
    "he", // Hebrew
    "ja", // Japanese
    "ar", // Arabic
    "fi", // Finnish
    "el", // Greek
    "is", // Icelandic
    "mt", // Maltese
    "tr", // Turkish
    "hr", // Croatian
    "zh-tw", // Chinese (Taiwan)
    "ur", // Urdu
    "hi", // Hindi
    "th", // Thai
    "ko", // Korean
    "lt", // Lithuanian
    "pl", // Polish
    "hu", // Hungarian
    "et", // Estonian
    "lv", // Latvian
    "smi", // Sami
    "fo", // Faroese
    "fa", // Persian
    "ru", // Russian
    "zh-cn", // Chinese (China)
    "nl-be", // Flemish
    "ga", // Irish
    "sq", // Albanian
    "ro", // Romanian
    "cs", // Czech
    "sk", // Slovak
    "sl", // Slovenian
    "yi", // Yiddish
    "sr", // Serbian
    "mk", // Macedonian
    "bg", // Bulgarian
    "uk", // Ukrainian
    "be", // Belarusian
    "uz", // Uzbek
    "kk", // Kazakh
    "az", // Azerbaijani
    "hy-az", // Armenian (Azerbaijani)
    "hy", // Armenian
    "ka", // Georgian
    "mo", // Moldavian
    "ky", // Kirghiz
    "tg", // Tajik
    "tk", // Turkmen
    "mn-cn", // Mongolian (China)
    "mn", // Mongolian
    "ps", // Pushto
    "ku", // Kurdish
    "ks", // Kashmiri
    "sd", // Sindhi
    "bo", // Tibetan
    "ne", // Nepali
    "sa", // Sanskrit
    "mr", // Marathi
    "bn", // Bengali
    "as", // Assamese
    "gu", // Gujarati
    "pa", // Panjabi
    "or", // Oriya
    "ml", // Malayalam
    "kn", // Kannada
    "ta", // Tamil
    "te", // Telugu
    "si", // Sinhala
    "my", // Burmese
    "km", // Khmer
    "lo", // Lao
    "vi", // Vietnamese
    "id", // Indonesian
    "tl", // Tagalog
    "ms", // Malay
    "ms-bn", // Malay (Brunei)
    "am", // Amharic
    "86", // Empty
    "om", // Oromo
    "so", // Somali
    "sw", // Swahili
    "rw", // Kinyarwanda
    "rn", // Rundi
    "ny", // Nyanja
    "mg", // Malagasy
    "eo", // Esperanto
    // Gap 95-128
    "cy", // Welsh
    "eu", // Basque
    "ca", // Catalan
    "la", // Latin
    "qu", // Quechua
    "gn", // Guarani
    "ay", // Aymara
    "tt", // Tatar
    "ug", // Uighur
    "dz", // Dzongkha
    "jv", // Javanese
};

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
int tfsxml_next_named(tfsxml_string* tfsxml_priv, tfsxml_string* result, const char* name)
{

    while (!tfsxml_next(tfsxml_priv, result))
    {
        if (!tfsxml_strcmp_charp(*result, name))
            return 0;
    }

    return -1;
}

//---------------------------------------------------------------------------
int tfsxml_last_named(tfsxml_string* tfsxml_priv, tfsxml_string* result, const char* name)
{
    while (!tfsxml_next_named(tfsxml_priv, result, name))
    {
        tfsxml_string tfsxml_priv_local = *tfsxml_priv, result_local = *result;
        if (tfsxml_next_named(&tfsxml_priv_local, &result_local, name))
            return 0;
    }

    return -1;
}

//---------------------------------------------------------------------------
string mp4_chan_ChannelDescription (uint32_t ChannelLabel)
{
    switch(ChannelLabel)
    {
        case   0 : return "(None)";
        case   1 : return "L";
        case   2 : return "R";
        case   3 : return "C";
        case   4 : return "LFE";
        case   5 : return "Ls";
        case   6 : return "Rs";
        case   7 : return "Lc";
        case   8 : return "Rc";
        case   9 : return "Cs";
        case  10 : return "Lsd";
        case  11 : return "Rsd";
        case  12 : return "Tcs";
        case  13 : return "Vhl";
        case  14 : return "Vhc";
        case  15 : return "Vhr";
        case  16 : return "Trs";
        case  17 : return "Trs";
        case  18 : return "Trs";
        case  33 : return "Lrs";
        case  34 : return "Rrs";
        case  35 : return "Lw";
        case  36 : return "Rw";
        case  37 : return "LFE2";
        case  38 : return "Lt";
        case  39 : return "Rt";
        case  40 : return "HearingImpaired";
        case  41 : return "Narration";
        case  42 : return "M";
        case  43 : return "DialogCentricMix";
        case  44 : return "CenterSurroundDirect";
        case  45 : return "Haptic";
        case 200 : return "W";
        case 201 : return "X";
        case 202 : return "Y";
        case 203 : return "Z";
        case 204 : return "M";
        case 205 : return "S";
        case 206 : return "X";
        case 207 : return "Y";
        case 301 : return "HeadphonesLeft";
        case 302 : return "HeadphonesRight";
        case 304 : return "ClickTrack";
        case 305 : return "ForeignLanguage";
        case 400 : return "Discrete";
        default  :
                    if ((ChannelLabel>>16)==1) //0x10000 to 0x1FFFF, numbered Discrete
                        return "Discrete-"+Ztring::ToZtring(ChannelLabel&0xFFFF).To_UTF8();
                    return Ztring::ToZtring(ChannelLabel).To_UTF8();
    }
}

//---------------------------------------------------------------------------
bool mp4_chan_ChannelCodes (string ChannelLabels, vector<uint32_t> &Codes, bool& Ignore, bool& Delete)
{
    Delete=false;
    Ignore=false;
    Codes.clear();

    if(ChannelLabels.empty())
        return false;

    if (ChannelLabels=="delete")
    {
        Delete=true;
        return true;
    }
    else if (ChannelLabels=="absent" || ChannelLabels=="nodescription")
    {
        Ignore=true;
        return true;
    }

    size_t Start=0, End=0;
    do
    {
        End = ChannelLabels.find('+', Start);
        string Current=ChannelLabels.substr(Start, End!=string::npos?End-Start:string::npos);
        uint32_t Code;

        istringstream iss(Current);
        iss >> Code;
        if (!iss.fail() && iss.eof())
        {
            Codes.push_back(Code);
            continue;
        }

        transform(Current.begin(), Current.end(), Current.begin(), ::tolower);
        if (Current=="l")
            Code=1;
        else if (Current=="r")
            Code=2;
        else if (Current=="c")
            Code=3;
        else if (Current=="lfe")
            Code=4;
        else if (Current=="ls")
            Code=5;
        else if (Current=="rs")
            Code=6;
        else if (Current=="lc")
            Code=7;
        else if (Current=="rc")
            Code=8;
        else if (Current=="cs")
            Code=9;
        else if (Current=="lsd")
            Code=10;
        else if (Current=="rsd")
            Code=11;
        else if (Current=="tcs")
            Code=12;
        else if (Current=="vhl")
            Code=13;
        else if (Current=="vhc")
            Code=14;
        else if (Current=="vhr")
            Code=15;
        else if (Current=="trs")
            Code=16;
        else if (Current=="trs2")
            Code=17;
        else if (Current=="trs3")
            Code=18;
        else if (Current=="lrs")
            Code=33;
        else if (Current=="rrs")
            Code=34;
        else if (Current=="lw")
            Code=35;
        else if (Current=="rw")
            Code=36;
        else if (Current=="lfe2")
            Code=37;
        else if (Current=="lt")
            Code=38;
        else if (Current=="rt")
            Code=39;
        else if (Current=="hearingimpaired")
            Code=40;
        else if (Current=="narration")
            Code=41;
        else if (Current=="m")
            Code=42;
        else if (Current=="dialogcentricmix")
            Code=43;
        else if (Current=="centersurrounddirect")
            Code=44;
        else if (Current=="haptic")
            Code=45;
        else if (Current=="w")
            Code=200;
        else if (Current=="x")
            Code=201;
        else if (Current=="y")
            Code=202;
        else if (Current=="z")
            Code=203;
        else if (Current=="m2")
            Code=204;
        else if (Current=="s")
            Code=205;
        else if (Current=="x2")
            Code=206;
        else if (Current=="y2")
            Code=207;
        else if (Current=="headphonesleft")
            Code=301;
        else if (Current=="headphonesright")
            Code=302;
        else if (Current=="clicktrack")
            Code=304;
        else if (Current=="foreignlanguage")
            Code=305;
        else if (Current=="discrete")
            Code=400;
        else if (Current.size()>9 && Current.find("discrete-")==0)
        {
            uint32_t Number=0;
            istringstream iss(Current.substr(9));
            iss >> Number;
            if (iss.fail() || !iss.eof() || Number>0xFFFF)
                return false;

            Code=0x10000+Number;
        }
        else
            return false;

        Codes.push_back(Code);

    } while((Start=ChannelLabels.find_first_not_of('+', End))!=string::npos);

    return true;
}

//---------------------------------------------------------------------------
string mp4_chan_ChannelLayout(uint32_t ChannelLayoutCode)
{
    switch (ChannelLayoutCode)
    {
        case (0<<16) | 0: return "UseChannelDescriptions";
        case (1<<16) | 0: return "UseChannelBitmap";
        case (100<<16) | 1: return "Mono";
        case (101<<16) | 2: return "Stereo";
        case (102<<16) | 2: return "StereoHeadphones";
        case (103<<16) | 2: return "MatrixStereo";
        case (104<<16) | 2: return "MidSide";
        case (105<<16) | 2: return "XY";
        case (106<<16) | 2: return "Binaural";
        case (107<<16) | 4: return "Ambisonic_BFormat";
        case (108<<16) | 4: return "Quadraphonic";
        case (109<<16) | 5: return "Pentagonal";
        case (110<<16) | 6: return "Hexagonal";
        case (111<<16) | 8: return "Octagonal";
        case (112<<16) | 8: return "Cube";
        // MPEG_1.0 is Mono
        // MPEG_2.0 is Stereo
        case (113<<16) | 3: return "MPEG_3.0A";
        case (114<<16) | 3: return "MPEG_3.0B";
        case (115<<16) | 4: return "MPEG_4.0A";
        case (116<<16) | 4: return "MPEG_4.0B";
        case (117<<16) | 5: return "MPEG_5.0A";
        case (118<<16) | 5: return "MPEG_5.0B";
        case (119<<16) | 5: return "MPEG_5.0C";
        case (120<<16) | 5: return "MPEG_5.0D";
        case (121<<16) | 6: return "MPEG_5.1A";
        case (122<<16) | 6: return "MPEG_5.1B";
        case (123<<16) | 6: return "MPEG_5.1C";
        case (124<<16) | 6: return "MPEG_5.1D";
        case (125<<16) | 7: return "MPEG_6.1A";
        case (126<<16) | 8: return "MPEG_7.1A";
        case (127<<16) | 8: return "MPEG_7.1B";
        case (128<<16) | 8: return "MPEG_7.1C";
        case (129<<16) | 8: return "Emagic_7.1";
        case (130<<16) | 8: return "SMPTE_DTV";
        // ITU_1.0 is Mono
        // ITU_2.0 is Stereo
        case (131<<16) | 3: return "ITU_2.1";
        case (132<<16) | 4: return "ITU_2.2";
        // ITU_3.0 is MPEG_3.0A
        // ITU_3.1 is MPEG_4.0A
        // ITU_3.2 is MPEG_5.0A
        // ITU_3.2.1 is MPEG_5.1A
        // ITU 3.4.1 is MPEG_7.1C
        // DVD_0 is Mono
        // DVD_1 is Stereo
        // DVD_2 is ITU_2.1
        // DVD_3 is ITU_2.2
        case (133<<16) | 3: return "DVD_4";
        case (134<<16) | 4: return "DVD_5";
        case (135<<16) | 5: return "DVD_6";
        // DVD_7 is MPEG_3.0A
        // DVD_8 is MPEG_4.0A
        // DVD_9 is MPEG_5.0A
        case (136<<16) | 4: return "DVD_10";
        case (137<<16) | 5: return "DVD_11";
        // DVD_12 is MPEG_5.1A
        // DVD_13 is MPEG_4.0A
        // DVD_14 is MPEG_5.0A
        // DVD_15 is DVD_10
        // DVD_16 is DVD_11
        // DVD_17 is MPEG_5.1A
        case (138<<16) | 5: return "DVD_18";
        // DVD_19 is MPEG_5.0B
        // DVD_20 is MPEG_5.1B
        // AudioUnit_4 is Quadraphonic
        // AudioUnit_5 is Pentagonal
        // AudioUnit_6 is Hexagonal
        // AudioUnit_8 is Octagonal
        // AudioUnit_5.0 is MPEG_5.0B
        case (139<<16) | 6: return "AudioUnit_6.0";
        case (140<<16) | 7: return "AudioUnit_7.0";
        case (148<<16) | 7: return "AudioUnit_7.0Front";
        // AudioUnit_5.1 is MPEG_5.1A
        // AudioUnit_6.1 is MPEG_6.1A
        // AudioUnit_7.1 is MPEG_7.1C
        // AudioUnit_7.1Front is MPEG_7.1A
        // AAC_3.0 is MPEG_3.0B
        // AAC_Quadraphonic is Quadraphonic
        // AAC_4.0 is MPEG_4.0B
        // AAC_5.0 is MPEG_5.0D
        // AAC_5.1 is MPEG_5.1D
        case (141<<16) | 6: return "AAC_6.0";
        case (142<<16) | 7: return "AAC_6.1";
        case (143<<16) | 7: return "AAC_7.0";
        // AAC_7.1 is MPEG_7.1B
        case (144<<16) | 8: return "AAC_Octagonal";
        case (145<<16) | 16: return "TMH_10.2std";
        case (146<<16) | 21: return "TMH_10.2full";
        case (149<<16) | 2: return "AC3_1.0.1";
        case (150<<16) | 3: return "AC3_3.0";
        case (151<<16) | 4: return "AC3_3.1";
        case (152<<16) | 4: return "AC3_3.0.1";
        case (153<<16) | 4: return "AC3_2.1.1";
        case (154<<16) | 5: return "AC3_3.1.1";
        default:
            if ((ChannelLayoutCode & 0xFFFF0000) == 0x00000093)
                return "DiscreteInOrder-" + to_string(ChannelLayoutCode & 0xFFFF);
            else if ((ChannelLayoutCode & 0xFFFF0000) == 0xFFFF0000)
                return "Unknown-" + to_string(ChannelLayoutCode & 0xFFFF);
    }

    return to_string(ChannelLayoutCode);
}

//---------------------------------------------------------------------------
bool mp4_chan_ChannelLayoutCode(string ChannelLayoutLabel, uint32_t &Code)
{
    Code=0;

    if(ChannelLayoutLabel.empty())
        return false;

    istringstream iss(ChannelLayoutLabel);
    iss >> Code;
    if (!iss.fail() && iss.eof())
        return true;

    static const unordered_map<std::string, uint32_t> LayoutCodes = {
        {"usechanneldescriptions", (0<<16) | 0},
        {"usechannelbitmap", (1<<16) | 0},
        {"mono", (100<<16) | 1},
        {"stereo", (101<<16) | 2},
        {"stereoheadphones", (102<<16) | 2},
        {"matrixstereo", (103<<16) | 2},
        {"midside", (104<<16) | 2},
        {"xy", (105<<16) | 2},
        {"binaural", (106<<16) | 2},
        {"ambisonic_bformat", (107<<16) | 4},
        {"quadraphonic", (108<<16) | 4},
        {"pentagonal", (109<<16) | 5},
        {"hexagonal", (110<<16) | 6},
        {"octagonal", (111<<16) | 8},
        {"cube", (112<<16) | 8},
        {"mpeg_1.0", (100<<16) | 1}, // Mono
        {"mpeg_2.0", (101<<16) | 2}, // Stereo
        {"mpeg_3.0a", (113<<16) | 3},
        {"mpeg_3.0b", (114<<16) | 3},
        {"mpeg_4.0a", (115<<16) | 4},
        {"mpeg_4.0b", (116<<16) | 4},
        {"mpeg_5.0a", (117<<16) | 5},
        {"mpeg_5.0b", (118<<16) | 5},
        {"mpeg_5.0c", (119<<16) | 5},
        {"mpeg_5.0d", (120<<16) | 5},
        {"mpeg_5.1a", (121<<16) | 6},
        {"mpeg_5.1b", (122<<16) | 6},
        {"mpeg_5.1c", (123<<16) | 6},
        {"mpeg_5.1d", (124<<16) | 6},
        {"mpeg_6.1a", (125<<16) | 7},
        {"mpeg_7.1a", (126<<16) | 8},
        {"mpeg_7.1b", (127<<16) | 8},
        {"mpeg_7.1c", (128<<16) | 8},
        {"emagic_7.1", (129<<16) | 8},
        {"smpte_dtv", (130<<16) | 8},
        {"itu_1.0", (100<<16) | 1}, // Mono
        {"itu_2.0", (101<<16) | 2}, // Stereo
        {"itu_2.1", (131<<16) | 3},
        {"itu_2.2", (132<<16) | 4},
        {"itu_3.0", (113<<16) | 3}, // MPEG_3.0A
        {"itu_3.1", (115<<16) | 4}, // MPEG_4.0A
        {"itu_3.2", (117<<16) | 5}, // MPEG_5.0A
        {"itu_3.2.1", (121<<16) | 6}, // MPEG_5.1A
        {"itu_3.4.1", (128<<16) | 8}, // MPEG_7.1C
        {"dvd_0", (100<<16) | 1}, // Mono
        {"dvd_1", (101<<16) | 2}, // Stereo
        {"dvd_2", (131<<16) | 3}, // ITU_2.1
        {"dvd_3", (132<<16) | 4}, // ITU_2.2
        {"dvd_4", (133<<16) | 3},
        {"dvd_5", (134<<16) | 4},
        {"dvd_6", (135<<16) | 5},
        {"dvd_7", (113<<16) | 3}, // MPEG_3.0A
        {"dvd_8", (115<<16) | 4}, // MPEG_4.0A
        {"dvd_9", (117<<16) | 5}, // MPEG_5.0A
        {"dvd_10", (136<<16) | 4},
        {"dvd_11", (137<<16) | 5},
        {"dvd_12", (121<<16) | 6}, // MPEG_5.1A
        {"dvd_13", (115<<16) | 4}, // MPEG_4.0A
        {"dvd_14", (117<<16) | 5}, // MPEG_5.0A
        {"dvd_15", (136<<16) | 4}, // DVD_10
        {"dvd_16", (137<<16) | 5}, // DVD_11
        {"dvd_17", (121<<16) | 6}, // MPEG_5.1A
        {"dvd_18", (138<<16) | 5},
        {"dvd_19", (118<<16) | 5}, // MPEG_5.0B
        {"dvd_20", (122<<16) | 6}, // MPEG_5.1B
        {"audiounit_4", (108<<16) | 4}, // Quadraphonic
        {"audiounit_5", (109<<16) | 5}, // Pentagonal
        {"audiounit_6", (110<<16) | 6}, // Hexagonal
        {"audiounit_8", (111<<16) | 8}, // Octogonal
        {"audiounit_5.0", (118<<16) | 5}, // MPEG_5.0B
        {"audiounit_6.0", (139<<16) | 6},
        {"audiounit_7.0", (140<<16) | 7},
        {"audiounit_7.0front", (148<<16) | 7},
        {"audiounit_5.1", (121<<16) | 6}, // MPEG_5.1A
        {"audiounit_6.1", (125<<16) | 7}, // MPEG_6.1A
        {"audiounit_7.1", (128<<16) | 8}, // MPEG_7.1C
        {"audiounit_7.1front", (126<<16) | 8}, // MPEG_7.1A
        {"aac_3.0",  (114<<16) | 3}, // MPEG_3.0B
        {"aac_quadraphonic", (108<<16) | 4}, // Quadraphonic
        {"aac_4.0", (116<<16) | 4}, // MPEG_4.0B
        {"aac_5.0", (120<<16) | 5}, // MPEG_5.0D
        {"aac_5.1", (124<<16) | 6}, // MPEG_5.1D
        {"aac_6.0", (141<<16) | 6},
        {"aac_6.1", (142<<16) | 7},
        {"aac_7.0", (143<<16) | 7},
        {"aac_7.1", (127<<16) | 8}, // MPEG_7.1B
        {"aac_octagonal", (144<<16) | 8},
        {"tmh_10.2std", (145<<16) | 16},
        {"tmh_10.2full", (146<<16) | 21},
        {"ac3_1.0.1", (149<<16) | 2},
        {"ac3_3.0", (150<<16) | 3},
        {"ac3_3.1", (151<<16) | 4},
        {"ac3_3.0.1", (152<<16) | 4},
        {"ac3_2.1.1", (153<<16) | 4},
        {"ac3_3.1.1", (154<<16) | 5}
    };

    transform(ChannelLayoutLabel.begin(), ChannelLayoutLabel.end(), ChannelLayoutLabel.begin(), ::tolower);
    auto It=LayoutCodes.find(ChannelLayoutLabel);
    if (It!=LayoutCodes.end())
        Code=It->second;
    else if (ChannelLayoutLabel.size()>16 && ChannelLayoutLabel.find("discreteinorder-")==0)
    {
        uint32_t Number=0;
        istringstream iss(ChannelLayoutLabel.substr(16));
        iss >> Number;
        if (iss.fail() || !iss.eof() || Number>0xFFFF)
            return false;

        Code=(147<<16) | Number;
    }
    else if (ChannelLayoutLabel.size()>8 && ChannelLayoutLabel.find("unknown-")==0)
    {
        uint32_t Number=0;
        istringstream iss(ChannelLayoutLabel.substr(8));
        iss >> Number;
        if (iss.fail() || !iss.eof() || Number>0xFFFF)
            return false;

        Code=0xFFFF0000 | Number;
    }
    else
        return false;

    return true;
}

//---------------------------------------------------------------------------
string mp4_mdhd_LanguageLabel(uint16_t LanguageCode)
{
    if (LanguageCode>=0x400 && LanguageCode!=0x7FFF && LanguageCode!=0xFFFF)
    {
        string Temp;
        Temp.append(1, (char)((LanguageCode>>10&0x1F)+0x60));
        Temp.append(1, (char)((LanguageCode>> 5&0x1F)+0x60));
        Temp.append(1, (char)((LanguageCode>> 0&0x1F)+0x60));
        return Temp;
    }

    if (LanguageCode>94)
        LanguageCode-=(128-94); // Gap 95-128

    if (LanguageCode<mp4_mdhd_Languages.size())
        return mp4_mdhd_Languages[LanguageCode];

    return to_string(LanguageCode);
}

//---------------------------------------------------------------------------
bool mp4_mdhd_LanguageCode(string LanguageLabel, uint16_t &Code, bool &Ignore, bool &Delete)
{
    Delete=false;
    Ignore=false;
    Code=0;

    if(LanguageLabel.empty())
        return false;

    istringstream iss(LanguageLabel);
    iss >> Code;
    if (!iss.fail() && iss.eof())
        return true;

    transform( LanguageLabel.begin(), LanguageLabel.end(), LanguageLabel.begin(), ::tolower);
    vector<string>::const_iterator It=find(mp4_mdhd_Languages.begin(), mp4_mdhd_Languages.end(), LanguageLabel);
    if (It!=mp4_mdhd_Languages.end())
    {
        Code=(uint16_t)(It-mp4_mdhd_Languages.begin());
        return true;
    }

    if (LanguageLabel.size()==3 &&
        LanguageLabel[0] > 0x60 && LanguageLabel[0] < 0x7B &&
        LanguageLabel[1] > 0x60 && LanguageLabel[1] < 0x7B &&
        LanguageLabel[2] > 0x60 && LanguageLabel[2] < 0x7B)
    {
        Code=(LanguageLabel[0]-0x60)<<10;
        Code|=(LanguageLabel[1]-0x60)<<5;
        Code|=(LanguageLabel[2]-0x60)<<0;
        return true;
    }

    return false;
}

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
mp4_Handler::mp4_Handler ()
{
    //Configuration
    Overwrite_Reject=false;
    NewChunksAtTheEnd=false;
    HdrDataFromXmlId=(size_t)-1;

    //Internal
    Chunks=NULL;
    File_IsValid=false;
    File_IsCanceled=false;
}

//---------------------------------------------------------------------------
mp4_Handler::~mp4_Handler ()
{
    delete Chunks; //Chunks=NULL;
}

//***************************************************************************
// I/O
//***************************************************************************

//---------------------------------------------------------------------------
bool mp4_Handler::Open(const string &FileName)
{
    //Init
    PerFile_Error.str(string());
    File_IsValid=false;
    File_IsCanceled=false;
    bool ReturnValue=true;
    
    //Global info
    delete Chunks; Chunks=new mp4();
    Chunks->Global->File_Name.From_Local(FileName);

    //Opening file
    if (!File::Exists(Chunks->Global->File_Name) || !Chunks->Global->In.Open(Chunks->Global->File_Name))
    {
        Errors<<FileName<<": File does not exist"<<endl;
        PerFile_Error<<"File does not exist";
        return false;
    }
    Chunks->Global->File_Size=Chunks->Global->In.Size_Get();
    Chunks->Global->File_Date=Chunks->Global->In.Created_Local_Get();
    if (Chunks->Global->File_Date.empty())
        Chunks->Global->File_Date=Chunks->Global->In.Modified_Local_Get();

    //Base
    mp4_Base::block Chunk;
    Chunk.Header.Size=16;
    Chunk.Content.Size=Chunks->Global->File_Size;
    Options_Update();

    //Parsing
    try
    {
        Chunks->Read(Chunk);
        File_IsValid=true;
    }
    catch (exception_canceled &)
    {
        CriticalSectionLocker(Chunks->Global->CS);
        File_IsCanceled=true;
        Chunks->Global->Canceling=false;
        ReturnValue=false;
    }
    catch (exception_read_block &e)
    {
        Errors<<Chunks->Global->File_Name.To_Local()<<": "<<(Chunk.Header.Name?(Ztring().From_CC4(Chunk.Header.Name).To_Local()+" "):string())<<" "<<e.what()<<endl;
        PerFile_Error<<(Chunk.Header.Name?(Ztring().From_CC4(Chunk.Header.Name).To_Local()+" "):string())<<e.what();
    }
    catch (exception &e)
    {
        Errors<<Chunks->Global->File_Name.To_Local() <<": "<<e.what()<<endl;
        PerFile_Error<<e.what();
        ReturnValue=false;
    }

    //Cleanup
    Chunks->Global->In.Close();

    CriticalSectionLocker(Chunks->Global->CS);
    Chunks->Global->Progress=1;
    
    return ReturnValue;
}

//---------------------------------------------------------------------------
bool mp4_Handler::Save()
{
    Chunks->Global->CS.Enter();
    Chunks->Global->Progress=(float)0.05;
    Chunks->Global->CS.Leave();
    
    //Init
    PerFile_Error.str(string());

    //Integrity
    if (Chunks==NULL)
    {
        Errors<<"(No file name): Internal error"<<endl;
        PerFile_Error << "Internal error";
        return false;
    }

    //Write only if modified
    if (!IsModified_Get())
    {
        Information<<Chunks->Global->File_Name.To_Local()<<": Nothing to do"<<endl;
        return false;
    }

    //Modifying the blocks in memory
    if (Chunks->Global->moov_meta_hdlr_Modified)
        Chunks->Modify(Elements::moov, Elements::moov_meta, Elements::moov_meta_hdlr);
    if (Chunks->Global->moov_meta_keys_Modified)
        Chunks->Modify(Elements::moov, Elements::moov_meta, Elements::moov_meta_keys);
    if (Chunks->Global->moov_meta_ilst_Modified)
        Chunks->Modify(Elements::moov, Elements::moov_meta, Elements::moov_meta_ilst);

    //Modify fist video track
    {
        size_t trak_Index=0;
        for (size_t Pos=0; Pos<Chunks->Subs.size(); Pos++)
        {
            bool Found=false;
            if (Chunks->Subs[Pos]->Chunk.Header.Name==Elements::moov)
            {
                for (size_t Pos2=0; Pos2<Chunks->Subs[Pos]->Subs.size(); Pos2++)
                {
                    if (Chunks->Subs[Pos]->Subs[Pos2]->Chunk.Header.Name==Elements::moov_trak)
                    {
                        if (trak_Index<Chunks->Global->moov_trak.size() && Chunks->Global->moov_trak[trak_Index]->IsVideo)
                        {
                            bool Modified=false;
                            if (Chunks->Global->moov_trak_tapt_clef_Modified)
                            {
                                Chunks->Subs[Pos]->Subs[Pos2]->Modify(Elements::moov_trak_tapt, Elements::moov_trak_tapt_clef);
                                Modified=true;
                            }
                            if (Chunks->Global->moov_trak_tapt_prof_Modified)
                            {
                                Chunks->Subs[Pos]->Subs[Pos2]->Modify(Elements::moov_trak_tapt, Elements::moov_trak_tapt_prof);
                                Modified=true;
                            }
                            if (Chunks->Global->moov_trak_tapt_enof_Modified)
                            {
                                Chunks->Subs[Pos]->Subs[Pos2]->Modify(Elements::moov_trak_tapt, Elements::moov_trak_tapt_enof);
                                Modified=true;
                            }
                            if (Chunks->Global->moov_trak[trak_Index]->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present)
                            {
                                if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Modified)
                                {
                                    Chunks->Subs[Pos]->Subs[Pos2]->Modify(Elements::moov_trak_mdia, Elements::moov_trak_mdia_minf, Elements::moov_trak_mdia_minf_stbl, Elements::moov_trak_mdia_minf_stbl_stsd, Elements::moov_trak_mdia_minf_stbl_stsd_xxxxVideo);
                                    Modified=true;
                                }
                                if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap_Modified)
                                {
                                    Chunks->Subs[Pos]->Subs[Pos2]->Modify(Elements::moov_trak_mdia, Elements::moov_trak_mdia_minf, Elements::moov_trak_mdia_minf_stbl, Elements::moov_trak_mdia_minf_stbl_stsd, Elements::moov_trak_mdia_minf_stbl_stsd_xxxxVideo, Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_clap);
                                    Modified=true;
                                }
                                if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr_Modified)
                                {
                                    Chunks->Subs[Pos]->Subs[Pos2]->Modify(Elements::moov_trak_mdia, Elements::moov_trak_mdia_minf, Elements::moov_trak_mdia_minf_stbl, Elements::moov_trak_mdia_minf_stbl_stsd, Elements::moov_trak_mdia_minf_stbl_stsd_xxxxVideo, Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_colr);
                                    Modified=true;
                                }
                                if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel_Modified)
                                {
                                    Chunks->Subs[Pos]->Subs[Pos2]->Modify(Elements::moov_trak_mdia, Elements::moov_trak_mdia_minf, Elements::moov_trak_mdia_minf_stbl, Elements::moov_trak_mdia_minf_stbl_stsd, Elements::moov_trak_mdia_minf_stbl_stsd_xxxxVideo, Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_fiel);
                                    Modified=true;
                                }
                                if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama_Modified)
                                {
                                    Chunks->Subs[Pos]->Subs[Pos2]->Modify(Elements::moov_trak_mdia, Elements::moov_trak_mdia_minf, Elements::moov_trak_mdia_minf_stbl, Elements::moov_trak_mdia_minf_stbl_stsd, Elements::moov_trak_mdia_minf_stbl_stsd_xxxxVideo, Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_gama);
                                    Modified=true;
                                }
                                if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp_Modified)
                                {
                                    Chunks->Subs[Pos]->Subs[Pos2]->Modify(Elements::moov_trak_mdia, Elements::moov_trak_mdia_minf, Elements::moov_trak_mdia_minf_stbl, Elements::moov_trak_mdia_minf_stbl_stsd, Elements::moov_trak_mdia_minf_stbl_stsd_xxxxVideo, Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_pasp);
                                    Modified=true;
                                }
                                if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv_Modified)
                                {                                    Chunks->Subs[Pos]->Subs[Pos2]->Modify(Elements::moov_trak_mdia, Elements::moov_trak_mdia_minf, Elements::moov_trak_mdia_minf_stbl, Elements::moov_trak_mdia_minf_stbl_stsd, Elements::moov_trak_mdia_minf_stbl_stsd_xxxxVideo, Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv);
                                    Modified=true;
                                }
                                if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli_Modified)
                                {
                                    Chunks->Subs[Pos]->Subs[Pos2]->Modify(Elements::moov_trak_mdia, Elements::moov_trak_mdia_minf, Elements::moov_trak_mdia_minf_stbl, Elements::moov_trak_mdia_minf_stbl_stsd, Elements::moov_trak_mdia_minf_stbl_stsd_xxxxVideo, Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_clli);
                                    Modified=true;
                                }
                            }

                            if (Modified)
                            {
                                Chunks->Chunk.Content.IsModified=true;
                                Chunks->Chunk.Content.Size_IsModified=true;
                                Chunks->Subs[Pos]->Chunk.Content.IsModified=true;
                                Chunks->Subs[Pos]->Chunk.Content.Size_IsModified=true;
                                Chunks->Subs[Pos]->Subs[Pos2]->Chunk.Content.IsModified=true;
                                Chunks->Subs[Pos]->Subs[Pos2]->Chunk.Content.Size_IsModified=true;
                            }

                            Found=true;
                            break;
                        }
                        trak_Index++;
                    }
                }
                if (Found)
                    break;
            }
        }
    }

    //Remove timecode tracks and chunks
    if (Chunks->Global->TimeCode_Track_Delete)
    {
        size_t trak_Index=0;
        for (size_t Pos=0; Pos<Chunks->Subs.size(); Pos++)
        {
            if (Chunks->Subs[Pos]->Chunk.Header.Name==Elements::moov)
            {
                for (size_t Pos2=0; Pos2<Chunks->Subs[Pos]->Subs.size(); Pos2++)
                {
                    for (size_t Pos3=0; Pos3<Chunks->Subs[Pos]->Subs[Pos2]->Subs.size(); Pos3++)
                    {
                        if (Chunks->Subs[Pos]->Subs[Pos2]->Subs[Pos3]->Chunk.Header.Name==Elements::moov_trak_tref)
                        {
                            for (size_t Pos4=0; Pos4<Chunks->Subs[Pos]->Subs[Pos2]->Subs[Pos3]->Subs.size(); Pos4++)
                            {
                                if (Chunks->Subs[Pos]->Subs[Pos2]->Subs[Pos3]->Subs[Pos4]->Chunk.Header.Name==Elements::moov_trak_tref_tmcd)
                                {
                                    Chunks->Subs[Pos]->Subs[Pos2]->Modify(Elements::moov_trak_tref, Elements::moov_trak_tref_tmcd);
                                    Chunks->Subs[Pos]->Chunk.Content.IsModified=true;
                                    Chunks->Subs[Pos]->Chunk.Content.Size_IsModified=true;
                                    Chunks->Chunk.Content.IsModified=true;
                                    Chunks->Chunk.Content.Size_IsModified=true;
                                }
                            }
                        }
                    }

                    if (Chunks->Subs[Pos]->Subs[Pos2]->Chunk.Header.Name==Elements::moov_trak)
                    {
                        Chunks->Subs[Pos]->Subs[Pos2]->Modify(Elements::moov_trak_tref, Elements::moov_trak_tref_tmcd);
                        if (trak_Index<Chunks->Global->moov_trak.size() && Chunks->Global->moov_trak[trak_Index]->moov_trak_mdia_minf_stbl_stsd_tmcd_Present)
                        {
                            delete Chunks->Subs[Pos]->Subs[Pos2];
                            Chunks->Subs[Pos]->Subs.erase(Chunks->Subs[Pos]->Subs.begin()+Pos2);
                            Chunks->Subs[Pos]->Chunk.Content.IsModified=true;
                            Chunks->Subs[Pos]->Chunk.Content.Size_IsModified=true;
                            Chunks->Chunk.Content.IsModified=true;
                            Chunks->Chunk.Content.Size_IsModified=true;
                        }
                        trak_Index++;
                    }
                }
            }
        }
    }

    if (Chunks->Global->moov_trak_tkhd_Modified)
        Chunks->Modify(Elements::moov, Elements::moov_trak, Elements::moov_trak_tkhd);
    // Modify mdhd in all audio tracks
    if (Chunks->Global->moov_trak_mdia_mdhd_Modified)
    {
        size_t trak_Index=0;
        for (size_t Pos=0; Pos<Chunks->Subs.size(); Pos++)
        {
            if (Chunks->Subs[Pos]->Chunk.Header.Name==Elements::moov)
            {
                for (size_t Pos2=0; Pos2<Chunks->Subs[Pos]->Subs.size(); Pos2++)
                {
                    if (Chunks->Subs[Pos]->Subs[Pos2]->Chunk.Header.Name==Elements::moov_trak)
                    {
                        if(trak_Index<Chunks->Global->moov_trak.size() && Chunks->Global->moov_trak[trak_Index]->IsSound)
                        {
                            Chunks->Chunk.Content.IsModified=true;
                            Chunks->Chunk.Content.Size_IsModified=true;
                            Chunks->Subs[Pos]->Chunk.Content.IsModified=true;
                            Chunks->Subs[Pos]->Chunk.Content.Size_IsModified=true;
                            Chunks->Subs[Pos]->Subs[Pos2]->Chunk.Content.IsModified=true;
                            Chunks->Subs[Pos]->Subs[Pos2]->Chunk.Content.Size_IsModified=true;
                            Chunks->Subs[Pos]->Subs[Pos2]->Modify(Elements::moov_trak_mdia, Elements::moov_trak_mdia_mdhd);
                        }
                        trak_Index++;
                    }
                }
            }
        }
    }
    // Modify chan in all audio tracks
    if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan_Modified)
    {
        size_t trak_Index=0;
        for (size_t Pos=0; Pos<Chunks->Subs.size(); Pos++)
        {
            if (Chunks->Subs[Pos]->Chunk.Header.Name==Elements::moov)
            {
                for (size_t Pos2=0; Pos2<Chunks->Subs[Pos]->Subs.size(); Pos2++)
                {
                    if (Chunks->Subs[Pos]->Subs[Pos2]->Chunk.Header.Name==Elements::moov_trak)
                    {
                        if(trak_Index<Chunks->Global->moov_trak.size() && Chunks->Global->moov_trak[trak_Index]->IsSound)
                        {
                            Chunks->Chunk.Content.IsModified=true;
                            Chunks->Chunk.Content.Size_IsModified=true;
                            Chunks->Subs[Pos]->Chunk.Content.IsModified=true;
                            Chunks->Subs[Pos]->Chunk.Content.Size_IsModified=true;
                            Chunks->Subs[Pos]->Subs[Pos2]->Chunk.Content.IsModified=true;
                            Chunks->Subs[Pos]->Subs[Pos2]->Chunk.Content.Size_IsModified=true;
                            Chunks->Subs[Pos]->Subs[Pos2]->Modify(Elements::moov_trak_mdia, Elements::moov_trak_mdia_minf,
                                                                  Elements::moov_trak_mdia_minf_stbl,
                                                                  Elements::moov_trak_mdia_minf_stbl_stsd,
                                                                  Elements::moov_trak_mdia_minf_stbl_stsd_xxxxSound,
                                                                  Elements::moov_trak_mdia_minf_stbl_stsd_xxxx_chan);
                        }
                        trak_Index++;
                    }
                }
            }
        }
    }
    //Opening files
    if (!Chunks->Global->In.Open(Chunks->Global->File_Name))
    {
        Errors<<Chunks->Global->File_Name.To_Local()<<": File does not exist anymore"<<endl;
        PerFile_Error<<"File does not exist anymore";
        return false;
    }

    //Old temporary file
    #if MASTORE
    if ((Chunks->Global->Temp_Path.size() && (Chunks->Global->Temp_Name.size() && File::Exists(Chunks->Global->Temp_Path+Chunks->Global->Temp_Name) && !File::Delete(Chunks->Global->Temp_Path+Chunks->Global->Temp_Name))
    #else
    if (File::Exists(Chunks->Global->File_Name+__T(".tmp")) && !File::Delete(Chunks->Global->File_Name+__T(".tmp")))
    #endif
    {
        Errors<<Chunks->Global->File_Name.To_Local()<<": Old temporary file can't be deleted"<<endl;
        PerFile_Error<<"Old temporary file can't be deleted";
        return false;
    }

    #ifdef MACSTORE
    if (Chunks->Global->Temp_Name.size())
        Chunks->Global->Temp_Name=__T("");

    if (Chunks->Global->Temp_Path.size())
    {
        if (Dir::Exists(Chunks->Global->Temp_Path))
            deleteTemporaryDirectory(Chunks->Global->Temp_Path.To_UTF8().c_str());

        Chunks->Global->Temp_Path=__T("");
    }
    #endif
    //Parsing
    try
    {
        Chunks->Write();
    }
    catch (exception_canceled &)
    {
        Chunks->Global->Out.Close();
        #ifdef MACSTORE
        if (Chunks->Global->Temp_Path.size() && Chunks->Global->Temp_Name.size())
            File::Delete(Chunks->Global->Temp_Path+Chunks->Global->Temp_Name);

        if (Chunks->Global->Temp_Name.size())
            Chunks->Global->Temp_Name=__T("");

        if (Chunks->Global->Temp_Path.size())
        {
            if (Dir::Exists(Chunks->Global->Temp_Path))
                deleteTemporaryDirectory(Chunks->Global->Temp_Path.To_UTF8().c_str());

            Chunks->Global->Temp_Path=__T("");
        }
        #else
        File::Delete(Chunks->Global->File_Name+__T(".tmp"));
        #endif
        CriticalSectionLocker(Chunks->Global->CS);
        File_IsCanceled=true;
        Chunks->Global->Canceling=false;
        return false;
    }
    catch (exception &e)
    {
        Errors<<Chunks->Global->File_Name.To_Local() <<": "<<e.what()<<endl;
        PerFile_Error << e.what();
        return false;
    }

    //Log
    Information<<(Chunks?Chunks->Global->File_Name.To_Local() :"")<<": Is modified"<<endl;

    CriticalSectionLocker(Chunks->Global->CS);
    Chunks->Global->Progress=1;

    return true;
}

//***************************************************************************
// Per Item
//***************************************************************************

//---------------------------------------------------------------------------
string mp4_Handler::Get(const string &Field)
{
    if (Field=="com.universaladid.idregistry" || Field == "com.universaladid.idvalue")
    {
        if (!Chunks || !Chunks->Global || !Chunks->Global->moov_meta_ilst)
            return string();

        // Searching key order
        size_t i = 0;
        while (i < Chunks->Global->moov_meta_keys->Keys.size())
        {
            mp4_Base::global::block_moov_meta_key& Key = Chunks->Global->moov_meta_keys->Keys[i];
            if (Key.namespace_==0x6D647461 && string((const char*)Key.value, Key.size-8)==Field)
                break;
            i++;
        }
        if (i >= Chunks->Global->moov_meta_keys->Keys.size())
            return string();
        
        // Searching value
        i++; //1-based
        size_t j = 0;
        while (j < Chunks->Global->moov_meta_ilst->Items.size())
        {
            mp4_Base::global::block_moov_meta_list& Item = Chunks->Global->moov_meta_ilst->Items[j];
            if (Item.name == i)
            {
                if (Item.data_Size)
                    return string((const char*)Item.value+Item.data_Pos+16, Item.data_Size-16);
            }
            j++;
        }
        return string();
    }
    else if (Field=="clef")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak_tapt_clef.count(Chunks->Global->moov_trak_FirstVideoIndex))
            return string();

        stringstream ss; ss << Chunks->Global->moov_trak_tapt_clef[Chunks->Global->moov_trak_FirstVideoIndex]->Width << "x"
                            << Chunks->Global->moov_trak_tapt_clef[Chunks->Global->moov_trak_FirstVideoIndex]->Height;
        return ss.str();
    }
    else if (Field=="prof")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak_tapt_prof.count(Chunks->Global->moov_trak_FirstVideoIndex))
            return string();

        stringstream ss; ss << Chunks->Global->moov_trak_tapt_prof[Chunks->Global->moov_trak_FirstVideoIndex]->Width << "x"
                            << Chunks->Global->moov_trak_tapt_prof[Chunks->Global->moov_trak_FirstVideoIndex]->Height;
        return ss.str();
    }
    else if (Field=="enof")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak_tapt_enof.count(Chunks->Global->moov_trak_FirstVideoIndex))
            return string();

        stringstream ss; ss << Chunks->Global->moov_trak_tapt_enof[Chunks->Global->moov_trak_FirstVideoIndex]->Width << "x"
                            << Chunks->Global->moov_trak_tapt_enof[Chunks->Global->moov_trak_FirstVideoIndex]->Height;
        return ss.str();
    }
    else if (Field=="stsd_xxxxvideo_version")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo.count(Chunks->Global->moov_trak_FirstVideoIndex))
            return string();

        stringstream ss; ss << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunks->Global->moov_trak_FirstVideoIndex]->Version;
        return ss.str();
    }
    else if (Field=="temporal_quality")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo.count(Chunks->Global->moov_trak_FirstVideoIndex))
            return string();

        stringstream ss; ss << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunks->Global->moov_trak_FirstVideoIndex]->TemporalQuality;
        return ss.str();
    }
    else if (Field=="fiel")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel.count(Chunks->Global->moov_trak_FirstVideoIndex))
            return string();

        stringstream ss; ss << (uint16_t)Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel[Chunks->Global->moov_trak_FirstVideoIndex]->Fields << ","
                            << (uint16_t)Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel[Chunks->Global->moov_trak_FirstVideoIndex]->Detail;
        return ss.str();
    }
    else if (Field=="colr")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr.count(Chunks->Global->moov_trak_FirstVideoIndex))
            return string();

        stringstream ss; ss << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries << ","
                            << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr[Chunks->Global->moov_trak_FirstVideoIndex]->Transfer << ","
                            << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr[Chunks->Global->moov_trak_FirstVideoIndex]->Matrix;
        return ss.str();
    }
    else if (Field=="gama")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama.count(Chunks->Global->moov_trak_FirstVideoIndex))
            return string();

        stringstream ss; ss << setprecision(3) << fixed << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama[Chunks->Global->moov_trak_FirstVideoIndex]->Gamma;
        return ss.str();
    }
    else if (Field=="pasp")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp.count(Chunks->Global->moov_trak_FirstVideoIndex))
            return string();

        stringstream ss; ss << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp[Chunks->Global->moov_trak_FirstVideoIndex]->hSpacing << ":"
                            << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp[Chunks->Global->moov_trak_FirstVideoIndex]->vSpacing;
        return ss.str();
    }
    else if (Field=="display_primaries")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv.count(Chunks->Global->moov_trak_FirstVideoIndex))
            return string();

        stringstream ss;
        for (size_t c = 0; c < Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries.size(); c++)
            ss << (c ? "," : "") << setprecision(3) << fixed << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[c];

        return ss.str();
    }
    else if (Field=="luminance")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv.count(Chunks->Global->moov_trak_FirstVideoIndex))
            return string();

        stringstream ss;
        ss << setprecision(4) << fixed << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Luminance[0] << ","
           << setprecision(4) << fixed << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Luminance[1];

        return ss.str();
    }
    else if (Field=="maximum_content_light_level")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli.count(Chunks->Global->moov_trak_FirstVideoIndex))
            return string();

        stringstream ss;
        ss << setprecision(4) << fixed << round(Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli[Chunks->Global->moov_trak_FirstVideoIndex]->maximum_content_light_level);

        return ss.str();
    }
    else if (Field=="maximum_frame_average_light_level")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli.count(Chunks->Global->moov_trak_FirstVideoIndex))
            return string();

        stringstream ss;
        ss << setprecision(4) << fixed << round(Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli[Chunks->Global->moov_trak_FirstVideoIndex]->maximum_frame_average_light_level);

        return ss.str();
    }
    else if (Field=="tmcd")
    {
        if (Chunks->Global->TimeCode_Track_Present && !Chunks->Global->TimeCode_Track_Delete)
            return "Present";

        return string();
    }
    else if (Field=="lang")
    {
        stringstream ss;
        size_t Index=0;

        for (size_t Pos=0; Pos<Chunks->Global->moov_trak.size(); Pos++)
        {
            if (!Chunks->Global->moov_trak[Pos]->IsSound)
                continue;

            if (!ss.str().empty())
                ss << ",";

            ss << Index << "=";

            map<size_t, mp4_Base::global::block_moov_trak_mdia_mdhd*>::iterator It=Chunks->Global->moov_trak_mdia_mdhd.find(Pos);
            if (It!=Chunks->Global->moov_trak_mdia_mdhd.end())
               ss << It->second->Language;
            else
                ss << "ABSENT";
            Index++;
        }
        return ss.str();
    }
    else if (Field=="chan")
    {
        json Json;

        size_t Index=0;
        for (size_t Pos=0; Pos<Chunks->Global->moov_trak.size(); Pos++)
        {
            if (!Chunks->Global->moov_trak[Pos]->IsSound)
                continue;

            string Key=to_string(Index++);

            auto It=Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.find(Pos);
            if (It==Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.end())
              continue;

            Json[Key]=json::object();
            Json[Key]["layout"]=mp4_chan_ChannelLayout(It->second->ChannelLayoutTag);

            string Descriptions;
            for (auto& Description : It->second->Descriptions)
            {
                if (!Descriptions.empty())
                    Descriptions+="+";

                Descriptions+=mp4_chan_ChannelDescription(Description.ChannelLabel);
            }

            if (!Descriptions.empty())
                Json[Key]["descriptions"]=Descriptions;
        }

        return Json.dump();
    }
    else if (Field=="clap")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap.count(Chunks->Global->moov_trak_FirstVideoIndex))
            return string();

        stringstream ss;
        ss << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Aperture_Width_Num;
        if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Aperture_Width_Den!=1)
            ss << ":" << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Aperture_Width_Den;
        ss << "," << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Aperture_Height_Num;
        if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Aperture_Height_Den!=1)
            ss << ":" << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Aperture_Height_Den;
        ss << "," << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Horizontal_Offset_Num;
        if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Horizontal_Offset_Den!=1)
            ss << ":" << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Horizontal_Offset_Den;
        ss << "," << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Vertical_Offset_Num;
        if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Vertical_Offset_Den!=1)
            ss << ":" << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Vertical_Offset_Den;

        return ss.str();
    }
    else if (Field=="wscale")
    {
        if (!Chunks->Global->moov_trak_tkhd)
            return string();

        stringstream ss; ss << setprecision(3) << fixed << Chunks->Global->moov_trak_tkhd->Width_Scale;
        return ss.str();
    }

    mp4_Base::global::block_strings** Chunk_Strings=block_strings_Get(Field);
    if (!Chunk_Strings || !*Chunk_Strings)
        return string();
    
    return Get(Field_Get(Field), *Chunk_Strings);
}

//---------------------------------------------------------------------------
bool mp4_Handler::Set(const string &Field, const string &Value, bool Simulate)
{
    //Integrity
    if (Chunks==NULL)
    {
        Errors<<"(No file name): Internal error"<<endl;
        PerFile_Error << "Internal error";
        return false;
    }

    //Setting it
    if (Field=="com.universaladid.idregistry" || Field == "com.universaladid.idvalue")
    {
            if (Simulate)
                return true;

        if (!Chunks || !Chunks->Global || !Chunks->Global->moov_meta_ilst)
        {
            Chunks->Global->moov_meta_keys_NewKeys.push_back(Field);
            Chunks->Global->moov_meta_ilst_NewValues.push_back(Value);
            Chunks->Global->moov_meta_hdlr_Modified=true;
            Chunks->Global->moov_meta_keys_Modified=true;
            Chunks->Global->moov_meta_ilst_Modified=true;
            return true;
        }

        // Searching key order
        size_t i = 0;
        while (i < Chunks->Global->moov_meta_keys->Keys.size())
        {
            mp4_Base::global::block_moov_meta_key& Key = Chunks->Global->moov_meta_keys->Keys[i];
            if (Key.namespace_==0x6D647461 && string((const char*)Key.value, Key.size-8)==Field)
                break;
            i++;
        }
        if (i >= Chunks->Global->moov_meta_keys->Keys.size())
        {
            Chunks->Global->moov_meta_keys_NewKeys.push_back(Field);
            Chunks->Global->moov_meta_ilst_NewValues.push_back(Value);
            Chunks->Global->moov_meta_hdlr_Modified=true;
            Chunks->Global->moov_meta_keys_Modified=true;
            Chunks->Global->moov_meta_ilst_Modified=true;
            return true;
        }
        
        // Searching value
        i++; //1-based
        size_t j = 0;
        while (j < Chunks->Global->moov_meta_ilst->Items.size())
        {
            mp4_Base::global::block_moov_meta_list& Item = Chunks->Global->moov_meta_ilst->Items[j];
            if (Item.name == i)
            {
                if (Item.data_Size)
                {
                    Item.ToBeReplacedBy=Value;
                    Item.ToBeReplacedBy_Modified=true;
                    Chunks->Global->moov_meta_hdlr_Modified=true;
                    Chunks->Global->moov_meta_keys_Modified=true;
                    Chunks->Global->moov_meta_ilst_Modified=true;
                    return true;
                }
            }
            j++;
        }

        Chunks->Global->moov_meta_keys_NewKeys.push_back(Field);
        Chunks->Global->moov_meta_ilst_NewValues.push_back(Value);
        Chunks->Global->moov_meta_hdlr_Modified=true;
        Chunks->Global->moov_meta_keys_Modified=true;
        Chunks->Global->moov_meta_ilst_Modified=true;
        return true;
    }
    else if (Field=="clef" || Field=="prof" || Field=="enof")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1)
            return false;

        double Width=0, Height=0;
        if (sscanf(Value.c_str(), "%lfx%lf", &Width, &Height)!=2)
            return false;

        if (!Simulate && Field=="clef")
        {
            if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1)
                return false;

            if (!Chunks->Global->moov_trak_tapt_clef.count(Chunks->Global->moov_trak_FirstVideoIndex))
                Chunks->Global->moov_trak_tapt_clef[Chunks->Global->moov_trak_FirstVideoIndex]=new mp4_Base::global::block_moov_trak_tapt_xxxx();

            Chunks->Global->moov_trak_tapt_clef[Chunks->Global->moov_trak_FirstVideoIndex]->Width=Width;
            Chunks->Global->moov_trak_tapt_clef[Chunks->Global->moov_trak_FirstVideoIndex]->Height=Height;
            Chunks->Global->moov_trak_tapt_clef_Modified=true;
        }
        else if (!Simulate && Field=="prof")
        {
            if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1)
                return false;

            if (!Chunks->Global->moov_trak_tapt_prof.count(Chunks->Global->moov_trak_FirstVideoIndex))
                Chunks->Global->moov_trak_tapt_prof[Chunks->Global->moov_trak_FirstVideoIndex]=new mp4_Base::global::block_moov_trak_tapt_xxxx();

            Chunks->Global->moov_trak_tapt_prof[Chunks->Global->moov_trak_FirstVideoIndex]->Width=Width;
            Chunks->Global->moov_trak_tapt_prof[Chunks->Global->moov_trak_FirstVideoIndex]->Height=Height;
            Chunks->Global->moov_trak_tapt_prof_Modified=true;
        }
        else if (!Simulate && Field=="enof")
        {
            if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1)
                return false;

            if (!Chunks->Global->moov_trak_tapt_enof.count(Chunks->Global->moov_trak_FirstVideoIndex))
                Chunks->Global->moov_trak_tapt_enof[Chunks->Global->moov_trak_FirstVideoIndex]=new mp4_Base::global::block_moov_trak_tapt_xxxx();

            Chunks->Global->moov_trak_tapt_enof[Chunks->Global->moov_trak_FirstVideoIndex]->Width=Width;
            Chunks->Global->moov_trak_tapt_enof[Chunks->Global->moov_trak_FirstVideoIndex]->Height=Height;
            Chunks->Global->moov_trak_tapt_enof_Modified=true;
        }
        return true;
    }

    else if (Field=="stsd_xxxxvideo_version")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak[Chunks->Global->moov_trak_FirstVideoIndex]->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present)
            return false;

        uint16_t Version=0;
        if (sscanf(Value.c_str(), "%hu", &Version)!=1 || Version>0x3)
            return false;

        if (!Simulate)
        {
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo.count(Chunks->Global->moov_trak_FirstVideoIndex))
                return false;

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunks->Global->moov_trak_FirstVideoIndex]->Version=Version;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Modified=true;
        }
        return true;
    }

    else if (Field=="temporal_quality")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak[Chunks->Global->moov_trak_FirstVideoIndex]->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present)
            return false;

        uint32_t Quality=0;
        if (sscanf(Value.c_str(), "%u", &Quality)!=1)
            return false;

        if (!Simulate)
        {
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo.count(Chunks->Global->moov_trak_FirstVideoIndex))
                return false;

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Chunks->Global->moov_trak_FirstVideoIndex]->TemporalQuality=Quality;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Modified=true;
        }
        return true;
    }

    else if (Field=="fiel")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak[Chunks->Global->moov_trak_FirstVideoIndex]->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present)
            return false;

        uint16_t Fields=0, Detail=0;
        if (sscanf(Value.c_str(), "%hu,%hu", &Fields, &Detail)!=2 || Fields>0xFF || Detail>0xFF)
            return false;

        if (!Simulate)
        {
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel.count(Chunks->Global->moov_trak_FirstVideoIndex))
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel[Chunks->Global->moov_trak_FirstVideoIndex]=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_fiel();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel[Chunks->Global->moov_trak_FirstVideoIndex]->Fields=Fields;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel[Chunks->Global->moov_trak_FirstVideoIndex]->Detail=Detail;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel_Modified=true;
        }
        return true;
    }
    else if (Field=="colr")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak[Chunks->Global->moov_trak_FirstVideoIndex]->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present)
            return false;

        uint16_t Primaries=0, Transfer=0, Matrix=0;
        if (sscanf(Value.c_str(), "%hu,%hu,%hu", &Primaries, &Transfer, &Matrix)!=3)
        if (!Primaries || !Transfer || !Matrix)
            return false;

        if (!Simulate)
        {
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr.count(Chunks->Global->moov_trak_FirstVideoIndex))
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr[Chunks->Global->moov_trak_FirstVideoIndex]=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_colr();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries=Primaries;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr[Chunks->Global->moov_trak_FirstVideoIndex]->Transfer=Transfer;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr[Chunks->Global->moov_trak_FirstVideoIndex]->Matrix=Matrix;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr_Modified=true;
        }
        return true;
    }
    else if (Field=="gama")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak[Chunks->Global->moov_trak_FirstVideoIndex]->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present)
            return false;

        double Gamma=0;
        if (sscanf(Value.c_str(), "%lf", &Gamma)!=1)
            return false;

        if (!Simulate)
        {
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama.count(Chunks->Global->moov_trak_FirstVideoIndex))
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama[Chunks->Global->moov_trak_FirstVideoIndex]=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_gama();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama[Chunks->Global->moov_trak_FirstVideoIndex]->Gamma=Gamma;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama_Modified=true;
        }
        return true;
    }
    else if (Field=="pasp")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak[Chunks->Global->moov_trak_FirstVideoIndex]->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present)
            return false;

        uint32_t hSpacing=0, vSpacing=0;
        if (sscanf(Value.c_str(), "%u:%u", &hSpacing, &vSpacing)!=2)
            return false;

        if (!Simulate)
        {
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp.count(Chunks->Global->moov_trak_FirstVideoIndex))
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp[Chunks->Global->moov_trak_FirstVideoIndex]=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_pasp();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp[Chunks->Global->moov_trak_FirstVideoIndex]->hSpacing=hSpacing;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp[Chunks->Global->moov_trak_FirstVideoIndex]->vSpacing=vSpacing;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp_Modified=true;
        }
        return true;
    }
    else if (Field=="display_primaries")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak[Chunks->Global->moov_trak_FirstVideoIndex]->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present)
            return false;

        double x1=0,y1=0,x2=0,y2=0,x3=0,y3=0, wp_x=0, wp_y=0;
        if (sscanf(Value.c_str(), "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", &x1, &y1, &x2, &y2, &x3, &y3, &wp_x, &wp_y)!=8)
            return false;

        if (!Simulate)
        {
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv.count(Chunks->Global->moov_trak_FirstVideoIndex))
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[0]=x1;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[1]=y1;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[2]=x2;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[3]=y2;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[4]=x3;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[5]=y3;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[6]=wp_x;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[7]=wp_y;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv_Modified=true;
        }
        return true;
    }
    else if (Field=="luminance")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak[Chunks->Global->moov_trak_FirstVideoIndex]->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present)
            return false;

        double max=0,min=0;
        if (sscanf(Value.c_str(), "%lf,%lf", &max, &min)!=2)
            return false;

        if (max < min)
        {
             double temp(max); max=min; min=temp;
        }

        if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv.count(Chunks->Global->moov_trak_FirstVideoIndex)) // Check if primaries values are present
            return false;

        if (!Simulate)
        {
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv.count(Chunks->Global->moov_trak_FirstVideoIndex))
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Luminance[0]=max;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Luminance[1]=min;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv_Modified=true;
        }
        return true;
    }
    else if (Field=="maximum_content_light_level")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak[Chunks->Global->moov_trak_FirstVideoIndex]->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present)
            return false;

        double max=0;
        if (sscanf(Value.c_str(), "%lf", &max)!=1)
            return false;

        if (!Simulate)
        {
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli.count(Chunks->Global->moov_trak_FirstVideoIndex))
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli[Chunks->Global->moov_trak_FirstVideoIndex]=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_clli();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli[Chunks->Global->moov_trak_FirstVideoIndex]->maximum_content_light_level=max;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli_Modified=true;
        }
        return true;
    }
    else if (Field=="maximum_frame_average_light_level")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak[Chunks->Global->moov_trak_FirstVideoIndex]->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present)
            return false;

        double max=0;
        if (sscanf(Value.c_str(), "%lf", &max)!=1)
            return false;

        if (!Simulate)
        {
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli.count(Chunks->Global->moov_trak_FirstVideoIndex))
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli[Chunks->Global->moov_trak_FirstVideoIndex]=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_clli();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli[Chunks->Global->moov_trak_FirstVideoIndex]->maximum_frame_average_light_level=max;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli_Modified=true;
        }
        return true;
    }
    else if (Field=="hdr_data_from_xml")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak[Chunks->Global->moov_trak_FirstVideoIndex]->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present)
            return false;

        string tmp;
        bool mdcv=false, clli=false;
        double x1=0, y1=0, x2=0, y2=0, x3=0, y3=0, wp_x=0, wp_y=0, luminance_min=0, luminance_max=0, max_cll=-1, max_fall=-1;

        tfsxml_string tfsxml_priv, tfsxml_priv_backup, result;
        tfsxml_init(&tfsxml_priv, (const void*)Value.c_str(), Value.size());
        if (tfsxml_last_named(&tfsxml_priv, &result, "DolbyLabsMDF") || tfsxml_enter(&tfsxml_priv) ||
            tfsxml_last_named(&tfsxml_priv, &result, "Outputs") || tfsxml_enter(&tfsxml_priv) ||
            tfsxml_last_named(&tfsxml_priv, &result, "Output") || tfsxml_enter(&tfsxml_priv) ||
            tfsxml_last_named(&tfsxml_priv, &result, "Video") || tfsxml_enter(&tfsxml_priv) ||
            tfsxml_last_named(&tfsxml_priv, &result, "Track") || tfsxml_enter(&tfsxml_priv))
            return false;

        tfsxml_priv_backup = tfsxml_priv;

        if (!tfsxml_last_named(&tfsxml_priv, &result, "Level6") && !tfsxml_enter(&tfsxml_priv))
        {
            if (!tfsxml_last_named(&tfsxml_priv, &result, "MaxCLL"))
            {
                if (tfsxml_value(&tfsxml_priv, &result))
                    return false;

                tmp = string(result.buf, result.len);
                if (sscanf(tmp.c_str(), "%lf", &max_cll)!=1)
                    return false;
            }
    
            if (!tfsxml_last_named(&tfsxml_priv, &result, "MaxFALL"))
            {
                if (tfsxml_value(&tfsxml_priv, &result))
                    return false;

                tmp = string(result.buf, result.len);
                if (sscanf(tmp.c_str(), "%lf", &max_fall)!=1)
                    return false;
            }

            clli=true;
        }
    
        tfsxml_priv = tfsxml_priv_backup;

        if (tfsxml_last_named(&tfsxml_priv, &result, "PluginNode") || tfsxml_enter(&tfsxml_priv) ||
            tfsxml_last_named(&tfsxml_priv, &result, "DVGlobalData") || tfsxml_enter(&tfsxml_priv))
            return false;

        bool display_found = false;
        while (!tfsxml_next_named(&tfsxml_priv, &result, "MasteringDisplay"))
        {
            if (HdrDataFromXmlId!=(size_t)-1)
            {
                tfsxml_string tfsxml_priv_local = tfsxml_priv, result_local = result;
                if (tfsxml_enter(&tfsxml_priv_local) ||
                    tfsxml_last_named(&tfsxml_priv_local, &result_local, "ID") ||
                    tfsxml_value(&tfsxml_priv_local, &result_local))
                    continue;

                tmp = string(result_local.buf, result_local.len);
                size_t id = 0;
                if (sscanf(tmp.c_str(), "%lu", &id)!=1)
                    continue;

                if (id == HdrDataFromXmlId)
                {
                    display_found = true;
                    break;
                }
            }
            else
            {
                tfsxml_string tfsxml_priv_local = tfsxml_priv, result_local = result;
                if (tfsxml_next_named(&tfsxml_priv_local, &result_local, "MasteringDisplay"))
                {
                    display_found = true;
                    break;
                }
            }
        }

        if (!display_found)
            return false;

        if (tfsxml_enter(&tfsxml_priv))
            return false;

        tfsxml_priv_backup = tfsxml_priv;
        if (tfsxml_last_named(&tfsxml_priv, &result, "Primaries") || tfsxml_enter(&tfsxml_priv))
            return false;

        if (tfsxml_last_named(&tfsxml_priv, &result, "Red"))
            return false;

        if (tfsxml_value(&tfsxml_priv, &result))
            return false;

        tmp = string(result.buf, result.len);
        if (sscanf(tmp.c_str(), "%lf %lf", &x1, &y1)!=2)
            return false;

        if (tfsxml_last_named(&tfsxml_priv, &result, "Green"))
            return false;

        if (tfsxml_value(&tfsxml_priv, &result))
            return false;

        tmp = string(result.buf, result.len);
        if (sscanf(tmp.c_str(), "%lf %lf", &x2, &y2)!=2)
            return false;

        if (tfsxml_last_named(&tfsxml_priv, &result, "Blue"))
            return false;

        if (tfsxml_value(&tfsxml_priv, &result))
            return false;

        tmp = string(result.buf, result.len);
        if (sscanf(tmp.c_str(), "%lf %lf", &x3, &y3)!=2)
            return false;

        tfsxml_priv = tfsxml_priv_backup;
        if (tfsxml_last_named(&tfsxml_priv, &result, "WhitePoint"))
            return false;

        if (tfsxml_value(&tfsxml_priv, &result))
            return false;

        tmp = string(result.buf, result.len);
        if (sscanf(tmp.c_str(), "%lf %lf", &wp_x, &wp_y)!=2)
            return false;

        tfsxml_priv = tfsxml_priv_backup;
        if (tfsxml_last_named(&tfsxml_priv, &result, "PeakBrightness"))
            return false;

        if (tfsxml_value(&tfsxml_priv, &result))
            return false;

        tmp = string(result.buf, result.len);
        if (sscanf(tmp.c_str(), "%lf", &luminance_max)!=1)
            return false;

        tfsxml_priv = tfsxml_priv_backup;
        if (tfsxml_last_named(&tfsxml_priv, &result, "MinimumBrightness"))
            return false;

        if (tfsxml_value(&tfsxml_priv, &result))
            return false;

        tmp = string(result.buf, result.len);
        if (sscanf(tmp.c_str(), "%lf", &luminance_min)!=1)
            return false;

        mdcv=true;

        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1)
            return false;

        if (!Simulate)
        {
            if (mdcv)
            {
                if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv.count(Chunks->Global->moov_trak_FirstVideoIndex))
                    Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv();

                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[4]=x1;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[5]=y1;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[0]=x2;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[1]=y2;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[2]=x3;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[3]=y3;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[6]=wp_x;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Primaries[7]=wp_y;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Luminance[0]=luminance_max;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex]->Luminance[1]=luminance_min;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv_Modified=true;
            }

            if (clli)
            {
                if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli.count(Chunks->Global->moov_trak_FirstVideoIndex))
                    Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli[Chunks->Global->moov_trak_FirstVideoIndex]=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_clli();

                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli[Chunks->Global->moov_trak_FirstVideoIndex]->maximum_content_light_level=max_cll;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli[Chunks->Global->moov_trak_FirstVideoIndex]->maximum_frame_average_light_level=max_fall;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli_Modified=true;
            }
        }

        return true;
    }
    else if (Field=="clap")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex==(size_t)-1 || !Chunks->Global->moov_trak[Chunks->Global->moov_trak_FirstVideoIndex]->moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present)
            return false;

        uint32_t Aperture_Width_Num=(uint32_t)-1, Aperture_Width_Den=1, Aperture_Height_Num=(uint32_t)-1, Aperture_Height_Den=1;
        uint32_t Horizontal_Offset_Num=(uint32_t)-1, Horizontal_Offset_Den=1, Vertical_Offset_Num=(uint32_t)-1, Vertical_Offset_Den=1;

        istringstream iss(Value);
        iss >> Aperture_Width_Num;
        if (!iss.good())
            return false;
        if (iss.peek()==':')
        {
            iss.ignore();
            iss >> Aperture_Width_Den;
            if (!iss.good())
                return false;
        }
        if (iss.peek()!=',')
            return false;
        iss.ignore();
        iss >> Aperture_Height_Num;
        if (!iss.good())
            return false;
        if (iss.peek()==':')
        {
            iss.ignore();
            iss >> Aperture_Height_Den;
            if (!iss.good())
                return false;
        }
        if (iss.peek()!=',')
            return false;
        iss.ignore();
        iss >> Horizontal_Offset_Num;
        if (!iss.good())
            return false;
        if (iss.peek()==':')
        {
            iss.ignore();
            iss >> Horizontal_Offset_Den;
            if (!iss.good())
                return false;
        }
        if (iss.peek()!=',')
            return false;
        iss.ignore();
        iss >> Vertical_Offset_Num;
        if (iss.fail())
            return false;
        if (iss.peek()==':')
        {
            iss.ignore();
            iss >> Vertical_Offset_Den;
            if (iss.fail())
                return false;
        }

        if (!Simulate)
        {
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap.count(Chunks->Global->moov_trak_FirstVideoIndex))
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_clap();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Aperture_Width_Num=Aperture_Width_Num;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Aperture_Width_Den=Aperture_Width_Den;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Aperture_Height_Num=Aperture_Height_Num;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Aperture_Height_Den=Aperture_Height_Den;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Horizontal_Offset_Num=Horizontal_Offset_Num;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Horizontal_Offset_Den=Horizontal_Offset_Den;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Vertical_Offset_Num=Vertical_Offset_Num;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex]->Vertical_Offset_Den=Vertical_Offset_Den;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap_Modified=true;
        }
        return true;
    }

    else if (Field=="lang")
    {
        bool Ok=true;

        size_t Start=0, End=0;
        do
        {
            End=Value.find(',', Start);
            string Current=Value.substr(Start, End!=string::npos?End-Start:string::npos);

            size_t Equal_Pos=Current.find('=');
            if (!Equal_Pos || Equal_Pos==string::npos || Equal_Pos+1==Current.size())
            {
                Ok=false;
                break;
            }

            uint32_t Track;
            uint16_t Code;
            bool Delete=false;
            bool Ignore=false;

            istringstream iss(Current.substr(0, Equal_Pos));
            iss >> Track;
            if (iss.fail() || !iss.eof())
            {
                Ok=false;
                break;
            }

            if (!mp4_mdhd_LanguageCode(Current.substr(Equal_Pos+1), Code, Ignore, Delete))
            {
                Ok=false;
                break;
            }

            size_t Index=0;
            size_t trak_Index=0;
            bool Found=false;
            for (size_t Pos=0; Pos<Chunks->Global->moov_trak.size(); Pos++)
            {
                if (!Chunks->Global->moov_trak[Pos]->IsSound)
                    continue;

                if (Index++==Track)
                {
                    trak_Index=Pos;
                    Found=true;
                    break;
                }
            }

            if (!Found)
            {
                Ok=false;
                continue;
            }

            if (!Chunks->Global->moov_trak[trak_Index]->IsSound)
            {
                Ok=false;
                continue;
            }

            if (!Simulate && !Ignore)
            {
                if (Chunks->Global->moov_trak_mdia_mdhd.find(trak_Index)!=Chunks->Global->moov_trak_mdia_mdhd.end())
                    Chunks->Global->moov_trak_mdia_mdhd[trak_Index]->Language=Code;
                else
                    Ok=false;
            }
        }
        while((Start=Value.find_first_not_of(',', End))!=string::npos);

        if (!Simulate && Ok)
            Chunks->Global->moov_trak_mdia_mdhd_Modified=true;

        return Ok;
    }

    else if (Field=="chan")
    {
        bool Ok=true;

        json Json = json::parse(Value, nullptr, false);
        if (Json.is_discarded() || !Json.is_object())
        {
            Ok=false;
            return Ok;
        }

        for (auto It : Json.items())
        {
            uint32_t Track;
            uint32_t Layout=0;
            vector<uint32_t> Codes;
            bool Ignore=false;
            bool Delete=false;

            istringstream iss(It.key());
            iss >> Track;
            if (iss.fail() || !iss.eof())
            {
                Ok=false;
                break;
            }

            if (It.value().find("descriptions")!=It.value().end())
            {
                if (!It.value()["descriptions"].is_string())
                {
                    Ok=false;
                    break;
                }

                if (!mp4_chan_ChannelCodes(It.value()["descriptions"], Codes, Ignore, Delete))
                {
                    Ok=false;
                    break;
                }
            }

            if (It.value().find("layout")!=It.value().end())
            {
                if (!It.value()["layout"].is_string())
                {
                    Ok=false;
                    break;
                }

                if (!mp4_chan_ChannelLayoutCode(It.value()["layout"], Layout))
                {
                    Ok=false;
                    break;
                }
            }

            size_t Index=0;
            size_t trak_Index=0;
            bool Found=false;
            for (size_t Pos=0; Pos<Chunks->Global->moov_trak.size(); Pos++)
            {
                if (!Chunks->Global->moov_trak[Pos]->IsSound)
                    continue;

                if (Index++==Track)
                {
                    trak_Index=Pos;
                    Found=true;
                    break;
                }
            }

            if (!Found)
            {
                Ok=false;
                continue;
            }

            if (!Chunks->Global->moov_trak[trak_Index]->IsSound)
            {
                Ok=false;
                continue;
            }

            if (!Simulate && !Ignore)
            {
                if (Delete)
                {
                    if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.find(trak_Index)!=Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.end())
                    {
                        delete Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[trak_Index];
                        Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.erase(trak_Index);
                    }

                    continue;
                }

                if (It.value().find("layout")!=It.value().end())
                {
                    if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.find(trak_Index)==Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.end())
                        Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[trak_Index]=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_chan;

                    Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[trak_Index]->ChannelLayoutTag=Layout;
                }

                if (Layout==0 && !Codes.empty())
                {
                    if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.find(trak_Index)==Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.end())
                        Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[trak_Index]=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_chan;

                    Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[trak_Index]->NumberChannelDescriptions=Codes.size();
                    Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[trak_Index]->Descriptions.resize(Codes.size());
                    for (size_t Pos=0; Pos<Codes.size(); Pos++)
                        Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[trak_Index]->Descriptions[Pos].ChannelLabel=Codes[Pos];
                }
                else if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.find(trak_Index)!=Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.end())
                    Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[trak_Index]->NumberChannelDescriptions=0;
            }
        }

        if (!Simulate && Ok)
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan_Modified=true;

        return Ok;
    }


    else if (Field=="wscale")
    {
        if (!Chunks->Global->moov_trak_tkhd)
            return false; // TODO: Error mesg

        double wScale=0;
        if (sscanf(Value.c_str(), "%lf", &wScale)!=1)
            return false;

        if (!Simulate)
        {
            Chunks->Global->moov_trak_tkhd->Width_Scale=wScale;
            Chunks->Global->moov_trak_tkhd_Modified=true;
            return true;
        }
    }

    return false;
}

//---------------------------------------------------------------------------
bool mp4_Handler::Remove(const string &Field)
{
    //Integrity
    if (Chunks==NULL)
    {
        Errors<<"(No file name): Internal error"<<endl;
        PerFile_Error << "Internal error";
        return false;
    }

    if (Field=="clef")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex!=(size_t)-1 && Chunks->Global->moov_trak_tapt_clef.count(Chunks->Global->moov_trak_FirstVideoIndex))
        {
            delete Chunks->Global->moov_trak_tapt_clef[Chunks->Global->moov_trak_FirstVideoIndex];
            Chunks->Global->moov_trak_tapt_clef.erase(Chunks->Global->moov_trak_FirstVideoIndex);
            Chunks->Global->moov_trak_tapt_clef_Modified=true;
        }

        return true;
    }
    else if (Field=="prof")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex!=(size_t)-1 && Chunks->Global->moov_trak_tapt_prof.count(Chunks->Global->moov_trak_FirstVideoIndex))
        {
            delete Chunks->Global->moov_trak_tapt_prof[Chunks->Global->moov_trak_FirstVideoIndex];
            Chunks->Global->moov_trak_tapt_prof.erase(Chunks->Global->moov_trak_FirstVideoIndex);
            Chunks->Global->moov_trak_tapt_prof_Modified=true;
        }

        return true;
    }
    else if (Field=="enof")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex!=(size_t)-1 && Chunks->Global->moov_trak_tapt_enof.count(Chunks->Global->moov_trak_FirstVideoIndex))
        {
            delete Chunks->Global->moov_trak_tapt_enof[Chunks->Global->moov_trak_FirstVideoIndex];
            Chunks->Global->moov_trak_tapt_enof.erase(Chunks->Global->moov_trak_FirstVideoIndex);
            Chunks->Global->moov_trak_tapt_enof_Modified=true;
        }

        return true;
    }
    else if (Field=="fiel")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex!=(size_t)-1 && Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel.count(Chunks->Global->moov_trak_FirstVideoIndex))
        {
            delete Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel[Chunks->Global->moov_trak_FirstVideoIndex];
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel.erase(Chunks->Global->moov_trak_FirstVideoIndex);
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel_Modified=true;
        }

        return true;
    }
    else if (Field=="colr")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex!=(size_t)-1 && Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr.count(Chunks->Global->moov_trak_FirstVideoIndex))
        {
            delete Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr[Chunks->Global->moov_trak_FirstVideoIndex];
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr.erase(Chunks->Global->moov_trak_FirstVideoIndex);
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr_Modified=true;
        }

        return true;
    }
    else if (Field=="gama")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex!=(size_t)-1 && Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama.count(Chunks->Global->moov_trak_FirstVideoIndex))
        {
            delete Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama[Chunks->Global->moov_trak_FirstVideoIndex];
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama.erase(Chunks->Global->moov_trak_FirstVideoIndex);
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama_Modified=true;
        }

        return true;
    }
    else if (Field=="pasp")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex!=(size_t)-1 && Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp.count(Chunks->Global->moov_trak_FirstVideoIndex))
        {
            delete Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp[Chunks->Global->moov_trak_FirstVideoIndex];
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp.erase(Chunks->Global->moov_trak_FirstVideoIndex);
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp_Modified=true;
        }

        return true;
    }
    else if (Field=="mdcv")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex!=(size_t)-1 && Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv.count(Chunks->Global->moov_trak_FirstVideoIndex))
        {
            delete Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Chunks->Global->moov_trak_FirstVideoIndex];
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv.erase(Chunks->Global->moov_trak_FirstVideoIndex);
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv_Modified=true;
        }

        return true;
    }
    else if (Field=="clli")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex!=(size_t)-1 && Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli.count(Chunks->Global->moov_trak_FirstVideoIndex))
        {
            delete Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli[Chunks->Global->moov_trak_FirstVideoIndex];
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli.erase(Chunks->Global->moov_trak_FirstVideoIndex);
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli_Modified=true;
        }

        return true;
    }
    else if (Field=="clap")
    {
        if (Chunks->Global->moov_trak_FirstVideoIndex!=(size_t)-1 && Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap.count(Chunks->Global->moov_trak_FirstVideoIndex))
        {
            delete Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Chunks->Global->moov_trak_FirstVideoIndex];
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap.erase(Chunks->Global->moov_trak_FirstVideoIndex);
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap_Modified=true;
        }

        return true;
    }
    else if (Field=="tmcd")
    {
        Chunks->Global->TimeCode_Track_Delete=true;
        return true;
    }
    else if (Field=="chan")
    {
        if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.empty())
        {

            for(map<size_t, mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_chan*>::iterator It=Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.begin();
                It!=Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.end(); It++)
                delete It->second;

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.clear();
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan_Modified=true;
        }

        return true;
    }
    else if (Field=="wscale" || Field=="display_primaries" || Field=="luminance")
        return false;

    return Set(Field, string(), *block_strings_Get(Field), Chunk_Name2_Get(Field), Chunk_Name3_Get(Field));
}

//---------------------------------------------------------------------------
bool mp4_Handler::IsModified(const string &Field)
{
    mp4_Base::global::block_strings** Chunk_Strings=block_strings_Get(Field);
    if (!Chunk_Strings || !*Chunk_Strings)
        return false;
    
    return IsModified(Field_Get(Field=="timereference (translated)"?string("timereference"):Field), *Chunk_Strings);
}

//---------------------------------------------------------------------------
bool mp4_Handler::IsValid(const string &Field_, const string &Value_)
{
    if (!IsValid_Errors.str().empty())
    {
        Errors<<(Chunks?Chunks->Global->File_Name.To_Local():"")<<": "<<IsValid_Errors.str()<<endl;
        PerFile_Error << IsValid_Errors.str();
        return false;
    }
    else
        return true;
}

//---------------------------------------------------------------------------
bool mp4_Handler::IsOriginal(const string &Field, const string &Value)
{
    mp4_Base::global::block_strings** Chunk_Strings=block_strings_Get(Field);
    if (!Chunk_Strings || !*Chunk_Strings)
        return true;
    
    return IsOriginal(Field_Get(Field=="timereference (translated)"?string("timereference"):Field), Value, *Chunk_Strings);
}

//---------------------------------------------------------------------------
string mp4_Handler::History(const string &Field)
{
    mp4_Base::global::block_strings** Chunk_Strings=block_strings_Get(Field);
    if (!Chunk_Strings || !*Chunk_Strings)
        return string();
    
    return History(Field_Get(Field), *Chunk_Strings);
}

//***************************************************************************
// Info
//***************************************************************************

//---------------------------------------------------------------------------
string mp4_Handler::Trace_Get()
{
    return Chunks->Global->Trace.str();
}

//---------------------------------------------------------------------------
string mp4_Handler::FileName_Get()
{
    return Chunks->Global->File_Name.To_Local();
}

//---------------------------------------------------------------------------
string mp4_Handler::FileDate_Get()
{
    return Chunks->Global->File_Date.To_Local();
}

//---------------------------------------------------------------------------
float mp4_Handler::Progress_Get()
{
    if (Chunks==NULL || Chunks->Global==NULL)
        return 0;    
    CriticalSectionLocker(Chunks->Global->CS);
    if (Chunks->Global->Progress==1)
        return (float)0.99; //Must finish opening, see Open()
    return Chunks->Global->Progress;
}

//---------------------------------------------------------------------------
void mp4_Handler::Progress_Clear()
{
    if (Chunks==NULL || Chunks->Global==NULL)
        return;

    CriticalSectionLocker(Chunks->Global->CS);
    Chunks->Global->Progress=0;
}

//---------------------------------------------------------------------------
bool mp4_Handler::Canceled_Get()
{
    if (Chunks==NULL || Chunks->Global==NULL)
        return false;    
    CriticalSectionLocker(Chunks->Global->CS);
    return File_IsCanceled;
}

//---------------------------------------------------------------------------
void mp4_Handler::Cancel()
{
    if (Chunks==NULL || Chunks->Global==NULL)
        return;    
    CriticalSectionLocker(Chunks->Global->CS);
    Chunks->Global->Canceling=true;
}

//---------------------------------------------------------------------------
bool mp4_Handler::IsModified_Get()
{
    return true;
}

//---------------------------------------------------------------------------
bool mp4_Handler::IsValid_Get()
{
    return File_IsValid;
}

//***************************************************************************
// Helpers - Per item
//***************************************************************************

//---------------------------------------------------------------------------
string mp4_Handler::Get(const string &Field, mp4_Base::global::block_strings* &Chunk_Strings)
{
    if (!File_IsValid)
        return string();

    return mp4_Handler_EmptyZtring_Const;
}

//---------------------------------------------------------------------------
bool mp4_Handler::Set(const string &Field, const string &Value, mp4_Base::global::block_strings* &Chunk_Strings, int32u Chunk_Name2, int32u Chunk_Name3)
{
    if (!File_IsValid
     || &Chunk_Strings==NULL) 
        return false;
    
    if ((Chunk_Strings!=NULL && Value==Chunk_Strings->Strings[Field])
     || (Chunk_Strings==NULL && Value.empty()))
        return true; //Nothing to do

    //Overwrite_Reject
    if (Overwrite_Reject && Chunk_Strings!=NULL && !Chunk_Strings->Strings[Field].empty())
    {
        Errors<<(Chunks?Chunks->Global->File_Name.To_Local():"")<<": overwriting is not authorized ("<<Field<<")"<<endl;
        PerFile_Error << "Overwriting is not authorized (" << Field << ")";
        return false;
    }

    //Filling
    bool Alreadyexists=false;
    for (size_t Pos=0; Pos<Chunk_Strings->Histories[Field].size(); Pos++)
        if (Chunk_Strings->Histories[Field][Pos]==Chunk_Strings->Strings[Field])
            Alreadyexists=true;
    if (!Alreadyexists)
        Chunk_Strings->Histories[Field].push_back(Chunk_Strings->Strings[Field]);
    Chunk_Strings->Strings[Field]=Value;

    return true;
}

//---------------------------------------------------------------------------
bool mp4_Handler::IsOriginal(const string &Field, const string &Value, mp4_Base::global::block_strings* &Chunk_Strings)
{
    if (!File_IsValid || &Chunk_Strings==NULL || Chunk_Strings==NULL)
        return Value.empty();
   
    if (Chunk_Strings->Histories[Field].empty())
        return Value==Chunk_Strings->Strings[Field];
   
    return Value==Chunk_Strings->Histories[Field][0];
}

//---------------------------------------------------------------------------
bool mp4_Handler::IsModified(const string &Field, mp4_Base::global::block_strings* &Chunk_Strings)
{
    if (!File_IsValid)
        return false;

    if (&Chunk_Strings!=NULL && Chunk_Strings && Chunk_Strings->Histories.find(Field)!=Chunk_Strings->Histories.end())
    {
        return !Chunk_Strings->Histories[Field].empty() && Chunk_Strings->Histories[Field][0]!=Chunk_Strings->Strings[Field];
    }
    else
        return false;
}

//---------------------------------------------------------------------------
string mp4_Handler::History(const string &Field, mp4_Base::global::block_strings* &Chunk_Strings)
{
    if (!File_IsValid)
        return string();

    return mp4_Handler_EmptyZtring_Const;
}

//***************************************************************************
// Configuration
//***************************************************************************

//---------------------------------------------------------------------------
void mp4_Handler::Options_Update()
{
    if (Chunks==NULL || Chunks->Global==NULL)
        return;

    Chunks->Global->NewChunksAtTheEnd=NewChunksAtTheEnd;
}

//***************************************************************************
// Helpers - Retrieval of blocks info
//***************************************************************************

//---------------------------------------------------------------------------
mp4_Base::global::block_strings** mp4_Handler::block_strings_Get(const string &Field)
{
    if (Chunks==NULL || Chunks->Global==NULL)
        return NULL;    

    //INFO
    //if (Field.size()==4)
    //    return &Chunks->Global->INFO;
    
    //Unknown
    return NULL;
}

//---------------------------------------------------------------------------
string mp4_Handler::Field_Get(const string &Field)
{
    //Unknown
    return 0x00000000;
}

//---------------------------------------------------------------------------
int32u mp4_Handler::Chunk_Name2_Get(const string &Field)
{  
    //Unknown
    return 0x00000000;
}

//---------------------------------------------------------------------------
int32u mp4_Handler::Chunk_Name3_Get(const string &Field)
{
    //Unknown / not needed
    return 0x00000000;
}
