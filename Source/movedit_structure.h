/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a MIT-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#include "movedit_atomread.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include "ZenLib/Dir.h"
#include "ZenLib/File.h"
using namespace std;
using namespace ZenLib;


namespace Element
{

    const uint32_t chan = 0x6368616e;
    const uint32_t clap = 0x636c6170;
    const uint32_t clef = 0x636c6566;
    const uint32_t colr = 0x636F6C72;
    const uint32_t enof = 0x656e6f66;
    const uint32_t fiel = 0x6669656C;
    const uint32_t free = 0x66726565;
    const uint32_t ftyp = 0x66747970;
    const uint32_t gama = 0x67616D61;
    const uint32_t mdat = 0x6D646174;
    const uint32_t mdia = 0x6D646961;
    const uint32_t minf = 0x6D696E66;
    const uint32_t moov = 0x6D6F6F76;
    const uint32_t pasp = 0x70617370;
    const uint32_t prof = 0x70726f66;
    const uint32_t stbl = 0x7374626C;
    const uint32_t stsd = 0x73747364;
    const uint32_t tapt = 0x74617074;
    const uint32_t trak = 0x7472616B;
    const uint32_t tkhd = 0x746B6864;
    const uint32_t vmhd = 0x766D6864;
    const uint32_t wide = 0x77696465;
}

class Structure
{
    atom* Root_;
    File* F;

public:
    Structure  (File* F, const Ztring &Name);
    ~Structure ();

    Ztring      Name;
    bool        IsOk;
    bool        mdatIsPresent;
    bool        moovIsPresent;
    bool        freeIsPresent;
    uint64_t    moovOffsetMax;
    uint64_t    mdatOffset;
    struct track_struct
    {

        struct cpe_struct
        {
            uint8_t     version;
            uint32_t    width;
            uint32_t    height;
            uint64_t    Offset;

            cpe_struct()
                :
                version(0),
                width(0),
                height(0),
                Offset((uint64_t)-1)
            {
            }
        };

        struct video_struct
        {
            struct clap_struct
            {
                uint32_t    aperture_width_n;
                uint32_t    aperture_width_d;
                uint32_t    aperture_height_n;
                uint32_t    aperture_height_d;
                uint32_t    orizontal_offset_n;
                uint32_t    orizontal_offset_d;
                uint32_t    vertical_offset_n;
                uint32_t    vertical_offset_d;
                uint64_t    Offset;

                clap_struct()
                    :
                    aperture_width_n(0),
                    aperture_width_d(0),
                    aperture_height_n(0),
                    aperture_height_d(0),
                    orizontal_offset_n(0),
                    orizontal_offset_d(0),
                    vertical_offset_n(0),
                    vertical_offset_d(0),
                    Offset((uint64_t)-1)
                {
                }
            };
            struct fiel_struct
            {
                uint8_t    fields;
                uint8_t    detail;
                uint64_t    Offset;

                fiel_struct()
                    :
                    fields(0),
                    detail(0),
                    Offset((uint64_t)-1)
                {
                }
            };
            struct pasp_struct
            {
                uint32_t    h;
                uint32_t    v;
                uint64_t    Offset;

                pasp_struct()
                    :
                    h(0),
                    v(0),
                    Offset((uint64_t)-1)
                {
                }
            };
            struct colr_struct
            {
                uint32_t    type;
                uint16_t    primary;
                uint16_t    transfer;
                uint16_t    matrix;
                uint64_t    Offset;

                colr_struct()
                    :
                    type(0),
                    primary(0),
                    transfer(0),
                    matrix(0),
                    Offset((uint64_t)-1)
                {
                }
            };
            struct gama_struct
            {
                double      gamma;
                uint64_t    Offset;

                gama_struct()
                    :
                    gamma(0.0f),
                    Offset((uint64_t)-1)
                {
                }
            };
            vector<uint64_t> UpOffsets;
            vector<uint64_t> UpTotalSizes;
            clap_struct clap;
            fiel_struct fiel;
            pasp_struct pasp;
            colr_struct colr;
            gama_struct gama;

            video_struct()
            {
            }
        };

        bool        IsVideo;
        uint16_t    Width;
        uint16_t    Height;
        double      WidthScale;
        uint64_t    WidthScalePos;
        cpe_struct  clef;
        cpe_struct  prof;
        cpe_struct  enof;
        typedef std::vector<video_struct> videos;
        videos      Videos;

        track_struct()
            :
            IsVideo(false),
            Width(0),
            Height(0),
            WidthScale(0),
            WidthScalePos((uint64_t)-1)
        {

        }

    };
    typedef std::vector<track_struct> tracks;
    tracks      Tracks;
    uint64_t    freeOffset;
    uint64_t    freeTotalSize;

    //Temporary references
    track_struct* Track;
    track_struct::video_struct* Video;

    void Parse ();

    void free                                                   (atom* Up);
    void mdat                                                   (atom* Up);
    void moov                                                   (atom* Up);
    void moov_trak                                              (atom* Up);
    void moov_trak_tapt                                         (atom* Up);
    void moov_trak_tapt_clef                                    (atom* Up);
    void moov_trak_tapt_prof                                    (atom* Up);
    void moov_trak_tapt_enof                                    (atom* Up);
    void moov_trak_tkhd                                         (atom* Up);
    void moov_trak_mdia                                         (atom* Up);
    void moov_trak_mdia_minf                                    (atom* Up);
    void moov_trak_mdia_minf_stbl                               (atom* Up);
    void moov_trak_mdia_minf_stbl_stsd                          (atom* Up);
    void moov_trak_mdia_minf_stbl_stsd_xxxxVideo                (atom* Up);
    void moov_trak_mdia_minf_stbl_stsd_xxxx_chan                (atom* Up);
    void moov_trak_mdia_minf_stbl_stsd_xxxx_clap                (atom* Up);
    void moov_trak_mdia_minf_stbl_stsd_xxxx_fiel                (atom* Up);
    void moov_trak_mdia_minf_stbl_stsd_xxxx_pasp                (atom* Up);
    void moov_trak_mdia_minf_stbl_stsd_xxxx_colr                (atom* Up);
    void moov_trak_mdia_minf_stbl_stsd_xxxx_gama                (atom* Up);
    void moov_trak_mdia_minf_vmhd                               (atom* Up);
};
