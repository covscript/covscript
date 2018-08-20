#!/usr/bin/env bash

rm ./include/*.gch
rm ./include/*/*.gch
astyle -q ./include/*.*
astyle -q ./include/*/*.*
rm ./include/*.orig
rm ./include/*/*.orig
astyle -A4 -N -t -q ./include/*.*
astyle -A4 -N -t -q ./include/*/*.*
rm ./include/*.orig
rm ./include/*/*.orig
rm ./sources/*.gch
rm ./sources/*/*.gch
astyle -q ./sources/*.*
astyle -q ./sources/*/*.*
rm ./sources/*.orig
rm ./sources/*/*.orig
astyle -A4 -N -t -q ./sources/*.*
astyle -A4 -N -t -q ./sources/*/*.*
rm ./sources/*.orig
rm ./sources/*/*.orig