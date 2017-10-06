/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifndef mp4_BaseH
#define mp4_BaseH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "ZenLib/Ztring.h"
#include "ZenLib/File.h"
#include "ZenLib/CriticalSection.h"
#include <vector>
#include <map>
#include <sstream>
#include <stdint.h>
using namespace ZenLib;
using namespace std;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
const int64u _Size_Limit=0xFFFFFFFF; //Limit about when we implement ds64
const int64u _free_DefaultSise=16*1024; //Default size of FLLR at the beginning of a file
//---------------------------------------------------------------------------

//***************************************************************************
// Exceptions
//***************************************************************************

class exception_read : public exception
{
    virtual const char* what() const throw()
    {
        return "error during reading";
    }
};

class exception_canceled : public exception
{
    virtual const char* what() const throw()
    {
        return "canceled";
    }
};

class exception_valid : public exception
{
public:
    exception_valid(const string &text_in) throw () : exception() {text="invalid MP4/MOV: "+text_in;}
    ~exception_valid() throw () {};
    virtual const char* what() const throw()
    {
        return text.c_str();
    }

protected:
    string text;    
};

class exception_read_block : public exception
{
public:
    exception_read_block(const string &text_in) throw () : exception() {text=text_in;}
    ~exception_read_block() throw () {};
    virtual const char* what() const throw()
    {
        return text.c_str();
    }

protected:
    string text;    
};

class exception_write : public exception
{
public:
    exception_write(const string &text_in) throw () : exception() {text=text_in;}
    exception_write() throw () {text="error during writing";};
    ~exception_write() throw () {};
    virtual const char* what() const throw()
    {
        return text.c_str();
    }

protected:
    string text;    
};


//***************************************************************************
// mp4_Base
//***************************************************************************

class mp4_Base
{
public:
    //***************************************************************************
    // Structures
    //***************************************************************************

    //---------------------------------------------------------------------------
    //Global structure for handling common data
    struct global
    {
        struct block_moov_meta_list
        {
            int32u size;
            int32u name;
            int8u* value; //Size is size-8
            size_t data_Pos;
            size_t data_Size;
            string ToBeReplacedBy;
            bool   ToBeReplacedBy_Modified;

            block_moov_meta_list()
                : size(0)
                , name(0)
                , value(NULL)
                , data_Pos(0)
                , data_Size(0)
                , ToBeReplacedBy_Modified(false)
            {}

            block_moov_meta_list(int32u size_, int32u name_, int8u* value_)
                : size(size_)
                , name(name_)
                , value(value_)
                , data_Pos(0)
                , data_Size(0)
                , ToBeReplacedBy_Modified(false)
            {}

            block_moov_meta_list(int32u size_, int32u name_, int8u* value_, size_t data_Pos_, size_t data_Size_)
                : size(size_)
                , name(name_)
                , value(value_)
                , data_Pos(data_Pos_)
                , data_Size(data_Size_)
            {}
        };
        struct block_moov_meta_key
        {
            int32u size;
            int32u namespace_;
            int8u* value; //Size is size-8

            block_moov_meta_key()
                : size(0)
                , namespace_(0)
                , value(NULL)
            {}

            block_moov_meta_key(int32u size_, int32u namespace__, int8u* value_)
                : size(size_)
                , namespace_(namespace__)
                , value(value_)
            {}
        };
        struct block_mdat
        {
            int64u          File_Offset_Begin;
            int64u          File_Offset_End; //Excluded

            block_mdat()
            {
                File_Offset_Begin=(int64u)-1;
                File_Offset_End=0;
            }
        };
        struct block_moov
        {
            int64u          File_Offset;

            block_moov()
            {
                File_Offset=(int64u)-1;
            }
        };
        struct block_moov_meta_ilst
        {
            vector<block_moov_meta_list> Items;

            block_moov_meta_ilst()
            {
            }
        };
        struct block_moov_meta_keys
        {
            vector<block_moov_meta_key> Keys;

            block_moov_meta_keys()
            {
            }
        };
        struct block_strings
        {
            map<string, string> Strings;
            map<string, vector<string> > Histories;
        };
        struct buffer
        {
            int8u*  Data;
            size_t  Size;
            size_t  Size_Maximum;

