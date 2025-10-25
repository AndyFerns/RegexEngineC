@echo off
REM Clears the console screen
cls

REM Define compiler and flags
SET CC=gcc
SET CFLAGS=-Wall -Wextra -g
SET INCLUDES=-Iinclude

REM Define the target executable name
SET TARGET=bin\regex_engine.exe

REM Find all source files in the project
SET SOURCES=main.c src\parser.c src\nfa.c src\simulator.c

REM --- Compilation Step ---
echo Compiling project...
%CC% %CFLAGS% %SOURCES% -o %TARGET% %INCLUDES%

REM --- Check if compilation was successful and run ---
if %errorlevel% neq 0 (
    echo.
    echo =====================
    echo  COMPILATION FAILED!
    echo =====================
    goto :eof
)

echo.
echo =======================
echo  COMPILATION SUCCESSFUL
echo =======================
echo.
echo -------------------------------------------------------------
echo Running the engine with pattern "(a|b)*c" and string "abbc"...
echo -------------------------------------------------------------
echo Example Run:
echo %TARGET% "(a|b)*c" "abbc"