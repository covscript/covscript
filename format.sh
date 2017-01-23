#/bin/sh
rm ./sources/*.gch
astyle ./sources/*.*
astyle -A4 -N -t ./sources/*.*
rm ./sources/*.orig