            buffer()
            {
                Data=NULL;
                Size=0;
                Size_Maximum=0;
            }
        };

        File                In;
        File                Out;
        buffer              Out_Buffer_Begin;
        buffer              Out_Buffer_End;
        bool                Out_Buffer_WriteAtEnd;

        Ztring              File_Name;
        int64u              File_Size;
        Ztring              File_Date;
        stringstream        Trace;
        block_mdat         *mdat;
        vector<block_moov*> moov;
        block_moov_meta_ilst* moov_meta_ilst;
        block_moov_meta_keys* moov_meta_keys;
        vector<string>      moov_meta_keys_NewKeys;
        size_t              moov_meta_keys_AlreadyPresent;
        vector<string>      moov_meta_ilst_NewValues;
        size_t              moov_meta_ilst_AlreadyPresent;
        bool                NewChunksAtTheEnd;
        bool                Out_Buffer_File_TryModification;
        bool                Out_Buffer_File_IsModified;

        CriticalSection     CS;
        float               Progress;
        bool                Canceling;

        global()
        {
            File_Size=0;
            mdat=NULL;
            moov_meta_ilst=NULL;
            moov_meta_keys=NULL;
            moov_meta_keys_AlreadyPresent=0;
            moov_meta_ilst_AlreadyPresent=0;
            NewChunksAtTheEnd=false;
            Out_Buffer_WriteAtEnd=false;
            Out_Buffer_File_TryModification=true;
            Out_Buffer_File_IsModified=false;
            Progress=0;
            Canceling=false;
        }

        ~global()
        {
            delete mdat;
            delete moov_meta_ilst;
            delete moov_meta_keys;
        }
    };

    //---------------------------------------------------------------------------
    //Chunk specific
    struct block
    {
        struct header
        {
            size_t  Level;
            int8u   Size;
            int32u  Name;
            
            header()
            {
                Level=0;
                Size=0;
                Name=0x00000000;
            }
        };
        struct content
        {
            int8u*  Buffer;
            size_t  Buffer_Offset; //Internal use
            int64u  Size; //Header excluded
            bool    IsModified;
            bool    IsRemovable;
            bool    Size_IsModified;
            
            content()
            {
                Buffer=NULL;
                Buffer_Offset=0;
                Size=0;
                IsModified=false;
                IsRemovable=false;
                Size_IsModified=false;
            }

            ~content()
            {
                delete[] Buffer; //Buffer=NULL;
            }   
        };

        int64u  File_In_Position;
        header  Header;
        content Content;

        block()
        {
            File_In_Position=(int64u)-1;
        }
    };

    //***************************************************************************
    // Interface
    //***************************************************************************

    //---------------------------------------------------------------------------
    //Constructor/Destructor
             mp4_Base           (global* Global, size_t Level);
    virtual ~mp4_Base           ();
    
    //---------------------------------------------------------------------------
    //Read/Write
    void Read                   (block &Chunk_In);
    void Modify                 (int32u Chunk_Name_1, int32u Chunk_Name_2, int32u Chunk_Name_3);
    void Modify                 ()                                              {Modify_Internal();};
    void Write                  ();

    //---------------------------------------------------------------------------
    //Data
    int64u Block_Size_Get       ();
    int64u Block_Size_Get       (int32u Element);
    size_t Subs_Pos_Get         (int32u Element);
    bool   Read_Header          (block &NewChunk);
    bool   IsModified           ()                                              {return Chunk.Content.IsModified;};
    void   IsModified_Clear     ()                                              {Chunk.Content.IsModified=false;};
    bool   IsRemovable          ()                                              {return Chunk.Content.IsRemovable;};
    int32u Header_Name_Get      ()                                              {return Chunk.Header.Name;};
    void   Header_Name_Set      (int32u Name)                                   {Chunk.Header.Name=Name;};

public: //protected :
    //***************************************************************************
    // Buffer handling (virtual)
    //***************************************************************************

    //---------------------------------------------------------------------------
    //Read/Write
    virtual void    Read_Internal       ();
    virtual void    Modify_Internal     ()                                      {}
    virtual size_t  Insert_Internal     (int32u)                                {return Subs.size();}
    virtual void    Write_Internal      ()                                      ;
    void            Write_Internal      (const int8u* Buffer, size_t Buffer_Size);

