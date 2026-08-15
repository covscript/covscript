@echo off
rem Run the curated non-interactive integration tests (Windows).
rem Same explicit list as run_tests.sh. Interactive tests (console, clocks,
rem tcp/udp servers) and extension-dependent tests (codec, darwin, extension,
rem reflection) are intentionally excluded. There is no per-test timeout: the
rem list is explicit and known to terminate.
rem
rem Usage:
rem   run_tests.bat              uses `cs` on PATH
rem   set CS=c:\path\to\cs.exe  then run_tests.bat

setlocal enabledelayedexpansion
cd /d "%~dp0"
if "%CS%"=="" set "CS=cs"

set "PASS=0"
set "FAIL=0"
set "FAIL_LIST="

for %%f in (
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
) do (
	"%CS%" "%%f" >nul 2>&1
	if errorlevel 1 (
		set /a FAIL+=1
		set "FAIL_LIST=!FAIL_LIST! %%f"
	) else (
		set /a PASS+=1
	)
)

echo pass=%PASS% fail=%FAIL%
if defined FAIL_LIST echo FAILED:%FAIL_LIST%
if not "%FAIL%"=="0" exit /b 1
