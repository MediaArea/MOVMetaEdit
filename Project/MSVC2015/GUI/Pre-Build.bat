if exist %1 (
  pushd ..\..\Qt
    %1\bin\qmake movmetaedit-gui.pro 2> nul
    nmake /NOLOGO /f Makefile.Release compiler_uic_make_all compiler_rcc_make_all mocables
  popd
) else (
  echo Missing Qt directory %1. Correct the Qt path in project properties: Include Directories, Library Directories, Pre-Build, Post-Build.
)
