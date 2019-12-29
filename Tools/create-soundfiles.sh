#!/bin/bash

LANG=${2:-de}
OUT=${1:-mp3}
BASE=$(dirname $0)
TMP=$(mktemp -d)

tts_google() {
    gtts-cli -l ${LANG} -o ${2}.mp3 "${1}"
}

tts_sox() {
    say -v anna "$1" -o ${2}.aiff
    sox ${2}.aiff ${2}.wav pitch 800
    lame -b 128 ${2}.wav ${2}.mp3
}

for i in {1..250}; do
    echo "${i} \"${i}\""
    tts_google "$i" ${TMP}/$(printf "%04d" $i)
done

gcc -I ${BASE}/../Tonuino -E -x c++ -w -P \
    ${BASE}/../Data/sound_files_${LANG}.txt -o ${TMP}/soundfiles.txt

while read f; do
    i=$(echo -ne "$f" | awk -F ';' '{print $1}')
    t=$(echo -ne "$f" | awk -F ';' '{print $2}' | sed -e 's/"//g')
    echo "$i $t"
    tts_google "$t" ${TMP}/$(printf "%04d" $i)
done < <(cat ${TMP}/soundfiles.txt)

mkdir -p ${OUT}
mv ${TMP}/*.mp3 ${OUT}/
