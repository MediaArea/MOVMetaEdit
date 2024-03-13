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
#include <tuple>
#include <array>
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
        struct block_moov_meta_hdlr
        {
            bool Present;

            block_moov_meta_hdlr()
            {
                Present=false;
            }
        };
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
                , ToBeReplacedBy_Modified(false)
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
        struct block_moov_trak
        {
            bool IsVideo;
            bool IsSound;
            bool moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present;
            bool moov_trak_mdia_minf_stbl_stsd_xxxxSound_Present;

            block_moov_trak()
            {
                IsVideo=false;
                IsSound=false;
                moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Present=false;
                moov_trak_mdia_minf_stbl_stsd_xxxxSound_Present=false;
            }
        };
        struct block_moov_trak_tapt_xxxx
        {
            double Width;
            double Height;

            block_moov_trak_tapt_xxxx()
            {
                Width=0;
                Height=0;
            }
        };
        struct block_moov_trak_mdia_minf_stbl_stsd_xxxxVideo
        {
            int64u   Reserved;
            int16u   DataReferenceIndex;
            int16u   Version;
            int16u   RevisionLevel;
            int32u   Vendor;
            int32u   TemporalQuality;
            int32u   SpatialQuality;
            int16u   Width;
            int16u   Height;
            int32u   HorizontalResolution;
            int32u   VerticalResolution;
            int32u   DataSize;
            int16u   FrameCount;
            string   CompressorName;
            int16u   Depth;
            int16u   ColorTableID;
            block_moov_trak_mdia_minf_stbl_stsd_xxxxVideo()
            {
                Reserved=0;
                DataReferenceIndex=0;
                Version=0;
                RevisionLevel=0;
                Vendor=0;
                TemporalQuality=0;
                SpatialQuality=0;
                Width=0;
                Height=0;
                HorizontalResolution=0;
                VerticalResolution=0;
                DataSize=0;
                FrameCount=0;
                Depth=0;
                ColorTableID=0;
            }
        };
        struct block_moov_trak_mdia_minf_stbl_stsd_xxxx_fiel
        {
            uint8_t    Fields;
            uint8_t    Detail;

            block_moov_trak_mdia_minf_stbl_stsd_xxxx_fiel()
            {
                Fields=0;
                Detail=0;
            }
        };
        struct block_moov_trak_mdia_minf_stbl_stsd_xxxx_colr
        {
            uint16_t    Primaries;
            uint16_t    Transfer;
            uint16_t    Matrix;

            block_moov_trak_mdia_minf_stbl_stsd_xxxx_colr()
            {
                Primaries=0;
                Transfer=0;
                Matrix=0;
            }
        };
        struct block_moov_trak_mdia_minf_stbl_stsd_xxxx_gama
        {
            double    Gamma;

            block_moov_trak_mdia_minf_stbl_stsd_xxxx_gama()
            {
                Gamma=0;
            }
        };
        struct block_moov_trak_mdia_minf_stbl_stsd_xxxx_pasp
        {
            uint32_t hSpacing;
            uint32_t vSpacing;

            block_moov_trak_mdia_minf_stbl_stsd_xxxx_pasp()
            {
                hSpacing=0;
                vSpacing=0;
            }
        };
        struct block_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv
        {
            std::array<double, 8> Primaries;
            std::array<double, 2> Luminance;

            block_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv()
            {
                Primaries.fill(0);
                Luminance.fill(-1);
            }
        };

        struct block_moov_trak_mdia_minf_stbl_stsd_xxxx_clli
        {
            double maximum_content_light_level;
            double maximum_frame_average_light_level;

            block_moov_trak_mdia_minf_stbl_stsd_xxxx_clli()
            {
                maximum_content_light_level=0;
                maximum_frame_average_light_level=0;
            }
        };

        struct block_moov_trak_mdia_minf_stbl_stsd_xxxx_clap
        {
            uint32_t Aperture_Width_Num;
            uint32_t Aperture_Width_Den;
            uint32_t Aperture_Height_Num;
            uint32_t Aperture_Height_Den;
            uint32_t Horizontal_Offset_Num;
            uint32_t Horizontal_Offset_Den;
            uint32_t Vertical_Offset_Num;
            uint32_t Vertical_Offset_Den;

            block_moov_trak_mdia_minf_stbl_stsd_xxxx_clap()
            {
                Aperture_Width_Num=0;
                Aperture_Width_Den=0;
                Aperture_Height_Num=0;
                Aperture_Height_Den=0;
                Horizontal_Offset_Num=0;
                Horizontal_Offset_Den=0;
                Vertical_Offset_Num=0;
                Vertical_Offset_Den=0;
            }
        };
        struct block_moov_trak_mdia_minf_stbl_stsd_xxxx_chan
        {
            struct description
            {
                uint32_t ChannelLabel;
                uint32_t ChannelFlags;
                uint32_t Coordinates0;
                uint32_t Coordinates1;
                uint32_t Coordinates2;

                description()
                {
                    ChannelLabel=0;
                    ChannelFlags=0;
                    Coordinates0=0;
                    Coordinates1=0;
                    Coordinates2=0;
                }
            };

            uint8_t Version;
            uint32_t Flags;
            uint32_t ChannelLayoutTag;
            uint32_t ChannelBitmap;
            uint32_t NumberChannelDescriptions;
            vector<description> Descriptions;

            block_moov_trak_mdia_minf_stbl_stsd_xxxx_chan()
            {
                Version=0;
                Flags=0;
                ChannelLayoutTag=0;
                ChannelBitmap=0;
                NumberChannelDescriptions=0;
            }
        };
        struct block_moov_trak_mdia_mdhd
        {
            int8u Version;
            int32u Flags;
            int64u CreationTime;
            int64u ModificationTime;
            int32u TimeScale;
            int64u Duration;
            int16u Language;
            int16u Quality;

            block_moov_trak_mdia_mdhd()
            {
                Version=0;
                Flags=0;
                CreationTime=0;
                ModificationTime=0;
                TimeScale=0;
                Duration=0;
                Language=0;
                Quality=0;
            }
        };
        struct block_moov_trak_tkhd
        {
            double Width_Scale;
            size_t Width_Scale_Pos;

            block_moov_trak_tkhd()
            {
                Width_Scale=0;
                Width_Scale_Pos=0;
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

        #ifdef MACSTORE
        Ztring              Temp_Path;
        Ztring              Temp_Name;
        #endif
        Ztring              File_Name;
        int64u              File_Size;
        Ztring              File_Date;
        stringstream        Trace;
        block_mdat         *mdat;
        vector<block_moov*> moov;
        vector<block_moov_trak*> moov_trak;
        map<size_t, block_moov_trak_tapt_xxxx*> moov_trak_tapt_clef;
        bool                                    moov_trak_tapt_clef_Modified; // For FirstVideoIndex
        map<size_t, block_moov_trak_tapt_xxxx*> moov_trak_tapt_prof;
        bool                                    moov_trak_tapt_prof_Modified; // For FirstVideoIndex
        map<size_t, block_moov_trak_tapt_xxxx*> moov_trak_tapt_enof;
        bool                                    moov_trak_tapt_enof_Modified; // For FirstVideoIndex
        map<size_t, block_moov_trak_mdia_minf_stbl_stsd_xxxxVideo*> moov_trak_mdia_minf_stbl_stsd_xxxxVideo;
        bool                                                        moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Modified; // For FirstVideoIndex
        map<size_t, block_moov_trak_mdia_minf_stbl_stsd_xxxx_clap*> moov_trak_mdia_minf_stbl_stsd_xxxx_clap;
        bool                                                        moov_trak_mdia_minf_stbl_stsd_xxxx_clap_Modified; // For FirstVideoIndex
        map<size_t, block_moov_trak_mdia_minf_stbl_stsd_xxxx_colr*> moov_trak_mdia_minf_stbl_stsd_xxxx_colr;
        bool                                                        moov_trak_mdia_minf_stbl_stsd_xxxx_colr_Modified; // For FirstVideoIndex
        map<size_t, block_moov_trak_mdia_minf_stbl_stsd_xxxx_fiel*> moov_trak_mdia_minf_stbl_stsd_xxxx_fiel;
        bool                                                        moov_trak_mdia_minf_stbl_stsd_xxxx_fiel_Modified; // For FirstVideoIndex
        map<size_t, block_moov_trak_mdia_minf_stbl_stsd_xxxx_gama*> moov_trak_mdia_minf_stbl_stsd_xxxx_gama;
        bool                                                        moov_trak_mdia_minf_stbl_stsd_xxxx_gama_Modified; // For FirstVideoIndex
        map<size_t, block_moov_trak_mdia_minf_stbl_stsd_xxxx_pasp*> moov_trak_mdia_minf_stbl_stsd_xxxx_pasp;
        bool                                                        moov_trak_mdia_minf_stbl_stsd_xxxx_pasp_Modified; // For FirstVideoIndex
        map<size_t, block_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv*> moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv;
        bool                                                        moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv_Modified; // For FirstVideoIndex
        map<size_t, block_moov_trak_mdia_minf_stbl_stsd_xxxx_clli*> moov_trak_mdia_minf_stbl_stsd_xxxx_clli;
        bool                                                        moov_trak_mdia_minf_stbl_stsd_xxxx_clli_Modified; // For FirstVideoIndex
        map<size_t, block_moov_trak_mdia_minf_stbl_stsd_xxxx_chan*> moov_trak_mdia_minf_stbl_stsd_xxxx_chan;
        bool                                                        moov_trak_mdia_minf_stbl_stsd_xxxx_chan_Modified;
        map<size_t, block_moov_trak_mdia_mdhd*> moov_trak_mdia_mdhd;
        bool                                    moov_trak_mdia_mdhd_Modified;
        block_moov_trak_tkhd* moov_trak_tkhd;
        bool                  moov_trak_tkhd_Modified;
        block_moov_meta_hdlr* moov_meta_hdlr;
        bool                  moov_meta_hdlr_Modified;
        block_moov_meta_ilst* moov_meta_ilst;
        bool                  moov_meta_ilst_Modified;
        block_moov_meta_keys* moov_meta_keys;
        bool                  moov_meta_keys_Modified;
        vector<string>      moov_meta_keys_NewKeys;
        size_t              moov_meta_keys_AlreadyPresent;
        vector<string>      moov_meta_ilst_NewValues;
        size_t              moov_meta_ilst_AlreadyPresent;
        size_t              moov_trak_FirstVideoIndex;
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
            moov_trak_tapt_clef_Modified=false;
            moov_trak_tapt_prof_Modified=false;
            moov_trak_tapt_enof_Modified=false;
            moov_trak_mdia_minf_stbl_stsd_xxxxVideo_Modified=false;
            moov_trak_mdia_minf_stbl_stsd_xxxx_clap_Modified=false;
            moov_trak_mdia_minf_stbl_stsd_xxxx_colr_Modified=false;
            moov_trak_mdia_minf_stbl_stsd_xxxx_fiel_Modified=false;
            moov_trak_mdia_minf_stbl_stsd_xxxx_gama_Modified=false;
            moov_trak_mdia_minf_stbl_stsd_xxxx_pasp_Modified=false;
            moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv_Modified=false;
            moov_trak_mdia_minf_stbl_stsd_xxxx_clli_Modified=false;
            moov_trak_mdia_minf_stbl_stsd_xxxx_chan_Modified=false;
            moov_trak_mdia_mdhd_Modified=false;
            moov_trak_tkhd=NULL;
            moov_trak_tkhd_Modified=false;
            moov_meta_hdlr=NULL;
            moov_meta_hdlr_Modified=false;
            moov_meta_ilst=NULL;
            moov_meta_ilst_Modified=false;
            moov_meta_keys=NULL;
            moov_meta_keys_Modified=false;
            moov_meta_keys_AlreadyPresent=0;
            moov_meta_ilst_AlreadyPresent=0;
            moov_trak_FirstVideoIndex=(size_t)-1;
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
            delete moov_trak_tkhd;
            delete moov_meta_hdlr;
            delete moov_meta_ilst;
            delete moov_meta_keys;

            for (size_t Pos=0; Pos<moov_trak.size(); Pos++)
                delete moov_trak[Pos];

            for (size_t Pos=0; Pos<moov_trak_tapt_clef.size(); Pos++)
                delete moov_trak_tapt_clef[Pos];

            for (size_t Pos=0; Pos<moov_trak_tapt_prof.size(); Pos++)
                delete moov_trak_tapt_prof[Pos];

            for (size_t Pos=0; Pos<moov_trak_tapt_enof.size(); Pos++)
                delete moov_trak_tapt_enof[Pos];

            for (size_t Pos=0; Pos<moov_trak_mdia_minf_stbl_stsd_xxxxVideo.size(); Pos++)
                delete moov_trak_mdia_minf_stbl_stsd_xxxxVideo[Pos];

            for (size_t Pos=0; Pos<moov_trak_mdia_minf_stbl_stsd_xxxx_clap.size(); Pos++)
                delete moov_trak_mdia_minf_stbl_stsd_xxxx_clap[Pos];

            for (size_t Pos=0; Pos<moov_trak_mdia_minf_stbl_stsd_xxxx_colr.size(); Pos++)
                delete moov_trak_mdia_minf_stbl_stsd_xxxx_colr[Pos];

            for (size_t Pos=0; Pos<moov_trak_mdia_minf_stbl_stsd_xxxx_fiel.size(); Pos++)
                delete moov_trak_mdia_minf_stbl_stsd_xxxx_fiel[Pos];

            for (size_t Pos=0; Pos<moov_trak_mdia_minf_stbl_stsd_xxxx_gama.size(); Pos++)
                delete moov_trak_mdia_minf_stbl_stsd_xxxx_gama[Pos];

            for (size_t Pos=0; Pos<moov_trak_mdia_minf_stbl_stsd_xxxx_pasp.size(); Pos++)
                delete moov_trak_mdia_minf_stbl_stsd_xxxx_pasp[Pos];

            for (size_t Pos=0; Pos<moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv.size(); Pos++)
                delete moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv[Pos];

            for (size_t Pos=0; Pos<moov_trak_mdia_minf_stbl_stsd_xxxx_clli.size(); Pos++)
                delete moov_trak_mdia_minf_stbl_stsd_xxxx_clli[Pos];

            for (size_t Pos=0; Pos<moov_trak_mdia_minf_stbl_stsd_xxxx_chan.size(); Pos++)
                delete moov_trak_mdia_minf_stbl_stsd_xxxx_chan[Pos];

            for (size_t Pos=0; Pos<moov_trak_mdia_mdhd.size(); Pos++)
                delete moov_trak_mdia_mdhd[Pos];
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
            int64u  Before_Subs_Content_Size;
            int64u  After_Subs_Content_Size;
            bool    IsModified;
            bool    IsRemovable;
            bool    Size_IsModified;
            
            content()
            {
                Buffer=NULL;
                Buffer_Offset=0;
                Size=0;
                Before_Subs_Content_Size=0;
                After_Subs_Content_Size=0;
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
        size_t  trak_Index;

        block()
        {
            File_In_Position=(int64u)-1;
            trak_Index=0;
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
    void Modify                 (int32u Chunk_Name_1,
                                 int32u Chunk_Name_2=0x00000000,
                                 int32u Chunk_Name_3=0x00000000,
                                 int32u Chunk_Name_4=0x00000000,
                                 int32u Chunk_Name_5=0x00000000,
                                 int32u Chunk_Name_6=0x00000000,
                                 int32u Chunk_Name_7=0x00000000,
                                 int32u Chunk_Name_8=0x00000000,
                                 int32u Chunk_Name_9=0x00000000);
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
    void Get_B6     (int64u &Value);
    void Put_B6     (int64u  Value);
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
    void Modify_Internal_Subs           (int32u Chunk_Name_1,
                                         int32u Chunk_Name_2=0x00000000,
                                         int32u Chunk_Name_3=0x00000000,
                                         int32u Chunk_Name_4=0x00000000,
                                         int32u Chunk_Name_5=0x00000000,
                                         int32u Chunk_Name_6=0x00000000,
                                         int32u Chunk_Name_7=0x00000000,
                                         int32u Chunk_Name_8=0x00000000,
                                         int32u Chunk_Name_9=0x00000000);
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
    while (Global->In.Position_Get()+8<Chunk.File_In_Position+Chunk.Header.Size+Chunk.Content.Size) \
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
