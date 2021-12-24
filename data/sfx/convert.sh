set -e

FILELIST=$(ls *.wav)

echo "Start ..."

for f in $FILELIST
do
    oldfilename=$(basename -- "$f")
    filename="${oldfilename%.*}"
    newfilename="$filename.ogg"
    echo "$oldfilename -> $newfilename"
    ffmpeg -i $oldfilename  -c:a libvorbis -qscale:a 3 $newfilename
done