    //***************************************************************************
    // Buffer handling (buffer read/write)
    //***************************************************************************

    //---------------------------------------------------------------------------
    //Buffer handling - Big endian values
    void Get_B1     (int8u  &Value);
    void Put_B1     (int8u   Value);
    void Get_B2     (int16u &Value);
    void Put_B2     (int16u  Value);
    void Get_B3     (int32u &Value);
    void Put_B3     (int32u  Value);
    void Get_B4     (int32u &Value);
    void Put_B4     (int32u  Value);
    void Get_B8     (int64u &Value);
    void Put_B8     (int64u  Value);
    void Get_B16    (int128u &Value);
    void Put_B16    (int128u  Value);

    //---------------------------------------------------------------------------
    //Buffer handling - Little endian values
    void Get_L1     (int8u  &Value);
    void Put_L1     (int8u   Value);
    void Get_L2     (int16u &Value);
    void Put_L2     (int16u  Value);
    void Get_L3     (int32u &Value);
    void Put_L3     (int32u  Value);
    void Get_L4     (int32u &Value);
    void Put_L4     (int32u  Value);
    void Get_L8     (int64u &Value);
    void Put_L8     (int64u  Value);
    void Get_L16    (int128u &Value);
    void Put_L16    (int128u  Value);

    //---------------------------------------------------------------------------
    //Buffer handling - Character codes
    void Get_C4     (int32u &Value);
    void Put_C4     (int32u  Value);

    //---------------------------------------------------------------------------
    //Buffer handling - UUID
    void Get_UUID   (int128u &Value);
    void Put_UUID   (int128u  Value);
    void Skip_UUID  ();

    //---------------------------------------------------------------------------
    //Buffer handling - Stirngs
    void Get_String (int64u Value_Size, string &Value);
    void Put_String (int64u Value_Size, string Value);

    //---------------------------------------------------------------------------
    //Buffer handling - Other
    void Skip_XX    (int64u Value_Size);

    //***************************************************************************
    // Buffer handling (internal)
    //***************************************************************************

    //---------------------------------------------------------------------------
    //Read/Write helpers
    void Read_Internal_ReadAllInBuffer  ();
    void Modify_Internal_Subs           (int32u Chunk_Name_0, int32u Chunk_Name_1, int32u Chunk_Name_2);
    void Write_Internal_Subs            ();

    //***************************************************************************
    // Data
    //***************************************************************************

    //---------------------------------------------------------------------------
    //Subs
    std::vector<mp4_Base*> Subs;

    //---------------------------------------------------------------------------
    //Global
    global *Global;

    //---------------------------------------------------------------------------
    //Chunk
    block Chunk;

    //---------------------------------------------------------------------------
    //Friends
    friend class _Handler;
};

//***************************************************************************
// Defines
//***************************************************************************

#define SUBS_BEGIN() \
    while (Global->In.Position_Get()<Chunk.File_In_Position+Chunk.Header.Size+Chunk.Content.Size) \
    { \
        block NewChunk; \
        NewChunk.Header.Level=Chunk.Header.Level+1; \
        if (!Read_Header(NewChunk)) \
            return; \
        \
        mp4_Base* Sub=NULL; \
        switch (NewChunk.Header.Name) \
        { \

#define SUB_ELEMENT(_Name) \
            case Elements::_Name : Sub=new mp4_##_Name(Global); break; \

#define SUB_ELEMENT_DEFAULT(_Name) \
            default : Sub=new mp4_##_Name(Global); break; \

#define SUBS_END() \
            default             : Sub=new mp4_Base(Global, Chunk.Header.Level+1); \
        SUBS_END_DEFAULT();

#define SUBS_END_DEFAULT() \
        } \
         \
        if (Sub) \
        { \
            Sub->Read(NewChunk); \
            Subs.push_back(Sub); \
            if (NewChunk.Content.IsModified) \
                Chunk.Content.IsModified=true; \
            if (NewChunk.Content.Size_IsModified) \
                Chunk.Content.Size_IsModified=true; \
        } \
    } \

#endif
