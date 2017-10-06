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

#############################################################################
# MOV_MetaEdit
if test -e Project/Qt/movmetaedit-gui.pro; then
 pushd Project/Qt
 test -e Makefile && rm Makefile
 ./prepare
 if test -e Makefile; then
  make clean
  Zen_Make
  if test -e movmetaedit-gui || test -e MOVMetaEdit.app || test -e "MOV MetaEdit.app"; then
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
echo "MOV MetaEdit (GUI) executable is in Project/Qt"
