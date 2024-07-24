#!/bin/sh

for x in *.adpcm; do
	ffmpeg -f s16le -c:a adpcm_ima_oki -ar 44100 -ac 2 -y -i "${x}" -c:a pcm_s16le "${x%*\.adpcm}.wav"
done

