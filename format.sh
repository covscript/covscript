#!/usr/bin/env bash

rm ./include/*.gch
rm ./include/*/*.gch
astyle ./include/*.*
astyle ./include/*/*.*
rm ./include/*.orig
rm ./include/*/*.orig
astyle -A4 -N -t ./include/*.*
astyle -A4 -N -t ./include/*/*.*
rm ./include/*.orig
rm ./include/*/*.orig
rm ./sources/*.gch
rm ./sources/*/*.gch
astyle ./sources/*.*
astyle ./sources/*/*.*
rm ./sources/*.orig
rm ./sources/*/*.orig
astyle -A4 -N -t ./sources/*.*
astyle -A4 -N -t ./sources/*/*.*
rm ./sources/*.orig
rm ./sources/*/*.orig
