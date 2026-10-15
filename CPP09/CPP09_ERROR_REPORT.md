# CPP09 — Full Error & Compliance Report

**Date:** 2026-10-15
**Scope:** `ex00` (BitcoinExchange) → `ex01` (RPN) → `ex02` (PmergeMe)
**Verdict:** ❌ **NOT READY** — ex01 and ex02 don't build, and both have runtime error-handling bugs.

---

## 0. Summary (ordered by exercise)

| # | ID | Exercise | Severity | Issue |
|---|----|----------|----------|-------|
| 1 | E00-01 | ex00 BitcoinExchange | 🟠 Moderate | Error messages lack trailing newline (violates general rule) |
| 2 | E00-02 | ex00 BitcoinExchange | 🟠 Moderate | `checkDate` accepts impossible dates (e.g. `2011-04-31`) |
| 3 | E00-03 | ex00 BitcoinExchange | 🟠 Moderate | Extensionless input files rejected with misleading error |
| 4 | E00-04 | ex00 BitcoinExchange | 🟠 Moderate | `strtod` accepts `nan` / empty value as valid |
| 5 | E00-05 | ex00 BitcoinExchange | 🟡 Minor | `operator=` returns by value, not by reference |
| 6 | E00-06 | ex00 BitcoinExchange | 🟡 Minor | Copy constructor doesn't copy `_year/_month/_day` |
| 7 | E00-07 | ex00 BitcoinExchange | 🟡 Minor | `getDataBase()` returns a copy; should return const reference |
| 8 | E00-08 | ex00 BitcoinExchange | 🟡 Minor | `fileFormat` rejects files without the exact `date | value` header |
| 9 | E00-09 | ex00 BitcoinExchange | 🟡 Minor | `loadDataBase` doesn't validate DB header/values; uses `atof` |
| 10 | E00-10 | ex00 BitcoinExchange | 🟡 Minor | Database path hardcoded to `./data.csv` |
| 11 | E01-01 | ex01 RPN | 🔴 Critical | Makefile compiles `BitcoinExchange.cpp` instead of `RPN.cpp` |
| 12 | E01-02 | ex01 RPN | 🔴 Critical | Errors printed to stdout + uninitialized result printed (UB) |
| 13 | E02-01 | ex02 PmergeMe | 🔴 Critical | Makefile compiles `BitcoinExchange.cpp` instead of `PmergeMe.cpp` |
| 14 | E02-02 | ex02 PmergeMe | 🔴 Critical | After an error, program continues and segfaults / prints wrong output |
| 15 | E02-03 | ex02 PmergeMe | 🟠 Moderate | `0` accepted (subject requires *positive* integers) |
| 16 | E02-04 | ex02 PmergeMe | 🟠 Moderate | Arbitrary 3000-argument cap |
| 17 | E02-05 | ex02 PmergeMe | 🟠 Moderate | `argc == 1` error printed to stdout (ex02 requires stderr) |
| 18 | E02-06 | ex02 PmergeMe | 🟠 Moderate | `printUseq`/`printSseq` crash on empty container (`size() - 1` underflow) |
| 19 | E02-07 | ex02 PmergeMe | 🟡 Minor | `atol` overflow is UB for very long digit strings |
| 20 | GEN-01 | all exercises | 🟡 Minor | `std::isdigit` called with `char` (potential UB on signed chars) |
| 21 | GEN-02 | all exercises | 🟡 Minor | `<stdexcept>` not included where `std::runtime_error` is used |
| 22 | GEN-03 | all exercises | 🟡 Minor | Unused includes (`fcntl.h`, `unistd.h`, `cstdlib`) |

---

## Verified test results (current state)

### ex00 (BitcoinExchange) — builds ✅, logic mostly works ✅
Subject example output matches exactly (0.9, 0.6, 0.3, 0.36, 0.32, 7.1).
Container used: `std::map` ✅
Known failures: E00-01, E00-02, E00-03, E00-04.

