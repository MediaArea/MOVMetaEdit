/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifndef mp4_ChunksH
#define mp4_ChunksH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Common/mp4_Base.h"
//---------------------------------------------------------------------------

//***************************************************************************
// 
//***************************************************************************

class mp4 : public mp4_Base
{
public:
    //Constructor/Destructor
    mp4();
    ~mp4();

protected:
    //Read/Write
    void Read_Internal();
};

//***************************************************************************
// Constants
//***************************************************************************

namespace Elements
{
    const uint32_t free = 0x66726565;

    const uint32_t mdat = 0x6D646174;
    const uint32_t moov = 0x6D6F6F76;
    const uint32_t moov_trak = 0x7472616B;
    const uint32_t moov_trak_tapt = 0x74617074;
    const uint32_t moov_trak_tapt_clef = 0x636C6566;
    const uint32_t moov_trak_tapt_prof = 0x70726F66;
    const uint32_t moov_trak_tapt_enof = 0x656E6F66;
    const uint32_t moov_trak_mdia = 0x6D646961;
    const uint32_t moov_trak_mdia_minf = 0x6D696E66;
    const uint32_t moov_trak_mdia_minf_stbl = 0x7374626C;
    const uint32_t moov_trak_mdia_minf_stbl_stsd = 0x73747364;
    const uint32_t moov_trak_mdia_minf_stbl_stsd_xxxxVideo = 0xFFFFFFFF;
    const uint32_t moov_trak_mdia_minf_stbl_stsd_xxxx_clap = 0x636C6170;
    const uint32_t moov_trak_mdia_minf_stbl_stsd_xxxx_colr = 0x636F6C72;
    const uint32_t moov_trak_mdia_minf_stbl_stsd_xxxx_fiel = 0x6669656C;
    const uint32_t moov_trak_mdia_minf_stbl_stsd_xxxx_gama = 0x67616D61;
    const uint32_t moov_trak_mdia_minf_stbl_stsd_xxxx_pasp = 0x70617370;
    const uint32_t moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv = 0x6D646376;
    const uint32_t moov_trak_mdia_minf_stbl_stsd_xxxx_clli = 0x636C6C69;
    const uint32_t moov_trak_mdia_minf_stbl_stsd_xxxxSound = 0xFFFFFFFF;
    const uint32_t moov_trak_mdia_minf_stbl_stsd_xxxx_chan = 0x6368616E;
    const uint32_t moov_trak_mdia_minf_vmhd = 0x766D6864;
    const uint32_t moov_trak_mdia_minf_smhd = 0x736D6864;
    const uint32_t moov_trak_mdia_mdhd = 0x6D646864;
    const uint32_t moov_trak_tkhd = 0x746B6864;
    const uint32_t moov_meta = 0x6D657461;
    const uint32_t moov_meta_hdlr = 0x68646C72;
    const uint32_t moov_meta_keys = 0x6B657973;
    const uint32_t moov_meta_ilst = 0x696C7374;
}

//***************************************************************************
// List of blocks
//***************************************************************************

//Chunk without modification
#define CHUNK__(_Level, _Name) \
class mp4_##_Name : public mp4_Base \
{ \
public: \
    mp4_##_Name(global* Global):mp4_Base(Global, _Level) {Chunk.Header.Name=Elements::_Name;} \
protected: \
    void Read_Internal(); \
}; \

//Chunk with insertion
#define CHUNK_I(_Level, _Name) \
class mp4_##_Name : public mp4_Base \
{ \
public: \
    mp4_##_Name(global* Global):mp4_Base(Global, _Level) {Chunk.Header.Name=Elements::_Name;} \
protected: \
    void Read_Internal(); \
    size_t Insert_Internal(int32u Chunk_Name_Insert); \
}; \

//Chunk with modification/write
#define CHUNK_W(_Level, _Name) \
class mp4_##_Name : public mp4_Base \
{ \
public: \
    mp4_##_Name(global* Global):mp4_Base(Global, _Level) {Chunk.Header.Name=Elements::_Name;} \
protected: \
    void Read_Internal(); \
    void Modify_Internal(); \
    void Write_Internal(); \
}; \

//Chunk with modification/insertion/write
#define CHUNK_M(_Level, _Name) \
class mp4_##_Name : public mp4_Base \
{ \
public: \
    mp4_##_Name(global* Global):mp4_Base(Global, _Level) {Chunk.Header.Name=Elements::_Name;} \
protected: \
    void Read_Internal(); \
    void Modify_Internal(); \
    size_t Insert_Internal(int32u Chunk_Name_Insert); \
    void Write_Internal(); \
}; \

CHUNK_W(1, mdat);
CHUNK_W(1, free);
CHUNK_I(1, moov);
CHUNK_I(2, moov_trak);
CHUNK_I(3, moov_trak_tapt);
CHUNK_W(4, moov_trak_tapt_clef);
CHUNK_W(4, moov_trak_tapt_prof);
CHUNK_W(4, moov_trak_tapt_enof);
CHUNK__(3, moov_trak_mdia);
CHUNK__(4, moov_trak_mdia_minf);
CHUNK__(5, moov_trak_mdia_minf_stbl);
CHUNK__(6, moov_trak_mdia_minf_stbl_stsd);
CHUNK_M(7, moov_trak_mdia_minf_stbl_stsd_xxxxVideo);
CHUNK_W(8, moov_trak_mdia_minf_stbl_stsd_xxxx_clap);
CHUNK_W(8, moov_trak_mdia_minf_stbl_stsd_xxxx_colr);
CHUNK_W(8, moov_trak_mdia_minf_stbl_stsd_xxxx_fiel);
CHUNK_W(8, moov_trak_mdia_minf_stbl_stsd_xxxx_gama);
CHUNK_W(8, moov_trak_mdia_minf_stbl_stsd_xxxx_pasp);
CHUNK_W(8, moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv);
CHUNK_W(8, moov_trak_mdia_minf_stbl_stsd_xxxx_clli);
CHUNK_I(7, moov_trak_mdia_minf_stbl_stsd_xxxxSound);
CHUNK_W(8, moov_trak_mdia_minf_stbl_stsd_xxxx_chan);
CHUNK__(5, moov_trak_mdia_minf_vmhd);
CHUNK__(5, moov_trak_mdia_minf_smhd);
CHUNK_W(4, moov_trak_mdia_mdhd);
CHUNK_W(3, moov_trak_tkhd);
CHUNK_I(2, moov_meta);
CHUNK_W(3, moov_meta_hdlr);
CHUNK_W(3, moov_meta_ilst);
CHUNK_W(3, moov_meta_keys);

#endif
