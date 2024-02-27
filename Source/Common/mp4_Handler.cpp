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
#include <iostream>
#include <algorithm>
#include "ZenLib/File.h"
#include "ZenLib/Dir.h"

#ifdef MACSTORE
#include "Common/Mac_Helpers.h"
#include "mp4_Handler.h"
#endif

using namespace std;
using namespace ZenLib;
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
bool mp4_chan_ChannelCode (string ChannelLabel, uint32_t &Code, bool& Ignore, bool& Delete)
{
    Delete=false;
    Ignore=false;
    Code=0;

    if(ChannelLabel.empty())
        return false;

    istringstream iss(ChannelLabel);
    iss >> Code;
    if (!iss.fail() && iss.eof())
        return true;

    transform( ChannelLabel.begin(), ChannelLabel.end(), ChannelLabel.begin(), ::tolower);
    if (ChannelLabel=="delete")
        Delete=true;
    else if (ChannelLabel=="")
        Code=0;
    else if (ChannelLabel=="absent")
        Ignore=true;
    else if (ChannelLabel=="nodescription")
        Ignore=true;
    else if (ChannelLabel=="multiples")
        Ignore=true;
    else if (ChannelLabel=="l")
        Code=1;
    else if (ChannelLabel=="r")
        Code=2;
    else if (ChannelLabel=="c")
        Code=3;
    else if (ChannelLabel=="lfe")
        Code=4;
    else if (ChannelLabel=="ls")
        Code=5;
    else if (ChannelLabel=="rs")
        Code=6;
    else if (ChannelLabel=="lc")
        Code=7;
    else if (ChannelLabel=="rc")
        Code=8;
    else if (ChannelLabel=="cs")
        Code=9;
    else if (ChannelLabel=="lsd")
        Code=10;
    else if (ChannelLabel=="rsd")
        Code=11;
    else if (ChannelLabel=="tcs")
        Code=12;
    else if (ChannelLabel=="vhl")
        Code=13;
    else if (ChannelLabel=="vhc")
        Code=14;
    else if (ChannelLabel=="vhr")
        Code=15;
    else if (ChannelLabel=="trs")
        Code=16;
    else if (ChannelLabel=="trs2")
        Code=17;
    else if (ChannelLabel=="trs3")
        Code=18;
    else if (ChannelLabel=="lrs")
        Code=33;
    else if (ChannelLabel=="rrs")
        Code=34;
    else if (ChannelLabel=="lw")
        Code=35;
    else if (ChannelLabel=="rw")
        Code=36;
    else if (ChannelLabel=="lfe2")
        Code=37;
    else if (ChannelLabel=="lt")
        Code=38;
    else if (ChannelLabel=="rt")
        Code=39;
    else if (ChannelLabel=="hearingimpaired")
        Code=40;
    else if (ChannelLabel=="narration")
        Code=41;
    else if (ChannelLabel=="m")
        Code=42;
    else if (ChannelLabel=="dialogcentricmix")
        Code=43;
    else if (ChannelLabel=="centersurrounddirect")
        Code=44;
    else if (ChannelLabel=="haptic")
        Code=45;
    else if (ChannelLabel=="w")
        Code=200;
    else if (ChannelLabel=="x")
        Code=201;
    else if (ChannelLabel=="y")
        Code=202;
    else if (ChannelLabel=="z")
        Code=203;
    else if (ChannelLabel=="m2")
        Code=204;
    else if (ChannelLabel=="s")
        Code=205;
    else if (ChannelLabel=="x2")
        Code=206;
    else if (ChannelLabel=="y2")
        Code=207;
    else if (ChannelLabel=="headphonesleft")
        Code=301;
    else if (ChannelLabel=="headphonesright")
        Code=302;
    else if (ChannelLabel=="clicktrack")
        Code=304;
    else if (ChannelLabel=="foreignlanguage")
        Code=305;
    else if (ChannelLabel=="discrete")
        Code=400;
    else if (ChannelLabel.size()>9 && ChannelLabel.find("discrete-")==0)
    {
        uint32_t Number=0;
        istringstream iss(ChannelLabel.substr(9));
        iss >> Number;
        if (iss.fail() || !iss.eof() || Number>0xFFFF)
            return false;

        Code=0x10000+Number;
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
        if (!Chunks->Global->moov_trak_tapt_clef)
            return string();

        stringstream ss; ss << Chunks->Global->moov_trak_tapt_clef->Width << "x" << Chunks->Global->moov_trak_tapt_clef->Height;
        return ss.str();
    }
    else if (Field=="prof")
    {
        if (!Chunks->Global->moov_trak_tapt_prof)
            return string();

        stringstream ss; ss << Chunks->Global->moov_trak_tapt_prof->Width << "x" << Chunks->Global->moov_trak_tapt_prof->Height;
        return ss.str();
    }
    else if (Field=="enof")
    {
        if (!Chunks->Global->moov_trak_tapt_enof)
            return string();

        stringstream ss; ss << Chunks->Global->moov_trak_tapt_enof->Width << "x" << Chunks->Global->moov_trak_tapt_enof->Height;
        return ss.str();
    }
    else if (Field=="fiel")
    {
        if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel)
            return string();

        stringstream ss; ss << (uint16_t)Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel->Fields << ","
                            << (uint16_t)Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel->Detail;
        return ss.str();
    }
    else if (Field=="colr")
    {
        if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr)
            return string();

        stringstream ss; ss << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr->Primaries << ","
                            << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr->Transfer << ","
                            << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr->Matrix;
        return ss.str();
    }
    else if (Field=="gama")
    {
        if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama)
            return string();

        stringstream ss; ss << setprecision(3) << fixed << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama->Gamma;
        return ss.str();
    }
    else if (Field=="pasp")
    {
        if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp)
            return string();

        stringstream ss; ss << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp->hSpacing << ":"
                            << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp->vSpacing;
        return ss.str();
    }
    else if (Field=="display_primaries")
    {
        if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv)
            return string();

        stringstream ss;
        for (size_t c = 0; c < Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries.size(); c++)
            ss << (c ? "," : "") << setprecision(3) << fixed << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[c];

        return ss.str();
    }
    else if (Field=="luminance")
    {
        if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv)
            return string();

        stringstream ss;
        ss << setprecision(4) << fixed << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Luminance[0] << ","
           << setprecision(4) << fixed << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Luminance[1];

        return ss.str();
    }
    else if (Field=="maximum_content_light_level")
    {
        if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli)
            return string();

        stringstream ss;
        ss << setprecision(4) << fixed << round(Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli->maximum_content_light_level);

        return ss.str();
    }
    else if (Field=="maximum_frame_average_light_level")
    {
        if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli)
            return string();

        stringstream ss;
        ss << setprecision(4) << fixed << round(Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli->maximum_frame_average_light_level);

        return ss.str();
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

            map<size_t, mp4_Base::global::block_moov_trak_mdia_mdhd*>::iterator It=Chunks->Global->moov_trak_mdia_mdhd.find(Pos+1);
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
        stringstream ss;
        size_t Index=0;

        for (size_t Pos=0; Pos<Chunks->Global->moov_trak.size(); Pos++)
        {
            if (!Chunks->Global->moov_trak[Pos]->IsSound)
                continue;

            if (!ss.str().empty())
                ss << ",";

            ss << Index << "=";

            map<size_t, mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_chan*>::iterator It=Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.find(Pos+1);
            if (It!=Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.end())
            {
                if (It->second->Descriptions.empty())
                    ss << "NODESCRIPTION";
                else if (It->second->Descriptions.size()==1)
                    ss << It->second->Descriptions[0].ChannelLabel;
                else
                    ss << "MULTIPLES";
            }
            else
                ss << "ABSENT";
            Index++;
        }
        return ss.str();
    }
    else if (Field=="clap")
    {
        if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap)
            return string();

        stringstream ss;
        ss << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Width_Num;
        if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Width_Den!=1)
            ss << ":" << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Width_Den;
        ss << "," << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Height_Num;
        if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Height_Den!=1)
            ss << ":" << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Height_Den;
        ss << "," << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Horizontal_Offset_Num;
        if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Horizontal_Offset_Den!=1)
            ss << ":" << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Horizontal_Offset_Den;
        ss << "," << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Vertical_Offset_Num;
        if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Vertical_Offset_Den!=1)
            ss << ":" << Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Vertical_Offset_Den;

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
            if (!Chunks->Global->moov_trak_tapt_clef)
                Chunks->Global->moov_trak_tapt_clef=new mp4_Base::global::block_moov_trak_tapt_xxxx();

            Chunks->Global->moov_trak_tapt_clef->Width=Width;
            Chunks->Global->moov_trak_tapt_clef->Height=Height;
            Chunks->Global->moov_trak_tapt_clef_Modified=true;
        }
        else if (!Simulate && Field=="prof")
        {
            if (!Chunks->Global->moov_trak_tapt_prof)
                Chunks->Global->moov_trak_tapt_prof=new mp4_Base::global::block_moov_trak_tapt_xxxx();

            Chunks->Global->moov_trak_tapt_prof->Width=Width;
            Chunks->Global->moov_trak_tapt_prof->Height=Height;
            Chunks->Global->moov_trak_tapt_prof_Modified=true;
        }
        else if (!Simulate && Field=="enof")
        {
            if (!Chunks->Global->moov_trak_tapt_enof)
                Chunks->Global->moov_trak_tapt_enof=new mp4_Base::global::block_moov_trak_tapt_xxxx();

            Chunks->Global->moov_trak_tapt_enof->Width=Width;
            Chunks->Global->moov_trak_tapt_enof->Height=Height;
            Chunks->Global->moov_trak_tapt_enof_Modified=true;
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
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel)
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_fiel();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel->Fields=Fields;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel->Detail=Detail;
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
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr)
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_colr();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr->Primaries=Primaries;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr->Transfer=Transfer;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr->Matrix=Matrix;
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
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama)
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_gama();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama->Gamma=Gamma;
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
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp)
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_pasp();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp->hSpacing=hSpacing;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp->vSpacing=vSpacing;
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
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv)
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[0]=x1;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[1]=y1;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[2]=x2;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[3]=y2;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[4]=x3;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[5]=y3;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[6]=wp_x;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[7]=wp_y;
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

        if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv) // Check if primaries values are present
            return false;

        if (!Simulate)
        {
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv)
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Luminance[0]=max;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Luminance[1]=min;
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
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli)
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_clli();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli->maximum_content_light_level=max;
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
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli)
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_clli();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli->maximum_frame_average_light_level=max;
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

        if (!Simulate)
        {
            if (mdcv)
            {
                if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv)
                    Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv();

                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[4]=x1;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[5]=y1;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[0]=x2;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[1]=y2;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[2]=x3;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[3]=y3;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[6]=wp_x;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Primaries[7]=wp_y;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Luminance[0]=luminance_max;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv->Luminance[1]=luminance_min;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv_Modified=true;
            }

            if (clli)
            {
                if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli)
                    Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_clli();

                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli->maximum_content_light_level=max_cll;
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli->maximum_frame_average_light_level=max_fall;
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
            if (!Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap)
                Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_clap();

            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Width_Num=Aperture_Width_Num;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Width_Den=Aperture_Width_Den;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Height_Num=Aperture_Height_Num;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Aperture_Height_Den=Aperture_Height_Den;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Horizontal_Offset_Num=Horizontal_Offset_Num;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Horizontal_Offset_Den=Horizontal_Offset_Den;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Vertical_Offset_Num=Vertical_Offset_Num;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap->Vertical_Offset_Den=Vertical_Offset_Den;
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
            for (size_t Pos=0; Pos<Chunks->Global->moov_trak.size(); Pos++)
            {
                if (!Chunks->Global->moov_trak[Pos]->IsSound)
                    continue;

                if (Index++==Track)
                {
                    trak_Index=Pos+1; //Track indexes are 1 based
                    break;
                }
            }

            if (!trak_Index)
            {
                Ok=false;
                continue;
            }

            if (!Chunks->Global->moov_trak[trak_Index-1]->IsSound)
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
            uint32_t Code;
            bool Delete=false;
            bool Ignore=false;

            istringstream iss(Current.substr(0, Equal_Pos));
            iss >> Track;
            if (iss.fail() || !iss.eof())
            {
                Ok=false;
                break;
            }

            if (!mp4_chan_ChannelCode(Current.substr(Equal_Pos+1), Code, Ignore, Delete))
            {
                Ok=false;
                break;
            }

            size_t Index=0;
            size_t trak_Index=0;
            for (size_t Pos=0; Pos<Chunks->Global->moov_trak.size(); Pos++)
            {
                if (!Chunks->Global->moov_trak[Pos]->IsSound)
                    continue;

                if (Index++==Track)
                {
                    trak_Index=Pos+1; //Track indexes are 1 based
                    break;
                }
            }

            if (!trak_Index)
            {
                Ok=false;
                continue;
            }

            if (!Chunks->Global->moov_trak[trak_Index-1]->IsSound)
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
                }
                else
                {
                    if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.find(trak_Index)==Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan.end())
                        Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[trak_Index]=new mp4_Base::global::block_moov_trak_mdia_minf_stbl_stsd_xxxx_chan;

                    Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[trak_Index]->NumberChannelDescriptions=1;
                    Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[trak_Index]->Descriptions.resize(1);
                    Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_chan[trak_Index]->Descriptions[0].ChannelLabel=Code;
                }
            }
        }
        while((Start=Value.find_first_not_of(',', End))!=string::npos);

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
        if (Chunks->Global->moov_trak_tapt_clef)
        {
            delete Chunks->Global->moov_trak_tapt_clef;
            Chunks->Global->moov_trak_tapt_clef=NULL;
            Chunks->Global->moov_trak_tapt_clef_Modified=true;
        }

        return true;
    }
    else if (Field=="prof")
    {
        if (Chunks->Global->moov_trak_tapt_prof)
        {
            delete Chunks->Global->moov_trak_tapt_prof;
            Chunks->Global->moov_trak_tapt_prof=NULL;
            Chunks->Global->moov_trak_tapt_prof_Modified=true;
        }

        return true;
    }
    else if (Field=="enof")
    {
        if (Chunks->Global->moov_trak_tapt_enof)
        {
            delete Chunks->Global->moov_trak_tapt_enof;
            Chunks->Global->moov_trak_tapt_enof=NULL;
            Chunks->Global->moov_trak_tapt_enof_Modified=true;
        }

        return true;
    }
    else if (Field=="fiel")
    {
        if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel)
        {
            delete Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel=NULL;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_fiel_Modified=true;
        }

        return true;
    }
    else if (Field=="colr")
    {
        if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr)
        {
            delete Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr=NULL;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_colr_Modified=true;
        }

        return true;
    }
    else if (Field=="gama")
    {
        if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama)
        {
            delete Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama=NULL;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_gama_Modified=true;
        }

        return true;
    }
    else if (Field=="pasp")
    {
        if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp)
        {
            delete Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp=NULL;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_pasp_Modified=true;
        }

        return true;
    }
    else if (Field=="mdcv")
    {
        if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv)
        {
            delete Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv=NULL;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv_Modified=true;
        }

        return true;
    }
    else if (Field=="clli")
    {
        if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli)
        {
            delete Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli=NULL;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clli_Modified=true;
        }

        return true;
    }
    else if (Field=="clap")
    {
        if (Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap)
        {
            delete Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap=NULL;
            Chunks->Global->moov_trak_mdia_minf_stbl_stsd_xxxx_clap_Modified=true;
        }

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
