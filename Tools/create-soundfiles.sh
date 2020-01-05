#!/bin/bash

TTS=${3:-tts_gcloud}
LANG=${2:-de}
OUT=${1:-mp3}

LANG_CODE="de-DE"

case $LANG in
    "de")
        LANG_CODE="de-DE"
        ;;
    "en")
        LANG_CODE="en-US"
        ;;
    *)
        echo "unknown language"
        exit -1
        ;;
esac

BASE=$(dirname $0)
TMP=$(mktemp -d)

tts_gcloud() {
    if [[ -z "$GOOGLE_APPLICATION_CREDENTIALS" ]]; then
        echo "need to set GOOGLE_APPLICATION_CREDENTIALS"
        exit -1
    fi

    curl -s -H "Authorization: Bearer $(gcloud auth application-default print-access-token)" \
        -H "Content-Type: application/json; charset=utf-8" \
        --data "{'input':{'text':'${1}'},
        'voice':{
          'languageCode':'${LANG_CODE}',
          'name':'${LANG_CODE}-Wavenet-A',
          'ssmlGender':'FEMALE'
        },
        'audioConfig':{
          'audioEncoding':'MP3'
        }
      }" "https://texttospeech.googleapis.com/v1/text:synthesize" > $2.txt
    cat $2.txt | jq .audioContent | sed -e 's/^"\|"$//g' | base64 --decode > $2.mp3
}

tts_google() {
    gtts-cli -l ${LANG} -o ${2}.mp3 "${1}"
}

tts_sox() {
    say -v anna "$1" -o ${2}.aiff
    sox ${2}.aiff ${2}.wav pitch 800
    lame -b 128 ${2}.wav ${2}.mp3
}

for i in {1..250}; do
    out=$(printf "%04d" $i)
    echo -n "${out} "
    if [[ -f ${OUT}/${out} ]]; then
        echo "skipping"
        continue
    fi
    ${TTS} "$i" ${TMP}/${out}
    echo "done"
done

gcc -I ${BASE}/../Tonuino -E -x c++ -w -P \
    ${BASE}/../Data/sound_files_${LANG}.txt -o ${TMP}/soundfiles.txt

while read f; do
    i=$(echo -ne "$f" | awk -F ';' '{print $1}')
    t=$(echo -ne "$f" | awk -F ';' '{print $2}' | sed -e 's/"//g')
    out=$(printf "%04d" $i)
    echo -n "$out "
    if [[ -f ${OUT}/${out} ]]; then
        echo "skipping"
        continue
    fi
    ${TTS} "$t" ${TMP}/$out
    echo "done"
done < <(cat ${TMP}/soundfiles.txt)

mkdir -p ${OUT}
mv ${TMP}/*.mp3 ${OUT}/
rm -rf ${TMP}
