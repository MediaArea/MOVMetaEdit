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

CHUNK_W(1, mdat);
CHUNK_W(1, free);
CHUNK_I(1, moov);
CHUNK_I(2, moov_meta);
CHUNK_W(3, moov_meta_hdlr);
CHUNK_W(3, moov_meta_ilst);
CHUNK_W(3, moov_meta_keys);

#endif
