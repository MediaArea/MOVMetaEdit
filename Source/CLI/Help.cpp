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
    TEXTOUT("- Modify PAR (Pixel Aspect Ratio)");
    TEXTOUT("- Inject or edit Ad-ID metadata");
    TEXTOUT("");
    TEXTOUT("Help:");
    TEXTOUT("  --help, -h");
    TEXTOUT("      Display this help and exit");
    TEXTOUT("  --help-par");
    TEXTOUT("      Display help for modifications related to pixel aspect ratio");
    TEXTOUT("  --help-adid");
    TEXTOUT("      Display help for modifications related to Ad-iD");
    TEXTOUT("  --version");
    TEXTOUT("      Display version and exit");
    TEXTOUT("");
    Help_PAR(false);
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
ReturnValue Help_PAR(bool WithExamples)
{
    TEXTOUT("Options related to pixel aspect ratio:");
    TEXTOUT("  --par, -p VALUE");
    TEXTOUT("      Modify PAR to VALUE, \"x:y\" format");
    TEXTOUT("  --width-scale, -w VALUE");
    TEXTOUT("      Modify width scale to VALUE, real number");
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