### ex01 (RPN) — `make` fails ❌
```
$ cd ex01 && make
c++ -Wall -Wextra -Werror -std=c++98 -c main.cpp -o main.o
make: *** No rule to make target `BitcoinExchange.o', needed by `RPN'.  Stop.
```
Compiled manually, valid expressions work (`42`, `42`, `0`). Error cases are broken (E01-02).

### ex02 (PmergeMe) — `make` fails ❌
```
$ cd ex02 && make
c++ -Wall -Wextra -Werror -std=c++98 -c main.cpp -o main.o
make: *** No rule to make target `BitcoinExchange.o', needed by `PmergeMe'.  Stop.
```
Compiled manually, Ford-Johnson sorting is correct (verified with random sequences + 3000 ints, vector ≈ 3.6 ms vs deque ≈ 5.9 ms). Error cases segfault (E02-02).

---

## 1. ex00 — BitcoinExchange

### E00-01 · Error messages lack a trailing newline
**File:** `ex00/main.cpp` line 20
```cpp
catch(const std::runtime_error &e)
{
    std::cout << e.what();    // no std::endl
    return 1;
}
```
General rule: *"every output message must end with a newline character."*
```bash
$ ./btc doesnotexist.txt
[ERROR]: 'doesnotexist.txt' doesn't exist or is not accessible.rc=1
```
**Fix:**
```cpp
catch (const std::exception &e)
{
    std::cerr << e.what() << std::endl;   // stderr + newline
    return 1;
}
```
(Or `std::cout << e.what() << std::endl;` if you prefer stdout — the newline is what's mandatory.)

---

### E00-02 · `checkDate` accepts impossible dates
**File:** `ex00/BitcoinExchange.cpp` lines 95–139

**Confirmed wrong behavior:**
```bash
$ echo -e "date | value\n2011-04-31 | 1\n2011-06-31 | 1" | ./btc /dev/stdin
2011-04-31 => 1 = 2.66    # April has 30 days
2011-06-31 => 1 = 17.89   # June has 30 days
```
The current code only rejects day > 31 and Feb > 29. It never checks month/day combinations.

**Fix — rewrite `checkDate` cleanly:**
```cpp
bool BitcoinExchange::checkDate(const std::string &date)
{
    if (date.size() != 10 || date[4] != '-' || date[7] != '-')
        return false;

    for (int i = 0; i < 10; i++)
    {
        if (i == 4 || i == 7)
            continue;
        if (!std::isdigit(static_cast<unsigned char>(date[i])))
            return false;
    }

    int year  = std::atoi(date.substr(0, 4).c_str());
    int month = std::atoi(date.substr(5, 2).c_str());
    int day   = std::atoi(date.substr(8, 2).c_str());

    if (month < 1 || month > 12 || day < 1)
        return false;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = daysInMonth[month - 1];

    if (month == 2)
    {
        bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (leap)
            maxDay = 29;
    }

    if (day > maxDay)
        return false;

    setYear(date.substr(0, 4));
    setMonth(date.substr(5, 2));
    setDay(date.substr(8, 2));
    return true;
}
```
You can then delete `checkLeapYear` or keep it for the Feb check — either way, the month/day combination must be validated.

---

### E00-03 · Extensionless input files are rejected with a misleading message
**Files:** `ex00/BitcoinExchange.cpp` — `checkFile` (lines 218–232), `fileExtension` (lines 54–61)

**Confirmed wrong behavior:**
```bash
$ ./btc /tmp/input          # a perfectly valid file with no extension
[ERROR]: The file name '/tmp/input' is too short.
```
The subject only says *"your program must take a file as argument"* — no extension requirement. The current logic:
- computes `fileExt` but never actually checks the extension against it,
- if there is no dot, `dot` is set to 0, so `fileExt` becomes the whole path and the size check throws "too short".

**Fix — remove the useless check entirely:**
```cpp
void BitcoinExchange::checkFile(const char* filePath)
{
    fileNotEmpty(filePath);
    fileFormat(filePath);
}
```
And remove `fileExtension()` (or, if you really want an extension check, implement it correctly — but don't require `.csv`, the subject example uses `.txt`).

---

### E00-04 · `strtod` accepts `nan` and empty value as valid
**File:** `ex00/BitcoinExchange.cpp` lines 186–199

**Problem:**
- `"nan"` → `strtod` returns NaN, endptr at end → passes `checkPrice` (both comparisons are false) → program prints `nan` as a valid price.
- `""` (empty value, e.g. `2011-01-03 | `) → `strtod` returns 0, endptr at end → accepted as 0.

**Fix:**
```cpp
std::string priceString = line.substr(separator + 3);
if (priceString.empty())
{
    std::cerr << "[ERROR]: bad input => " << line << std::endl;
    continue;
}

