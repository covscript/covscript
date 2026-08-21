#!/bin/bash
# Run the curated non-interactive integration tests.
#
# Usage:
#   ./run_tests.sh                 # uses `cs` on PATH
#   CS=/path/to/cs ./run_tests.sh  # use a specific interpreter
#   ./run_tests.sh --generate      # generate expected output files
#
# Note: tests/*.csc that require interactive input (choice, hash_map, import,
# optimize, recursion, test_coroutine) are deliberately NOT part of the
# automatic list — they would block on stdin.

cd "$(dirname "$0")"
CS="${CS:-cs}"
GENERATE=0

# Per-test timeout (seconds). `timeout` is a GNU coreutils command; on systems
# without it (e.g. macOS without coreutils) the timeout guard is skipped.
TIMEOUT_SECS=120
TIMEOUT_CMD=""
if command -v timeout >/dev/null 2>&1; then
	TIMEOUT_CMD="timeout $TIMEOUT_SECS"
fi

if [ "$1" = "--generate" ]; then
	GENERATE=1
	mkdir -p expected
fi

tests=(
	access.csc
	api_cov.csc
	arithmetic.csc
	array.csc
	async_iterator.csc
	benchmark.csc
	char.csc
	char_buff.csc
	cmtime.csc
	compute_pi.csc
	const_in_namespace.csc
	const_in_struct.csc
	coroutine.csc
	escape.csc
	exception.csc
	fiber_busy_wait.csc
	fibonacci.csc
	file.csc
	file_os.csc
	function_invoker.csc
	info.csc
	inherit.csc
	integer.csc
	lambda.csc
	limit.csc
	list.csc
	move.csc
	new.csc
	numeric.csc
	pair.csc
	reference.csc
	serial_execution.csc
	string.csc
	string_ops.csc
	struct.csc
	struct_iterator.csc
	struct_op.csc
	structured_binding.csc
	system.csc
	test_bounds_check.csc
	test_cache.csc
	test_circular_import.csc
	test_dead_co.csc
	test_debugger.csc
	test_fiber_cross_caller.csc
	test_fiber_depth2.csc
	test_future.csc
	test_future_regression.csc
	test_import_fail_cache.csc
	test_inherit_typeid.csc
	test_lambda_unify.csc
	test_link.csc
	test_reentrant_co.csc
	test_self_import.csc
	test_set.csc
	test_shared_cxt.csc
	time.csc
	to_string.csc
	truncate.csc
	type.csc
	typeid.csc
	using.csc
	va_list.csc
)

# Scripts with non-deterministic or platform-dependent output (skip output
# comparison, exit code is still checked).
skip_output=(
	benchmark.csc        # performance timings
	cmtime.csc           # absolute file timestamps
	coroutine.csc        # busy-loop iteration counts
	fiber_busy_wait.csc  # latency measurements
	file_os.csc          # chmod/permission semantics differ on Windows
	info.csc             # runtime.info() is platform-dependent
	serial_execution.csc # performance timings
	test_future.csc      # round-trip counts and timings
	time.csc             # absolute timestamps
	using.csc            # console.clrscr output differs on Windows
)

is_skipped() {
	local name="$1"
	for s in "${skip_output[@]}"; do
		[ "$s" = "$name" ] && return 0
	done
	return 1
}

pass=0
fail=0
output_fail=0
fail_list=""
for f in "${tests[@]}"; do
	if [ "$GENERATE" -eq 1 ]; then
		$TIMEOUT_CMD "$CS" "$f" > "expected/${f%.csc}.expected" 2>/dev/null || true
		echo "Generated expected/${f%.csc}.expected"
		continue
	fi

	actual=$($TIMEOUT_CMD "$CS" "$f" 2>/dev/null | tr -d '\r')
	rc=${PIPESTATUS[0]}
	expected_file="expected/${f%.csc}.expected"

	if [ $rc -eq 124 ]; then
		fail=$((fail + 1))
		fail_list="$fail_list $f"
		echo "TIMEOUT: $f (exceeded ${TIMEOUT_SECS}s)"
	elif [ $rc -ne 0 ]; then
		fail=$((fail + 1))
		fail_list="$fail_list $f"
	elif [ -f "$expected_file" ] && ! is_skipped "$f"; then
		# Normalize line endings so expected files (LF) match Windows CRLF output.
		expected=$(tr -d '\r' < "$expected_file")
		if [ "$actual" != "$expected" ]; then
			output_fail=$((output_fail + 1))
			fail_list="$fail_list $f"
			echo "OUTPUT MISMATCH: $f"
			diff <(printf '%s' "$expected") <(printf '%s' "$actual") || true
		else
			pass=$((pass + 1))
		fi
	else
		pass=$((pass + 1))
	fi
done

if [ "$GENERATE" -eq 1 ]; then
	echo "Generated ${#tests[@]} expected output files in tests/expected/"
	exit 0
fi

total_fail=$((fail + output_fail))
echo "pass=$pass exit_fail=$fail output_mismatch=$output_fail"
if [ -n "$fail_list" ]; then
	echo "FAILED:$fail_list"
	exit 1
fi
