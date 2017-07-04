#! /bin/sh

#############################################################################
# Configure
Home=`pwd`

#############################################################################
# Setup for parallel builds
Zen_Make()
{
 if test -e /proc/stat; then
  numprocs=`egrep -c ^cpu[0-9]+ /proc/stat || :`
  if [ "$numprocs" = "0" ]; then
   numprocs=1
  fi
  make -s -j$numprocs
 else
  make
 fi
}

##################################################################
# ZenLib
if test -e ZenLib/Project/GNU/Library/configure; then
    cd ZenLib/Project/GNU/Library/
    test -e Makefile && rm Makefile
    chmod +x configure
    if [ "$OS" = "mac" ]; then
        ./configure --enable-static --disable-shared $MacOptions $ZenLib_Options $*
    else
        ./configure --enable-static --disable-shared $ZenLib_Options $*
    fi
    if test -e Makefile; then
        make clean
        Zen_Make
        if test -e libzen.la; then
            echo ZenLib compiled
        else
            echo Problem while compiling ZenLib
            exit
        fi
    else
        echo Problem while configuring ZenLib
        exit
    fi
else
    echo ZenLib directory is not found
    exit
fi
cd $Home

#############################################################################
# MOV_MetaEdit
if test -e MOV_MetaEdit/Project/GNU/CLI/configure; then
 cd MOV_MetaEdit/Project/GNU/CLI/
 test -e Makefile && rm Makefile
 chmod u+x configure
 ./configure --enable-staticlibs $*
 if test -e Makefile; then
  make clean
  Zen_Make
  if test -e movmetaedit; then
   echo MOV_MetaEdit compiled
  else
   echo Problem while compiling MOV_MetaEdit
   exit
  fi
 else
  echo Problem while configuring MOV_MetaEdit
  exit
 fi
else
 echo MOV_MetaEdit directory is not found
 exit
fi
cd $Home

#############################################################################
# Going home
cd $Home
echo "MOV MetaEdit (CLI) executable is in Project/GNU/CLI"
echo "For installing, cd Project/GNU/CLI && make install"
