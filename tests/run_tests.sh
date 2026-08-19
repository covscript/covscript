#!/bin/bash
# Run the curated non-interactive integration tests.
#
# The list below is a curated set of tests/*.csc that run to completion without
# keyboard/network input and without external extensions. Stress/profile suites
# are run separately. Interactive
# tests (console, clocks, tcp/udp servers) and extension-dependent tests (codec,
# darwin, extension, reflection) are intentionally excluded. There is no per-test
# timeout: the list is explicit and known to terminate; a hang here is a real
# regression.
#
# Usage:
#   ./run_tests.sh                 # uses `cs` on PATH
#   CS=/path/to/cs ./run_tests.sh  # use a specific interpreter

cd "$(dirname "$0")"
CS="${CS:-cs}"

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

pass=0
fail=0
fail_list=""
for f in "${tests[@]}"; do
	if "$CS" "$f" > /dev/null 2>&1; then
		pass=$((pass + 1))
	else
		fail=$((fail + 1))
		fail_list="$fail_list $f"
	fi
done

echo "pass=$pass fail=$fail"
if [ -n "$fail_list" ]; then
	echo "FAILED:$fail_list"
	exit 1
fi
