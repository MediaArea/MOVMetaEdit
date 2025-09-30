/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "CLI/Help.h"
#include "Common/config.h"
//---------------------------------------------------------------------------

//***************************************************************************
//
//***************************************************************************

//---------------------------------------------------------------------------
static string Program_Name = "movmetaedit";

void Set_Program_Name(const char* Name)
{
    Program_Name = Name;
    #if defined(_MSC_VER)
        Program_Name = Program_Name.substr(Program_Name.rfind('\\')+1);
        Program_Name = Program_Name.substr(0, Program_Name.find('.'));
    #else
        Program_Name = Program_Name.substr(Program_Name.rfind('/')+1);
    #endif
}

//---------------------------------------------------------------------------
ReturnValue Help()
{
    STRINGOUT(string("Usage: \" [options] FileNames [options]\"").insert(8, Program_Name));
    TEXTOUT("");
    TEXTOUT("This tool was developed for 2 specific purposes:");
    TEXTOUT("- Modify the technical metadata)");
    TEXTOUT("- Inject or edit Ad-ID metadata");
    TEXTOUT("");
    TEXTOUT("Help:");
    TEXTOUT("  --help, -h");
    TEXTOUT("      Display this help and exit");
    TEXTOUT("  --help-tech");
    TEXTOUT("      Display help for modifications related to the technical metadata");
    TEXTOUT("  --help-adid");
    TEXTOUT("      Display help for modifications related to Ad-iD");
    TEXTOUT("  --version");
    TEXTOUT("      Display version and exit");
    TEXTOUT("");
    Help_Tech(false);
    TEXTOUT("");
    Help_AdID(false);
    TEXTOUT("");
    TEXTOUT("If no option, aspect ratio information is displayed, no modification");

    return ReturnValue_OK;
}

//---------------------------------------------------------------------------
ReturnValue Help_AdID(bool WithExamples)
{
    TEXTOUT("Options related to Universal Ad ID:");
    TEXTOUT("  --adid VALUE");
    TEXTOUT("      Add or modify an Universal Ad ID with the specified VALUE");
    TEXTOUT("      Use VALUE of \"auto\" for filling automatically from the file name");
    TEXTOUT("      (e.g. 11-12 alphanumeric characters for \"ad-id.org\" registry )");
    TEXTOUT("  --adid-registry VALUE");
    TEXTOUT("      Id-iD is stored with the specified VALUE as registry");
    TEXTOUT("      (default is \"ad-id.org\")");
    TEXTOUT("  --simulate, -s");
    TEXTOUT("      Do not modify file (only display of potential modifications)");
    if (WithExamples)
    {
    TEXTOUT("");
    TEXTOUT("Examples:");
    STRINGOUT(string(" --adid auto FileName0000.mov").insert(0, Program_Name));
    STRINGOUT(string(" --adid ADID0000000 FileName.mov").insert(0, Program_Name));
    STRINGOUT(string(" --adid-registry example.com --adid ADID0000000 FileName.mov").insert(0, Program_Name));
    }

    return ReturnValue_OK;
}

