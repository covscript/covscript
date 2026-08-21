# Run the curated non-interactive integration tests (Windows).
# Provides per-test timeout and CRLF-normalized expected-output comparison
# (Windows `fc /n` compares bytes and would always fail against LF files).
#
# Usage:
#   powershell -NoProfile -ExecutionPolicy Bypass -File run_tests.ps1
#   CS=C:\path\to\cs.exe powershell -NoProfile -ExecutionPolicy Bypass -File run_tests.ps1
#   powershell -NoProfile -ExecutionPolicy Bypass -File run_tests.ps1 -Generate
#
# Note: tests/*.csc that require interactive input (choice, hash_map, import,
# optimize, recursion, test_coroutine) are deliberately NOT part of the
# automatic list — they would block on stdin.

param(
	[switch]$Generate
)

$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot

$CS = if ($env:CS) { $env:CS } else { 'cs' }
$TimeoutSec = 120

$tests = @(
	'access.csc',
	'api_cov.csc',
	'arithmetic.csc',
	'array.csc',
	'async_iterator.csc',
	'benchmark.csc',
	'char.csc',
	'char_buff.csc',
	'cmtime.csc',
	'compute_pi.csc',
	'const_in_namespace.csc',
	'const_in_struct.csc',
	'coroutine.csc',
	'escape.csc',
	'exception.csc',
	'fiber_busy_wait.csc',
	'fibonacci.csc',
	'file.csc',
	'file_os.csc',
	'function_invoker.csc',
	'info.csc',
	'inherit.csc',
	'integer.csc',
	'lambda.csc',
	'limit.csc',
	'list.csc',
	'move.csc',
	'new.csc',
	'numeric.csc',
	'pair.csc',
	'reference.csc',
	'serial_execution.csc',
	'string.csc',
	'string_ops.csc',
	'struct.csc',
	'struct_iterator.csc',
	'struct_op.csc',
	'structured_binding.csc',
	'system.csc',
	'test_bounds_check.csc',
	'test_cache.csc',
	'test_circular_import.csc',
	'test_dead_co.csc',
	'test_debugger.csc',
	'test_fiber_cross_caller.csc',
	'test_fiber_depth2.csc',
	'test_future.csc',
	'test_future_regression.csc',
	'test_import_fail_cache.csc',
	'test_inherit_typeid.csc',
	'test_lambda_unify.csc',
	'test_link.csc',
	'test_reentrant_co.csc',
	'test_self_import.csc',
	'test_set.csc',
	'test_shared_cxt.csc',
	'time.csc',
	'to_string.csc',
	'truncate.csc',
	'type.csc',
	'typeid.csc',
	'using.csc',
	'va_list.csc'
)

# Scripts with non-deterministic or platform-dependent output (skip output
# comparison, exit code is still checked).
$skipOutput = @(
	'benchmark.csc',
	'cmtime.csc',
	'coroutine.csc',
	'fiber_busy_wait.csc',
	'file_os.csc',
	'info.csc',
	'limit.csc',
	'numeric.csc',
	'serial_execution.csc',
	'test_future.csc',
	'time.csc',
	'using.csc'
)

function Get-ExpectedPath([string]$scriptName) {
	return Join-Path (Join-Path $PSScriptRoot 'expected') `
	    ([System.IO.Path]::GetFileNameWithoutExtension($scriptName) + '.expected')
}

if ($Generate) {
	New-Item -ItemType Directory -Force -Path (Join-Path $PSScriptRoot 'expected') | Out-Null
	foreach ($f in $tests) {
		# cmd redirection keeps raw bytes (PowerShell `>` would write UTF-16).
		cmd /c "`"$CS`" `"$f`" > `"$(Get-ExpectedPath $f)`"" 2>$null
		Write-Host "Generated $(Get-ExpectedPath $f)"
	}
	Write-Host "Generated $($tests.Count) expected output files in tests\expected\"
	exit 0
}

$pass = 0
$fail = 0
$outputFail = 0
$failList = @()

foreach ($f in $tests) {
	# Note: Start-Process' ExitCode is unreliable in Windows PowerShell 5.1
	# when combined with -RedirectStandardOutput, so use the raw Process API.
	$psi = New-Object System.Diagnostics.ProcessStartInfo
	$psi.FileName = $CS
	$psi.Arguments = $f
	$psi.WorkingDirectory = $PSScriptRoot
	$psi.UseShellExecute = $false
	$psi.CreateNoWindow = $true
	$psi.RedirectStandardOutput = $true
	$psi.RedirectStandardError = $true
	$psi.StandardOutputEncoding = [System.Text.Encoding]::UTF8
	$p = [System.Diagnostics.Process]::Start($psi)
	if (-not $p.WaitForExit($TimeoutSec * 1000)) {
		$p.Kill()
		$p.WaitForExit(5000) | Out-Null
		$fail += 1
		$failList += $f
		Write-Host "TIMEOUT: $f (exceeded ${TimeoutSec}s)"
		continue
	}
	$rc = $p.ExitCode
	# Normalize line endings so expected files (LF) match CRLF output.
	$actual = $p.StandardOutput.ReadToEnd() -replace "`r", ""
	$expectedFile = Get-ExpectedPath $f
	if ($rc -ne 0) {
		$fail += 1
		$failList += $f
	} elseif ((Test-Path $expectedFile) -and ($skipOutput -notcontains $f)) {
		$expected = [System.IO.File]::ReadAllText($expectedFile, [System.Text.Encoding]::UTF8) -replace "`r", ""
		if ($actual -ne $expected) {
			$outputFail += 1
			$failList += $f
			Write-Host "OUTPUT MISMATCH: $f"
		} else {
			$pass += 1
		}
	} else {
		$pass += 1
	}
}

Write-Host "pass=$pass exit_fail=$fail output_mismatch=$outputFail"
if ($failList.Count -gt 0) {
	Write-Host "FAILED:$($failList -join ' ')"
	exit 1
}
exit 0
