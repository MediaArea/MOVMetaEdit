#!/bin/bash

# To update the md5sums, run make check with GENERATE=1 as environment

PATH="${PWD}:$PATH"
path_script="${PWD}/test"

rcode=0

fd=1
if command exec >&9 ; then
    fd=9
fi >/dev/null 2>&1

valgrind=""
if command -v valgrind ; then
    valgrind="valgrind --quiet --log-file=valgrind.log"
fi >/dev/null 2>&1

pushd "${path_script}/TestingFiles/MOV/For MOVMetaEdit"
    rm -fr Files.bak
    cp -fr Files Files.bak

    for test in ${path_script}/test*.txt ; do
        name=$(basename -s.txt "${test}")
        md5file=${path_script}/${name}.md5sums
        echo ${name} >&${fd}

        while read line ; do
            eval "${valgrind} ${line}"

            if [ -z "${GENERATE}" ] && [ -s "valgrind.log" ] ; then
                cat valgrind.log >&${fd}
                rcode=1
            fi
        done < "${test}"

        if [ ! -z "${GENERATE}" ] ; then
            md5sum Files/*/* > "${md5file}"
        elif ! md5sum -c "${md5file}" >&${fd} ; then
            rcode=1
        fi

        rm -fr Files
        cp -fr Files.bak Files
    done
popd

exit ${rcode}

