#!/bin/bash
# Run the curated non-interactive integration tests.
#
# Usage:
#   ./run_tests.sh                 # uses `cs` on PATH
#   CS=/path/to/cs ./run_tests.sh  # use a specific interpreter
#   ./run_tests.sh --generate      # generate expected output files

cd "$(dirname "$0")"
CS="${CS:-cs}"
GENERATE=0

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
	choice.csc
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
	hash_map.csc
	import.csc
	info.csc
	inherit.csc
	integer.csc
	lambda.csc
	limit.csc
	list.csc
	move.csc
	new.csc
	numeric.csc
	optimize.csc
	pair.csc
	recursion.csc
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
	test_coroutine.csc
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

# Scripts that are non-deterministic (skip output comparison).
skip_output=(
	benchmark.csc
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
		"$CS" "$f" > "expected/${f%.csc}.expected" 2>/dev/null || true
		echo "Generated expected/${f%.csc}.expected"
		continue
	fi

	actual=$("$CS" "$f" 2>/dev/null)
	rc=$?
	expected_file="expected/${f%.csc}.expected"

	if [ $rc -ne 0 ]; then
		fail=$((fail + 1))
		fail_list="$fail_list $f"
	elif [ -f "$expected_file" ] && ! is_skipped "$f"; then
		expected=$(cat "$expected_file")
		if [ "$actual" != "$expected" ]; then
			output_fail=$((output_fail + 1))
			fail_list="$fail_list $f"
			echo "OUTPUT MISMATCH: $f"
			diff <(echo "$expected") <(echo "$actual") || true
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
