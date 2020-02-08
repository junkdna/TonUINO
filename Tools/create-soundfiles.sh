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
TMP=${TMP:-$(mktemp -d)}

mkdir -p ${TMP}/advert
mkdir -p ${TMP}/mp3


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
          'name':'${LANG_CODE}-Wavenet-C',
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

for i in {1..255}; do
    out=$(printf "%04d" $i)
    echo -n "${out} "
    if [[ -f ${OUT}/mp3/${out}.mp3 ]]; then
        echo "skipping"
        continue
    fi
    ${TTS} "$i" ${TMP}/${out}
    echo "done"
done

cat ${BASE}/../Data/sound_files_${LANG}.txt | \
    awk -F ';' '/ADVERT_/{print "advert;" $0 ";";next}/MESSAGE_/{print "mp3;" $0;next}{print $0}' | \
    gcc -I ${BASE}/../Tonuino -E -x c++ -w -P -o ${TMP}/soundfiles.txt -

while read f; do
    d=$(echo -ne "$f" | awk -F ';' '{print $1}')
    i=$(echo -ne "$f" | awk -F ';' '{print $2}')
    t=$(echo -ne "$f" | awk -F ';' '{print $3}' | sed -e 's/"//g')
    out="$(printf "%04d" $i)"
    echo -n "$d $out "
    if [[ -f ${OUT}/$d/${out}.mp3 ]]; then
        echo "skipping"
        continue
    fi
    ${TTS} "$t" ${TMP}/$d/$out
    echo "done"
done < <(cat ${TMP}/soundfiles.txt)

mkdir -p ${OUT}/mp3
mkdir -p ${OUT}/advert

# copy numbers
cp ${TMP}/*.mp3 ${OUT}/advert
mv ${TMP}/*.mp3 ${OUT}/mp3

# copy advert and mp3 to output dir
mv ${TMP}/advert/*.mp3 ${OUT}/advert
mv ${TMP}/mp3/*.mp3 ${OUT}/mp3

rm -rf ${TMP}