char* endPtr = NULL;
double price = std::strtod(priceString.c_str(), &endPtr);
if (*endPtr != '\0' || price != price)   // price != price detects NaN
{
    std::cerr << "[ERROR]: bad input => " << line << std::endl;
    continue;
}
```

---

### E00-05 · `operator=` returns by value
**File:** `ex00/BitcoinExchange.hpp` line 17, `ex00/BitcoinExchange.cpp` lines 10–17
Orthodox Canonical Form expects a reference return. Current code creates an unnecessary temporary.
**Fix:**
```cpp
// .hpp
BitcoinExchange& operator=(const BitcoinExchange& other);

// .cpp
BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other)
{
    if (this != &other)
    {
        _database = other._database;
        _year = other._year;
        _month = other._month;
        _day = other._day;
    }
    return *this;
}
```

---

### E00-06 · Copy constructor doesn't copy `_year/_month/_day`
**File:** `ex00/BitcoinExchange.cpp` lines 5–8
```cpp
BitcoinExchange::BitcoinExchange(const BitcoinExchange& other)
{
    _database = other._database;   // _year, _month, _day left uninitialized
}
```
**Fix:**
```cpp
BitcoinExchange::BitcoinExchange(const BitcoinExchange& other)
{
    _database = other._database;
    _year = other._year;
    _month = other._month;
    _day = other._day;
}
```

---

### E00-07 · `getDataBase()` returns a full copy
**File:** `ex00/BitcoinExchange.hpp` line 20, `ex00/BitcoinExchange.cpp` lines 23–26
**Fix:**
```cpp
// .hpp
const std::map<std::string, float>& getDataBase() const;

// .cpp
const std::map<std::string, float>& BitcoinExchange::getDataBase() const
{
    return _database;
}
```

---

### E00-08 · Files without the exact `date | value` header are rejected
**File:** `ex00/BitcoinExchange.cpp` lines 161–166
```cpp
std::getline(userDB, line);
if (line != "date | value")
    throw ...;
```
The subject example includes this header, so it's defensible, but an evaluator may feed a headerless file. A common, safer approach: read the first line, skip it only if it equals `date | value`, otherwise process it as data.
**Optional fix:**
```cpp
std::getline(userDB, line);
if (line == "date | value")
{
    if (!std::getline(userDB, line))
        return;
}
// now process 'line' (and continue the loop with subsequent lines)
```
(If you prefer strict validation, keep the throw — but know the risk.)

---

### E00-09 · `loadDataBase` doesn't validate the database
**File:** `ex00/BitcoinExchange.cpp` lines 234–262
It never checks that the first line is `date,exchange_rate`, that lines contain a comma, or that prices are valid numbers. With the official `data.csv` it works, so this is hardening, not a blocker.
**Suggested improvements:** verify the header, reject lines with no comma, use `strtod` with validation instead of `atof`.

---

### E00-10 · Database path hardcoded to `./data.csv`
**File:** `ex00/main.cpp` line 15
```cpp
be.loadDataBase("./data.csv");
```
Works if the evaluator runs from inside `ex00` (the usual case). If they run `./ex00/btc file` from the repo root, it fails. Low risk, but you could derive the path from `argv[0]` or try several candidate paths.

---

## 2. ex01 — RPN

### E01-01 · Makefile references the wrong source file
**File:** `ex01/Makefile` lines 7–8
**Current:**
```make
SRCS        = main.cpp \
              BitcoinExchange.cpp
```
**Problem:** `BitcoinExchange.cpp` exists only in `ex00`. `make` cannot find it and stops.

**Fix:**
```make
SRCS        = main.cpp \
              RPN.cpp
