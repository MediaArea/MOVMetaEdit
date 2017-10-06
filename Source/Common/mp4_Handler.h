/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifndef mp4_HandlerH
#define mp4_HandlerH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "ZenLib/File.h"
#include "ZenLib/Ztring.h"
#include <string>
#include <sstream>
#include <bitset>
#include "Common/mp4_Handler.h"
#include "Common/mp4/mp4_.h"
using namespace ZenLib;
using namespace std;
class mp4;
//---------------------------------------------------------------------------

//***************************************************************************
// 
//***************************************************************************

class mp4_Handler
{
public:
    //---------------------------------------------------------------------------
    //Constructor/Destructor
    mp4_Handler();
    ~mp4_Handler();
    
    //---------------------------------------------------------------------------
    //I/O
    bool            Open            (const string &FileName);
    bool            Save            ();

    //---------------------------------------------------------------------------
    //Per Item
    string          Get                 (const string &Field);
    bool            Set                 (const string &Field, const string &Value);
    bool            Remove              (const string &Field);
    string          History             (const string &Field);
    bool            IsOriginal          (const string &Field, const string &Value);
    bool            IsValid             (const string &Field, const string &Value);
    string          IsValid_LastError   () {return IsValid_Errors.str();}
    bool            IsModified          (const string &Field);
    
    //---------------------------------------------------------------------------
    //Info
    string          Trace_Get();
    string          FileName_Get();
    string          FileDate_Get();
    float           Progress_Get();
    void            Progress_Clear();
    bool            Canceled_Get();
    void            Cancel();
    bool            IsValid_Get();
    bool            IsModified_Get();

    //---------------------------------------------------------------------------
    //Configuration
    bool            Overwrite_Reject;
    bool            NewChunksAtTheEnd;
    void            Options_Update();

    //---------------------------------------------------------------------------
    //Stdxxx
    ostringstream   Errors;
    ostringstream   Information;
    ostringstream   PerFile_Error;
    ostringstream   PerFile_Information;
    ostringstream   IsValid_Errors;

private:
    //---------------------------------------------------------------------------
    //Helpers - Per item
    string          Get             (const string &Field,                       mp4_Base::global::block_strings* &Chunk_Strings);
    bool            Set             (const string &Field, const string &Value,  mp4_Base::global::block_strings* &Chunk_Strings, int32u Chunk_Name2, int32u Chunk_Name3=0x00000000);
    bool            IsModified      (const string &Field,                       mp4_Base::global::block_strings* &Chunk_Strings);
    bool            IsOriginal      (const string &Field, const string &Value,  mp4_Base::global::block_strings* &Chunk_Strings);
    string          History         (const string &Field,                       mp4_Base::global::block_strings* &Chunk_Strings);
    
    //---------------------------------------------------------------------------
    //Helpers - Retrieval of blocks info
    mp4_Base::global::block_strings** block_strings_Get    (const string &Field);
    string Field_Get                                        (const string &Field);
    int32u Chunk_Name2_Get                                  (const string &Field);
    int32u Chunk_Name3_Get                                  (const string &Field);

    //---------------------------------------------------------------------------
    //Internal
    ZenLib::File    In;
    ZenLib::File    Out;
    mp4*            Chunks;
    bool            File_IsValid;
    bool            File_IsCanceled;
};

#endif
