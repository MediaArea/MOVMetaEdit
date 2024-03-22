AUTOMAKE_OPTIONS = foreign subdir-objects

bin_PROGRAMS = movmetaedit
movmetaedit_SOURCES = \
    ../../../Source/CLI/Help.cpp \
    ../../../Source/Common/AdID.cpp \
    ../../../Source/Common/mp4/mp4_.cpp \
    ../../../Source/Common/mp4/mp4_mdat.cpp \
    ../../../Source/Common/mp4/mp4_free.cpp \
    ../../../Source/Common/mp4/mp4_moov.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_tapt.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_tapt_xxxx.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxxVideo.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_clap.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_colr.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_fiel.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_gama.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_pasp.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_clli.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxxSound.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_chan.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxxUnknown.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_vmhd.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_smhd.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_gmhd.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_minf_gmhd_tmcd.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_mdia_mdhd.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_tkhd.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_tref.cpp \
    ../../../Source/Common/mp4/mp4_moov_trak_tref_tmcd.cpp \
    ../../../Source/Common/mp4/mp4_moov_meta.cpp \
    ../../../Source/Common/mp4/mp4_moov_meta_hdlr.cpp \
    ../../../Source/Common/mp4/mp4_moov_meta_ilst.cpp \
    ../../../Source/Common/mp4/mp4_moov_meta_keys.cpp \
    ../../../Source/Common/mp4_Base.cpp \
    ../../../Source/Common/mp4_Base_parsing.cpp \
    ../../../Source/Common/mp4_Handler.cpp \
    ../../../Source/movedit_structure.cpp \
    ../../../Source/movedit_atomread.cpp \
    ../../../Source/movedit_main.cpp \
    ../../../Source/ZenLib/Conf.cpp \
    ../../../Source/ZenLib/CriticalSection.cpp \
    ../../../Source/ZenLib/Dir.cpp \
    ../../../Source/ZenLib/File.cpp \
    ../../../Source/ZenLib/FileName.cpp \
    ../../../Source/ZenLib/InfoMap.cpp \
    ../../../Source/ZenLib/MemoryDebug.cpp \
    ../../../Source/ZenLib/OS_Utils.cpp \
    ../../../Source/ZenLib/PreComp.cpp \
    ../../../Source/ZenLib/Thread.cpp \
    ../../../Source/ZenLib/Translation.cpp \
    ../../../Source/ZenLib/Utils.cpp \
    ../../../Source/ZenLib/Ztring.cpp \
    ../../../Source/ZenLib/ZtringList.cpp \
    ../../../Source/ZenLib/ZtringListList.cpp \
    ../../../Source/ZenLib/ZtringListListF.cpp \
    ../../../Source/ZenLib/int128s.cpp \
    ../../../Source/ZenLib/int128u.cpp \
    ../../../Source/ThirdParty/tfsxml/tfsxml.c

AM_CXXFLAGS = -std=c++0x -I../../../Source

AM_TESTS_FD_REDIRECT = 9>&2

TESTS = test/test.sh

TESTING_DIR = test/TestingFiles

clone_testing_files: $(TESTING_DIR)

$(TESTING_DIR):
	git clone https://github.com/MediaArea/MediaArea-RegressionTestingFiles test/TestingFiles