```

---

### E01-02 · Errors go to stdout, exceptions are swallowed, uninitialized result is printed
**Files:** `ex01/RPN.cpp` (constructor), `ex01/main.cpp`

**Current behavior:**
```bash
$ ./RPN "(1 + 1)"
[ERROR]: Invalid character: '('.1      # error on stdout, no newline, garbage "1", rc=0
```

**Root cause 1 — constructor swallows the exception** (`ex01/RPN.cpp` lines 5–15):
```cpp
RPN::RPN(const std::string &xpr) : _xpr(xpr)
{
    try
    {
        _result = parseStk(xpr);
    }
    catch (const std::runtime_error &e)
    {
        std::cout << e.what();          // stdout + no newline + exception swallowed
    }
}
```

**Root cause 2 — main prints `_result` anyway** (`ex01/main.cpp` lines 11–13):
```cpp
RPN arg(argv[1]);
std::cout << arg.getResult() << std::endl;   // _result was never initialized -> UB
```

The subject requires: *"If an error occurs during the execution of the program an error message should be displayed on the standard error."*

**Fix:**

`ex01/RPN.cpp` — constructor no longer catches (or rethrows):
```cpp
RPN::RPN(const std::string &xpr) : _xpr(xpr), _result(0)
{
    _result = parseStk(xpr);
}
```
(Remove the `try/catch` entirely.)

`ex01/main.cpp` — wrap everything in try/catch, return 1 on error:
```cpp
#include "RPN.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "[ERROR]: The number of arguments is invalid." << std::endl;
        return 1;
    }

    try
    {
        RPN arg(argv[1]);
        std::cout << arg.getResult() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```
This also fixes the missing newline (adds `<< std::endl`).

---

## 3. ex02 — PmergeMe

### E02-01 · Makefile references the wrong source file
**File:** `ex02/Makefile` lines 7–8
**Fix:**
```make
SRCS        = main.cpp \
              PmergeMe.cpp
```

---

### E02-02 · Program continues after an error → segfault / wrong output
**File:** `ex02/main.cpp` lines 41–55

**Current behavior:**
```bash
$ ./PmergeMe -1 2
Segmentation fault (exit 139)

$ ./PmergeMe 2147483648
Segmentation fault (exit 139)

$ ./PmergeMe 1 a 3
[ERROR]: Invalid character found. Only positive integers allowed.
Unsorted Sequence: 1        # should NOT print anything after an error
Sorted Sequence: 1
...
```

**Root cause:** the `catch` block prints the error but never returns, then `main` builds `PmergeMe` with an empty/partial vector and prints it.

```cpp
catch(const std::exception& e)
{
    std::cerr << e.what() << '\n';   // no return -> execution continues
}

PmergeMe sequence(vec, deq);          // runs even after an error
...
```

**Fix:** add `return 1;` inside the catch block:
```cpp
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}
```

---

### E02-03 · `0` is accepted (subject says *positive* integers)
**File:** `ex02/main.cpp` line 30
```cpp
if (val < 0 || val > INT_MAX)     // allows 0
```
**Fix:**
```cpp
if (val <= 0 || val > INT_MAX)
```

---

### E02-04 · Arbitrary cap at 3000 arguments
**File:** `ex02/main.cpp` line 5
```cpp
if (argc == 1 || argc > 3001)
```
The subject only says the program must handle **at least** 3000 integers. A cap can fail an evaluator who passes more.
**Fix:**
```cpp
if (argc < 2)
```

---

### E02-05 · No-argument error goes to stdout
**File:** `ex02/main.cpp` line 7
```cpp
std::cout << "[ERROR]: The number of arguments are invalid." << std::endl;
```
ex02 errors must go to **stderr**.
**Fix:** change `std::cout` → `std::cerr`.

---

### E02-06 · `printUseq`/`printSseq` crash on empty container
**File:** `ex02/PmergeMe.cpp` lines 65–83
```cpp
for (size_t i = 0; i < _uvec.size() - 1; i++)   // size()-1 underflows when size()==0
    ...