//---------------------------------------------------------------------------
ReturnValue Help_Tech(bool WithExamples)
{
    TEXTOUT("Options related to pixel aspect ratio:");
    TEXTOUT("  --par, -p VALUE");
    TEXTOUT("      Modify PAR to VALUE, \"x:y\" format");
    TEXTOUT("  --width-scale, -w VALUE");
    TEXTOUT("      Modify width scale to VALUE, real number");
    TEXTOUT("Options related to the aperture mode dimensions:");
    TEXTOUT("  --clean VALUE");
    TEXTOUT("      Modify the clean aperture dimensions to VALUE, \"WxH\" format");
    TEXTOUT("  --clean-delete");
    TEXTOUT("      Delete the clean aperture dimensions atom");
    TEXTOUT("  --prod VALUE");
    TEXTOUT("      Modify the production aperture dimensions to VALUE, \"WxH\" format");
    TEXTOUT("  --prod-delete");
    TEXTOUT("      Delete the production aperture dimensions atom");
    TEXTOUT("  --pixels VALUE");
    TEXTOUT("      Modify the aperture pixels size to VALUE, \"WxH\" format");
    TEXTOUT("  --pixels-delete");
    TEXTOUT("      Delete the aperture pixels size atom");
    TEXTOUT("Options related to the field handling");
    TEXTOUT("  --field VALUE");
    TEXTOUT("      Modify the field handling parameter to VALUE, \"fields,detail\" format");
    TEXTOUT("  --field-delete");
    TEXTOUT("      Delete the field handling atom");
    TEXTOUT("Options related to video stsd field handling");
    TEXTOUT("  --stsd-video-version VALUE");
    TEXTOUT("      Modify the field version to VALUE for the first video stream, integer value between 0 and 3");
    TEXTOUT("  --temporal-quality VALUE");
    TEXTOUT("    Modify the temporal quality to VALUE, integer value");
    TEXTOUT("Options related to the color parameter:");
    TEXTOUT("  --color VALUE");
    TEXTOUT("      Modify the color parameter to VALUE, \"primaries,transfer,matrix\" format");
    TEXTOUT("  --color-delete");
    TEXTOUT("      Delete the color atom");
    TEXTOUT("Options related to the gamma value:");
    TEXTOUT("  --gamma VALUE");
    TEXTOUT("      Modify the gamma to VALUE, real number");
    TEXTOUT("  --gamma-delete");
    TEXTOUT("      Delete the gamma atom");
    TEXTOUT("Options related to the clean aperture:");
    TEXTOUT("  --aperture VALUE");
    TEXTOUT("      Modify the clean aperture to VALUE, \"with:d,height:d,hOffset:d,vOffset:d\" format");
    TEXTOUT("      (':d' (denominator) are optionnal')");
    TEXTOUT("  --aperture-delete");
    TEXTOUT("      Delete the clean aperture atom");
    TEXTOUT("Options related to high dynamic range:");
    TEXTOUT("  --display-primaries VALUE");
    TEXTOUT("      Modify the display primaries and white-point to VALUE, \"rx,ry,gx,gy,bx,by,wpx,wpy\" format");
    TEXTOUT("  --luminance VALUE");
    TEXTOUT("      Modify the luminance to VALUE, \"max,min\" format");
    TEXTOUT("      (display primaries must be already presents in file or specified in the command-line)");
    TEXTOUT("  --mdcv-delete");
    TEXTOUT("      Delete the mdcv atom (display primaries and luminance)");
    TEXTOUT("  --maximum-content-light-level VALUE");
    TEXTOUT("      Modify the maximum content light level to VALUE, real number");
    TEXTOUT("  --maximum-frame-average-light-level VALUE");
    TEXTOUT("      Modify the maximum frame average light level to VALUE, real number");
    TEXTOUT("  --clli-delete");
    TEXTOUT("      Delete the clli atom (light levels)");
    TEXTOUT("  --from FILE");
    TEXTOUT("      Read HDR values from DolbyLabsMDF XML");
    TEXTOUT("  --from-id VALUE");
    TEXTOUT("      Read HDR values from the MasteringDisplay with the the specified ID in the XML");
    TEXTOUT("Options related to the timecode track:");
    TEXTOUT("  --timecode-delete");
    TEXTOUT("      Remove the timecode track and all related references atoms");
    TEXTOUT("");
    TEXTOUT("Options related to the media header atom:");
    TEXTOUT("  --language, --languages VALUE");
    TEXTOUT("      Modify the media header language to VALUE for the specified audio track, \"[trackIndex=]lang[,[trackIndex=]lang...]\" format, e.g en,fr or 0=eng,1=fra");
    TEXTOUT("      MOV style: Lang can be numeric or any of these predefined keywords(ISO 639-1) : en, fr, de, it, nl, sv, es, da, pt, no, he, ja, ar, fi, el, is,");
    TEXTOUT("      mt, tr, hr, zh-tw, ur, hi, th, ko, lt, pl, hu, et, lv, smi, fo, fa, ru, zh-cn, nl-be, ga, sq, ro, cs, sk, sl, yi, sr, mk, bg, uk, be, uz, kk,");
    TEXTOUT("      hy-az, hy, ka, mo, ky, tg, tk, mn-cn, mn, ps, ku, ks, sd, bo, ne, sa, mr, bn, as, gu, pa, or, ml, kn, ta, te, si, my, km, lo, vi, id,");
    TEXTOUT("      az, tl, ms, ms-bn, am, om, so, sw, rw, rn, ny, mg, eo, cy, eu, ca, la, qu, gn, ay, tt, ug, dz, jv");
    TEXTOUT("      MP4 style: Lang is a 3-letter code which should be a ISO 639-2 language code");
    TEXTOUT("");
    TEXTOUT("Options related to the audio channels description labels:");
    TEXTOUT("  --channels[-label] VALUE");
    TEXTOUT("      Modify the channels description to VALUE for the specified track, \"[trackIndex=]code[+code][,[trackIndex=]code[+code]...]\" format, e.g L+R, L,R or 0=L,1=L+R,3=Delete");
    TEXTOUT("      Code can be numeric or any of these predefined keywords: Delete(delete the channel information atom), L, R, C, LFE, Ls, Rs, Lc, Rc, Cs,");
    TEXTOUT("      Lsd, Rsd, Tcs, Vhl, Vhc, Vhr, Trs, Trs2, Trs3, Lrs, Rrs, Lw, Rw, LFE2, Lt, Rt, HearingImpaired, Narration, M, DialogCentricMix,");
    TEXTOUT("      CenterSurroundDirect, Haptic, W, X, Y,Z, M2, S, X2, Y2, HeadphonesLeft, HeadphonesRight, ClickTrack, ForeignLanguage,");
    TEXTOUT("      Discrete, Discrete-1, Discrete-2...");
    TEXTOUT("      (WARNING existing channels descriptions for the track will be overwritten)");
    TEXTOUT("      (WARNING Setting channel labels will automatically set the layout to UseChannelDescriptions)");
    TEXTOUT("  --channels-layout VALUE");
    TEXTOUT("      Modify the channels layout to VALUE for the specified track, \"[trackIndex=]layout[,[trackIndex=]layout...]\" format, e.g Mono, Stereo or 0=Stereo,1=MPEG_5_1_A");
    TEXTOUT("      Layout can be numeric or any of these predefined keywords: UseChannelDescriptions, UseChannelBitmap, Mono, Stereo, StereoHeadphones, MatrixStereo, MidSide,");
    TEXTOUT("      XY, Binaural, Ambisonic_BFormat, Quadraphonic, Pentagonal, Hexagonal, Octagonal, Cube, MPEG_1.0, MPEG_2.0, MPEG_3.0A, MPEG_3.0B, MPEG_4.0A, MPEG_4.0B,");
    TEXTOUT("      MPEG_5.0A, MPEG_5.0B, MPEG_5.0C, MPEG_5.0D, MPEG_5.1A, MPEG_5.1B, MPEG_5.1C, MPEG_5.1D, MPEG_6.1A, MPEG_7.1A, MPEG_7.1B, MPEG_7.1C, Emagic_7.1, SMPTE_DTV,");
    TEXTOUT("      ITU_1.0, ITU_2.0, ITU_2.1, ITU_2.2, ITU_3.0, ITU_3.1, ITU_3.2, ITU_3.2.1, ITU_3.4.1, DVD_0, DVD_1, DVD_2, DVD_3, DVD_4, DVD_5, DVD_6, DVD_7, DVD_8, DVD_9,");
    TEXTOUT("      DVD_10, DVD_11, DVD_12, DVD_13, DVD_14, DVD_15, DVD_16, DVD_17, DVD_18, DVD_19, DVD_20, AudioUnit_4, AudioUnit_5, AudioUnit_6, AudioUnit_8, AudioUnit_5.0,");
    TEXTOUT("      AudioUnit_6.0, AudioUnit_7.0, AudioUnit_7.0Front, AudioUnit_5.1, AudioUnit_6.1, AudioUnit_7.1, AudioUnit_7.1Front, AAC_3.0, AAC_Quadraphonic, AAC_4.0,");
    TEXTOUT("      AAC_5.0, AAC_5.1, AAC_6.0, AAC_6.1, AAC_7.0, AAC_7.1, AAC_Octagonal, TMH_10.2std, TMH_10.2full, AC3_1.0.1, AC3_3.0, AC3_3.1, AC3_3.0.1, AC3_2.1.1, AC3_3.1.1,");
    TEXTOUT("      DiscreteInOrder, Unknown");
    TEXTOUT("      (WARNING Setting the layout to any value other than UseChannelDescriptions will remove any existing channels descriptions for the track)");
    TEXTOUT("  --channels-delete");
    TEXTOUT("      Delete the channel atom in ALL audio tracks");
    TEXTOUT("");
    TEXTOUT("  --simulate, -s");
    TEXTOUT("      Do not modify file (only display of potential modifications)");
    if (WithExamples)
    {
    TEXTOUT("");
    TEXTOUT("Examples:");
    STRINGOUT(string(" --par 9:10 FileName.mov").insert(0, Program_Name));
    STRINGOUT(string(" --width-scale 0.9 FileName.mov").insert(0, Program_Name));
    }

    return ReturnValue_OK;
}

//---------------------------------------------------------------------------
ReturnValue Usage()
{
    STRINGOUT(string("Usage: \" [options] FileNames [options]\"").insert(8, Program_Name));
    STRINGOUT(string("\" --help\" for displaying more information").insert(1, Program_Name));

    return ReturnValue_ERROR;
}

//---------------------------------------------------------------------------
ReturnValue Version()
{
    STRINGOUT(string(Program_Name)+' '+VERSION);

    return ReturnValue_OK;
}
