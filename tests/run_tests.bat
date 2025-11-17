@echo off
setlocal

REM --- Config ---
SET EXECUTABLE=..\bin\regex_engine.exe
SET PASS_COUNT=0
SET FAIL_COUNT=0
SET TOTAL_COUNT=0

REM --- Helper Function to Run a Test ---
GOTO :run_all_tests

:run_test
    SET "PATTERN=%~1"
    SET "STRING=%~2"
    SET "EXPECTED_RESULT=%~3"
    
    REM Run the engine. 
    REM We expect it to return 0 for a match and 1 for no match.
    %EXECUTABLE% "%PATTERN%" "%STRING%" > nul 2> nul
    SET "EXIT_CODE=%errorlevel%"

    SET /A TOTAL_COUNT+=1
    
    REM Check the result
    if "%EXPECTED_RESULT%" == "Match" (
        if %EXIT_CODE% == 0 (
            echo  [PASS] "%PATTERN%" vs "%STRING%" (Expected: Match)
            SET /A PASS_COUNT+=1
        ) else (
            echo  [FAIL] "%PATTERN%" vs "%STRING%" (Expected: Match, Got: No Match)
            SET /A FAIL_COUNT+=1
        )
    ) else (
        if %EXIT_CODE% == 1 (
            echo  [PASS] "%PATTERN%" vs "%STRING%" (Expected: No Match)
            SET /A PASS_COUNT+=1
        ) else (
            echo  [FAIL] "%PATTERN%" vs "%STRING%" (Expected: No Match, Got: Match)
            SET /A FAIL_COUNT+=1
        )
    )
    GOTO :eof

:run_all_tests
REM --- Test Suite ---
echo Running tests...
echo.

echo --- Group 1: Literals ---
call :run_test "a" "a" "Match"
call :run_test "a" "b" "No Match"
call :run_test "b" "a" "No Match"
echo.

echo --- Group 2: Concatenation ---
call :run_test "ab" "ab" "Match"
call :run_test "abc" "abc" "Match"
call :run_test "ab" "ac" "No Match"
call :run_test "abc" "ab" "No Match"
echo.

echo --- Group 3: Union ---
call :run_test "a|b" "a" "Match"
call :run_test "a|b" "b" "Match"
call :run_test "a|b" "c" "No Match"
call :run_test "ab|cd" "ab" "Match"
call :run_test "ab|cd" "cd" "Match"
call :run_test "ab|cd" "ac" "No Match"
echo.

echo --- Group 4: Kleene Star ---
call :run_test "a*" "" "Match"
call :run_test "a*" "a" "Match"
call :run_test "a*" "aaaa" "Match"
call :run_test "a*" "aaaab" "No Match"
call :run_test "ab*c" "ac" "Match"
call :run_test "ab*c" "abc" "Match"
call :run_test "ab*c" "abbbc" "Match"
call :run_test "ab*c" "ab" "No Match"
echo.

echo --- Group 5: Parentheses ---
call :run_test "(ab)c" "abc" "Match"
call :run_test "a(bc)" "abc" "Match"
call :run_test "a(b|c)d" "abd" "Match"
call :run_test "a(b|c)d" "acd" "Match"
call :run_test "a(b|c)d" "ad" "No Match"
echo.

echo --- Group 6: Combinations ---
call :run_test "(a|b)*" "" "Match"
call :run_test "(a|b)*" "ababbba" "Match"
call :run_test "(a|b)*" "ababbac" "No Match"
call :run_test "(a|b)*c" "c" "Match"
call :run_test "(a|b)*c" "aabbc" "Match"
call :run_test "(a|b)*c" "aabba" "No Match"
echo.

REM --- Summary ---
echo ---------------------------------
echo Test Summary:
echo   Total: %TOTAL_COUNT%
echo   Passed: %PASS_COUNT%
echo   Failed: %FAIL_COUNT%
echo ---------------------------------

endlocal
EXIT /B %FAIL_COUNT%