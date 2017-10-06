#!/bin/bash

# To update the md5sums, run make check with GENERATE=1 as environment

PATH="${PWD}:$PATH"
path_script="${PWD}/test"

rcode=0

pushd "${path_script}/TestingFiles/MOV/For MOVMetaEdit"
    rm -fr Files.bak
    cp -fr Files Files.bak

    for test in ${path_script}/test*.txt ; do
        echo $(basename -s.txt "${test}") >&9
        md5file=${path_script}/$(basename -s.txt "${test}").md5sums

        while read line ; do
            eval ${line}
        done < "${test}"

        if [ ! -z "${GENERATE}" ] ; then
            md5sum Files/*/* > "${md5file}"
        elif ! md5sum -c "${md5file}" >&9 ; then
            rcode=1
        fi

        rm -fr Files
        cp -fr Files.bak Files
    done
popd

exit ${rcode}

