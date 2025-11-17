# RegexEngineC

- A lightweight C implementation of a regular expression engine using:

- Shunting-Yard parser (infix → postfix)

- Thompson’s Construction (postfix → NFA)

- NFA simulation (supports ε-transitions)

- NFA → DFA subset conversion

- Full DFA simulation

- A command-line interface supporting both NFA and DFA modes

- This project is an educational, step-by-step implementation of core regex concepts.

## Features Implemented So Far

### 1. Regex Preprocessing & Parsing

The engine automatically:

- Inserts explicit concatenation operators (.)

- Converts infix regex into postfix (RPN) using the Shunting-Yard algorithm
Supports:

- Literals (a–z, A–Z, 0–9)

- Kleene star *

- Union |

- Grouping ()

- Explicit concatenation . (auto-inserted)

### 2. NFA Construction (Thompson’s Construction)

- Creates NFAs using standard fragments:

- Character transitions

- Concatenation (.)

- Union (|)

- Kleene Star (*)

Each NFA:

- Has unique state IDs

- Stores epsilon-transitions

- Correctly marks accepting states

### 3. NFA Simulation Engine

A fully working NFA simulator:

- Tracks full epsilon-closure

- Maintains current/next state sets

- Correctly detects match vs. no-match

### 4. NFA → DFA Conversion

- Implements subset construction, including:

- Epsilon closures

- Set comparison & canonicalization

- DFA state creation

- Complete ASCII-based transition table

### 5. DFA Simulation Engine

- After conversion:

- Simulates DFA in O(n) time

- Supports full transition table

- Includes optional DFA graph printing

## Project Structure

```text
RegexEngineC/
├── build.bat
├── main.c
├── include/
│   ├── parser.h
│   ├── nfa.h
│   ├── simulator.h
│   ├── dfa.h
├── src/
│   ├── parser.c
│   ├── nfa.c
│   ├── simulator.c
│   ├── dfa.c
├── bin/
│   ├── regex_engine.exe   (created after build)
├── tests/
│   ├── run_tests.ps1
│   ├── run_tests.bat
```

## Build Instructions (Windows)

Requirements

- GCC (e.g., via MinGW or MSYS2)

- PowerShell or CMD

- build.bat (provided)

### Build

From the project root:

```powershell
build.bat
```

If compilation succeeds, you will see:

```powershell
=======================
  COMPILATION SUCCESSFUL
=======================
```

Executable output is placed in:

```bash
bin/regex_engine.exe
```

## Running the Regex Engine

Basic Usage

```bash
regex_engine.exe <regex> <string>
```

Using DFA Mode

```bash
regex_engine.exe --dfa <regex> <string>
```

## Examples

- NFA Simulation

```bash
bin/regex_engine.exe "(a|b)*c" "abbc"
```

- Output:

```bash
Result: Match
```

- DFA Simulation

```bash
bin/regex_engine.exe --dfa "(a|b)*c" "abbc"
```

- Output:

```powershell
Starting regex engine...

Input Infix Regex:  (a|b)*c
String to test:     abbc

--- Phase 1: Parsing ---
Preprocessed Regex: (a|b)*.c
Postfix Notation:   ab|*c.

--- Phase 2: NFA Construction ---
NFA constructed successfully!
Start State ID: 6, End State ID: 9

--- Phase 3a: NFA->DFA Conversion ---
DFA constructed successfully (4 states).

--- Phase 4: DFA Simulation ---
DFA Structure (4 states):
  Start State: S0
  Accepting States: S3

  Transitions:
    State S0 (NFA states: {0,2,4,6,7,8})
      'a' -> S1
      'b' -> S2
      'c' -> S3
    State S1 (NFA states: {0,1,2,4,5,7,8})
      'a' -> S1
      'b' -> S2
      'c' -> S3
    State S2 (NFA states: {0,2,3,4,5,7,8})
      'a' -> S1
      'b' -> S2
      'c' -> S3
    State S3 (NFA states: {9}) [ACCEPT]

Result: Match
```

> Note: Output includes DFA state info + final result.

## Supported Regex Operators

| Operator | Meaning                 | Example      |
|----------|--------------------------|--------------|
| `a`      | Literal character        | `a`          |
| `ab`     | Concatenation            | `a.b`        |
| `\|`     | Alternation (OR)         | `a\|b`       |
| `*`      | Kleene star (0 or more)  | `a*`         |
| `( )`    | Grouping                 | `(ab)`       |

> Note: Implicit concatenation (e.g., `ab`) is automatically rewritten to `a.b` during preprocessing.

## Tests

Tests are located in:

```pwsh
tests/run_tests.ps1
tests/run_tests.bat
```

Run (PowerShell):

```pwsh
./tests/run_tests.ps1
```

> The test harness automatically runs the engine against a suite of predefined regex/string pairs.

## To-Do / Future Work

(Not implemented yet — for roadmap only)

- Freeing NFA & DFA memory (graph traversal)

- Extended regex support (+, ?, character classes)

- Error messaging improvements

- Additional test coverage

## Purpose of This Project

This project is an educational implementation designed to help understand:

- Regex parsing

- NFA theory

- Thompson’s construction

- Subset (powerset) NFA→DFA conversion

- Formal language execution models

## Author

Andrew Fernandes \:)

Built in accordance with the syllabus of Theory of Computer Science, Vivek Kulkarni
