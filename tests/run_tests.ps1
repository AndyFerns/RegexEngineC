# Force script to use its own directory as working directory
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location $ScriptDir

# --- Config ---
$executable = "..\bin\regex_engine.exe"
$passCount = 0
$failCount = 0

# --- Test Suite ---
$testCases = @(
    # Group 1: Literals
    @{ Pattern = "a"; String = "a"; Expected = "Match" },
    @{ Pattern = "a"; String = "b"; Expected = "NoMatch" },
    @{ Pattern = "b"; String = "a"; Expected = "NoMatch" },
    
    # Group 2: Concatenation
    @{ Pattern = "ab"; String = "ab"; Expected = "Match" },
    @{ Pattern = "abc"; String = "abc"; Expected = "Match" },
    @{ Pattern = "ab"; String = "ac"; Expected = "NoMatch" },
    @{ Pattern = "abc"; String = "ab"; Expected = "NoMatch" },
    
    # Group 3: Union
    @{ Pattern = "a|b"; String = "a"; Expected = "Match" },
    @{ Pattern = "a|b"; String = "b"; Expected = "Match" },
    @{ Pattern = "a|b"; String = "c"; Expected = "NoMatch" },
    @{ Pattern = "ab|cd"; String = "ab"; Expected = "Match" },
    @{ Pattern = "ab|cd"; String = "cd"; Expected = "Match" },
    @{ Pattern = "ab|cd"; String = "ac"; Expected = "NoMatch" },
    
    # Group 4: Kleene Star
    @{ Pattern = "a*"; String = ""; Expected = "Match" },
    @{ Pattern = "a*"; String = "a"; Expected = "Match" },
    @{ Pattern = "a*"; String = "aaaa"; Expected = "Match" },
    @{ Pattern = "a*"; String = "aaaab"; Expected = "NoMatch" },
    @{ Pattern = "ab*c"; String = "ac"; Expected = "Match" },
    @{ Pattern = "ab*c"; String = "abc"; Expected = "Match" },
    @{ Pattern = "ab*c"; String = "abbbc"; Expected = "Match" },
    @{ Pattern = "ab*c"; String = "ab"; Expected = "NoMatch" },
    
    # Group 5: Parentheses
    @{ Pattern = "(ab)c"; String = "abc"; Expected = "Match" },
    @{ Pattern = "a(bc)"; String = "abc"; Expected = "Match" },
    @{ Pattern = "a(b|c)d"; String = "abd"; Expected = "Match" },
    @{ Pattern = "a(b|c)d"; String = "acd"; Expected = "Match" },
    @{ Pattern = "a(b|c)d"; String = "ad"; Expected = "NoMatch" },
    
    # Group 6: Combinations
    @{ Pattern = "(a|b)*"; String = ""; Expected = "Match" },
    @{ Pattern = "(a|b)*"; String = "ababbba"; Expected = "Match" },
    @{ Pattern = "(a|b)*"; String = "ababbac"; Expected = "NoMatch" },
    @{ Pattern = "(a|b)*c"; String = "c"; Expected = "Match" },
    @{ Pattern = "(a|b)*c"; String = "aabbc"; Expected = "Match" },
    @{ Pattern = "(a|b)*c"; String = "aabba"; Expected = "NoMatch" },

    # Group 7: Complex / DFA Stress Tests (New!)
    # These test if state merging works correctly
    @{ Pattern = "((a|b)*)c"; String = "abababc"; Expected = "Match" },
    @{ Pattern = "a*b*c*"; String = "aaabbc"; Expected = "Match" },
    @{ Pattern = "a*b*c*"; String = "c"; Expected = "Match" }, # Zero a's and zero b's
    @{ Pattern = "a*b*c*"; String = "aaacbb"; Expected = "NoMatch" } # Order violation
)

# Define the modes we want to run
$modes = @(
    @{ Name = "NFA SIMULATION"; ArgList = @() },
    @{ Name = "DFA SIMULATION"; ArgList = @("--dfa") }
)

# --- Run Tests Loop ---
foreach ($mode in $modes) {
    Write-Host ""
    Write-Host "==========================================" -ForegroundColor Cyan
    Write-Host "  RUNNING $($mode.Name)" -ForegroundColor Cyan
    Write-Host "==========================================" -ForegroundColor Cyan
    Write-Host ""

    foreach ($test in $testCases) {
        # Build the arguments: Flag (if any) + Pattern + String
        $argsToRun = $mode.ArgList + $test.Pattern + $test.String

        # Run the executable, supressing its stdout
        # We use the call operator '&' to pass the array of arguments cleanly
        & $executable $argsToRun > $null
        
        # Get the exit code (0 = Match, 1 = NoMatch)
        $exitCode = $LASTEXITCODE
        
        # Determine what the engine reported
        $result = if ($exitCode -eq 0) { "Match" } else { "NoMatch" }

        # Check against expectation
        if ($result -eq $test.Expected) {
            Write-Host -ForegroundColor Green "  [PASS] '$($test.Pattern)' vs '$($test.String)' (Expected: $($test.Expected))"
            $passCount++
        } else {
            Write-Host -ForegroundColor Red "  [FAIL] '$($test.Pattern)' vs '$($test.String)' (Expected: $($test.Expected), Got: $result)"
            $failCount++
        }
    }
}

# --- Summary ---
$totalTestsRun = $testCases.Count * $modes.Count

Write-Host ""
Write-Host "---------------------------------"
Write-Host "Final Test Summary:"
Write-Host "  Total Tests Run: $totalTestsRun"
Write-Host -ForegroundColor Green "  Passed:          $passCount"
if ($failCount -gt 0) {
    Write-Host -ForegroundColor Red "  Failed:          $failCount"
} else {
    Write-Host -ForegroundColor Green "  Failed:          $failCount"
}
Write-Host "---------------------------------"

# Set the exit code for the script itself
if ($failCount -gt 0) {
    exit 1
} else {
    exit 0
}