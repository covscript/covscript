#!/bin/bash
# Verify the COVSCRIPT_DEBUG runtime guard levels (none / warning / strict).
# Runs tests/test_debug_mode.csc (a fixture that abandons a suspended fiber)
# under each mode and checks exit code and stderr.
#
# Usage:
#   ./test_debug_mode.sh              # uses `cs` on PATH
#   CS=/path/to/cs ./test_debug_mode.sh

cd "$(dirname "$0")"
CS="${CS:-cs}"
FAIL=0

check() {
	local name="$1" mode="$2" want_exit="$3" want_warn="$4"
	local out code
	if [ -n "$mode" ]; then
		out=$(COVSCRIPT_DEBUG="$mode" "$CS" test_debug_mode.csc 2>&1)
		code=$?
	else
		out=$("$CS" test_debug_mode.csc 2>&1)
		code=$?
	fi
	local pass=1
	case "$want_exit" in
		0) [ "$code" -eq 0 ] || pass=0 ;;
		nonzero) [ "$code" -ne 0 ] || pass=0 ;;
	esac
	case "$want_warn" in
		yes) case "$out" in *"[fiber] warning"*) ;; *) pass=0 ;; esac ;;
		no) case "$out" in *"[fiber] warning"*) pass=0 ;; *) ;; esac ;;
	esac
	if [ "$pass" -eq 1 ]; then
		echo "PASS $name (mode=${mode:-unset} exit=$code)"
	else
		echo "FAIL $name (mode=${mode:-unset} exit=$code stderr=[$out])"
		FAIL=$((FAIL + 1))
	fi
}

check none      none     0      no
check warning   warning  0      yes
check strict    strict   nonzero yes
check default   ""       0      yes
check invalid   bogus    0      yes

echo "fail=$FAIL"
[ "$FAIL" -eq 0 ]
