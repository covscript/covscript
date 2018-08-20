#!/usr/bin/env bash
astyle -q ./include/covscript/*.*
astyle -q ./include/covscript/*/*.*
rm ./include/covscript/*.orig
rm ./include/covscript/*/*.orig
astyle -A4 -N -t -q ./include/covscript/*.*
astyle -A4 -N -t -q ./include/covscript/*/*.*
rm ./include/covscript/*.orig
rm ./include/covscript/*/*.orig
astyle -q ./sources/*.*
rm ./sources/*.orig
astyle -A4 -N -t -q ./sources/*.*
rm ./sources/*.orig