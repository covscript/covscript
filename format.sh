#/bin/sh
rm ./sources/*.gch
astyle ./sources/*.*
astyle ./sources/*/*.*
rm ./sources/*.orig
rm ./sources/*/*.orig
astyle -A4 -N -t ./sources/*.*
astyle -A4 -N -t ./sources/*/*.*
rm ./sources/*.orig
rm ./sources/*/*.orig