std::cout << _uvec[_uvec.size() - 1] << std::endl;  // out-of-bounds when empty
```
This is the direct cause of the segfault after E02-02 leaves `vec` empty.
**Fix (safer loop, no underflow):**
```cpp
void PmergeMe::printUseq()
{
    for (size_t i = 0; i < _uvec.size(); i++)
    {
        std::cout << _uvec[i];
        if (i + 1 < _uvec.size())
            std::cout << " ";
    }
    std::cout << std::endl;
}
```
(Apply the same pattern to `printSseq`.)

---

### E02-07 · `atol` overflow is undefined behavior
**File:** `ex02/main.cpp` lines 20–34
Very long digit strings (e.g. `99999999999999999999999`) pass the `isdigit` loop, then `std::atol` overflows → UB.
**Fix:** use `std::strtol` with an end pointer:
```cpp
char* end = NULL;
long val = std::strtol(argv[i], &end, 10);
if (*end != '\0' || val <= 0 || val > INT_MAX)
{
    throw std::runtime_error("[ERROR]: Number out of range.");
}
```

---

## 4. General / all exercises

### GEN-01 · `std::isdigit(char)` can be undefined behavior
**Files:** `ex00/BitcoinExchange.cpp` line 121, `ex01/RPN.cpp` line 58, `ex02/main.cpp` line 22
`std::isdigit` requires the argument to be an `unsigned char` or `EOF`. Negative `char` values (e.g. non-ASCII input) are UB.
**Fix everywhere:** cast first:
```cpp
std::isdigit(static_cast<unsigned char>(str[j]))
```

### GEN-02 · `<stdexcept>` not included where `std::runtime_error` is used
It compiles only because `<iostream>` transitively includes it on this platform. Headers should include all their dependencies.
**Fix:** add `#include <stdexcept>` to:
- `ex00/BitcoinExchange.hpp` (or `BitcoinExchange.cpp`)
- `ex01/RPN.hpp` (or `RPN.cpp`)
- `ex02/PmergeMe.hpp` (or `main.cpp`)

### GEN-03 · Unused includes
- `ex00/BitcoinExchange.hpp`: `<fcntl.h>`, `<unistd.h>` — remove.
- `ex01/RPN.hpp`: `<cstdlib>` — remove (not used).

---

## 5. Things already compliant ✅

- Directory / file names match the subject (`Makefile`, `main.cpp`, `BitcoinExchange.{cpp,hpp}`, `RPN.{cpp,hpp}`, `PmergeMe.{cpp,hpp}`).
- Program names: `btc`, `RPN`, `PmergeMe`.
- Compiles with `-Wall -Wextra -Werror -std=c++98`.
- No `using namespace`, no `friend`, no `printf`/`*alloc`/`free`, no C++11 features.
- Container rule respected: ex00 → `std::map`, ex01 → `std::stack`, ex02 → `std::vector` + `std::deque` (no reuse).
- Orthodox Canonical Form present in all three classes (ex00 `operator=` return type aside).
- Makefiles have `$(NAME), all, clean, fclean, re` + `.PHONY`, use `c++`, and don't relink.
- ex00: lower-bound → closest lower date logic is correct; subject example output values match exactly.
- ex01: valid expressions evaluate correctly (`8 9 * 9 - 9 - 9 - 4 - 1 +` → 42, etc.).
- ex02: Ford-Johnson (merge-insertion) algorithm sorts correctly; Jacobsthal insertion order is right; timing precision (µs) clearly shows vector vs deque difference; no memory leaks anywhere (`new` is never used).

---

## 6. Suggested fix order

1. **E01-01 + E02-01** — fix both Makefiles (30 seconds).
2. **E01-02** — move error handling to `main`, stderr + newline + return 1.
3. **E02-02 + E02-06** — `return 1` in catch; safe print loops.
4. **E02-03, E02-04, E02-05, E02-07** — input validation fixes.
5. **E00-01 … E00-04** — newline, date validation, extension check, price parsing.
6. Rebuild all three with `make re` and re-run the tests listed in each section.

After fixes, re-run:
```bash
cd ex00 && make && ./btc input.txt
cd ex01 && make && ./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +" && ./RPN "(1 + 1)"
cd ex02 && make && ./PmergeMe 3 5 9 7 4 && ./PmergeMe -1 2
```
