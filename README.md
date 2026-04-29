# Straw Hats ChocAn

A C++17 console application implementing the ChocAn healthcare provider management system.
Covers provider login, member validation, service record entry, weekly batch reporting,
operator CRUD, and provider directory generation. All persistence is CSV-based — no database required.

Startup main.cpp loads all 4 CSV files into memory (members, providers, services, service_records), wires everything together, then drops you at a 3-way terminal selector.

## Provider Terminal — the main workflow

Provider enters their 9-digit number + 4-digit PIN → ProviderLoginController checks it against providers.csv
Once in, they can either:
Submit a service record — walks through member validation -> date -> service code -> confirm -> optional comments -> then appends a row to service_records.csv
Request the provider directory — generates a sorted .txt file of all services + fees to data/output/


## Manager Terminal — reporting

Enter a week-ending date, WeeklyBatchGenerator scans service_records.csv for the 7-day window and spits out 5 files: per-provider report, per-member report, weekly summary, accounts payable summary, and an EFT transfer file


## Operator Terminal — data management

Add/update/delete members and providers, changes write back to CSV immediately
Import an Acme nightly update file to bulk-update member statuses


## Data flow in one line:

CSV files -> loaded into memory at startup → controllers read/write in-memory maps → changes flushed back to CSV on every mutation

Nothing is held in a database. Every repo is just an unordered_map keyed by ID number, backed by a flat CSV file. service_records.csv is the one exception — it's append-only, never rewritten in full.

## Testing 

run_tests.py pipes scripted keyboard input directly to the compiled binary and checks that the exact locked tag strings (like [LOGIN_SUCCESS], [RECORD_SAVED]) appear in stdout. CSVs are snapshotted before each mutating test and restored after, so the 41 tests are fully repeatable

---

## Team

| Name | Module |
|------|--------|
| Aldo Medina | UC02 — Submit Service Record; Provider Terminal flow + record save |
| Lulu Chen | UC01 — Validate Member Eligibility; Data Management (Operator) |
| Audity Moumi | UC04 — Weekly Batch + Reports + EFT outputs |
| Rania Dayekh | UC05 — Provider Directory generation |

---

## How to Build

### Option 1 — Build script (recommended PLEASE)

Prerequisites: CMake 3.16+, a C++17 compiler

**Windows:**
```bat
./build.bat
```

**Linux / macOS:**
```bash
chmod +x build.sh
./build.sh
```

### Option 2 — Make

```bash
make          # release build
make debug    # debug build
make clean    # remove build/
make run      # build + run with data/
```

**Output binary:**
- Windows: `build\Release\strawhats.exe`
- Linux/macOS: `build/strawhats`

### Prerequisites

| Tool | Windows | macOS | Linux (Debian/Ubuntu) |
|------|---------|-------|-----------------------|
| CMake | [cmake.org](https://cmake.org/download) or VS 2019/2022 | `brew install cmake` | `sudo apt install cmake` |
| Compiler | Visual Studio 2019/2022 Community | `xcode-select --install` | `sudo apt install build-essential` |
| Python 3 | [python.org](https://www.python.org/downloads/) | `brew install python3` | `sudo apt install python3` |

---

## How to Run

```bash
# Windows
build\Release\strawhats.exe --data-dir data

# Linux / macOS (macOS eww)
./build/strawhats --data-dir data
```

The app starts with a top-level menu:

```
=== Straw Hats ChocAn ===

  [1] Provider Terminal
  [2] Manager Terminal
  [3] Operator Terminal
  [q] Quit
```

---

## Terminals

### Provider Terminal

Login, member validation, service record submission, and provider directory requests.

```
Provider # (9 digits, 'q' to quit): 123456789
PIN (4 digits): 1234
[LOGIN_SUCCESS] Login successful.

  [1] Submit Service Record
  [2] Request Provider Directory (UC05)
  [q] Log out
  Choice: 1

  Member # (9 digits, 'b' back): 111111111
  [MEMBER_OK] Member is validated.

  Date of service (MM-DD-YYYY, 'b' retry member): 04-21-2026
  [DATE_OK] Date accepted.

  Service code (6 digits, 'b' retry date): 100001
  [CODE_OK] Service confirmed.

  [SERVICE_FOUND] Dietitian Consultation — fee: $50.00
  Confirm service? (y/n): y

  Comments (press Enter to skip, max 100 chars): Annual checkup
  [RECORD_SAVED] Service record saved successfully.
```

At any prompt: `b` = go back one step, `q` = log out.

### Manager Terminal

Triggers the weekly batch run. Generates all 5 report files for a 7-day window.

```
  [1] Run Weekly Batch (UC04)
  [q] Quit
  Choice: 1

  Week ending date (MM-DD-YYYY, Enter = today): 04-21-2026

  [BATCH_OK] Reports generated in: data/output/reports/week_20260421
```

Output files under `data/output/reports/week_<YYYYMMDD>/`:
- `weekly_summary.txt`
- `provider_<number>.txt` — one per provider with activity
- `member_<number>.txt` — one per member with activity
- `ap_summary.txt`
- `eft_output.txt`

### Operator Terminal

Add, update, or delete members and providers. Import Acme nightly status updates.

```
  [1] Manage Members
  [2] Manage Providers
  [3] Import Acme Update File
  [q] Quit
```

CRUD operations write back to CSV immediately. Acme import format (`acme_updates.csv`):
```
memberNumber,newStatus
111111111,VALIDATED
222222222,SUSPENDED
```
Unknown member numbers are skipped — the import never crashes.

---

## Running the Tests

Build the binary first, then run the automated test suite:

### Basic Usage (Recommended)
```bash
python tests/run_tests.py
```
- Uses default paths:
  - Binary: Auto-detected (`build/strawhats` on Linux/macOS, `build\Release\strawhats.exe` on Windows)
  - Data directory: `data`

### Advanced Usage (Optional Flags)
```bash
python tests/run_tests.py --binary <path/to/binary> --data-dir <path/to/data>
 ex: python tests/run_tests.py --data-dir data/test_alt
```
- `--binary <path>`: Specify a custom binary path (e.g., for debug builds or non-standard locations).
  - Example (Windows): `--binary build/Release/strawhats.exe`
  - Example (Linux/macOS): `--binary build/strawhats`
- `--data-dir <path>`: Specify a custom data directory (e.g., for testing with different CSV files).
  - Default entry: `data/..`
- Use when: using alternative data sets.

The runner covers 41 test cases:
- BB-PV-01 -> BB-PV-24 — all provider/validation black-box cases
- UC05-01 — provider directory generation
- UC04-01, UC04-02 — weekly batch (valid and invalid date)
- OP-MEM-01 -> OP-MEM-07 — member CRUD
- OP-PRV-01 -> OP-PRV-05 — provider CRUD
- OP-ACM-01, OP-ACM-02 — Acme import

Each test pipes scripted stdin to the binary and checks stdout for the exact locked tag string.
CSV files are snapshotted and restored between tests so runs are fully repeatable.

Output:
```
PASS  BB-PV-01      Login — success
PASS  BB-PV-02      Login — wrong PIN
...
All 41 tests passed.
```

Exit codes: `0` = all pass, `1` = failures, `2` = binary not found.

After the run, `tests/executed_test_plan.md` is auto-generated with actual results filled in.

---

## Data Files

All CSV files live in `data/` and are loaded at startup. **Do not change column order.**

| File | Header | Notes |
|------|--------|-------|
| `providers.csv` | `providerNumber,name,pin` | 9-digit number, 4-digit PIN |
| `members.csv` | `memberNumber,name,status` | status = `VALIDATED` or `SUSPENDED` |
| `services.csv` | `code,name,fee` | 6-digit code |
| `service_records.csv` | `timestamp,dateOfService,providerNumber,memberNumber,serviceCode,fee,comments` | auto-created on first save |
| `acme_updates.csv` | `memberNumber,newStatus` | used by Operator import |

All output files go under `data/output/`.

---

## Output Tags (locked — used for test evidence)

These strings must never change wording or punctuation — the test runner matches them exactly.

| Tag | Meaning |
|-----|---------|
| `[LOGIN_SUCCESS] Login successful.` | Provider authenticated |
| `[LOGIN_FAIL] Provider not found.` | |
| `[LOGIN_FAIL] Incorrect PIN.` | |
| `[LOGIN_FAIL] Invalid provider number (must be 9 digits).` | |
| `[LOGIN_FAIL] Invalid PIN (must be 4 digits).` | |
| `[MEMBER_OK] Member is validated.` | |
| `[MEMBER_SUSPENDED] Member is suspended.` | Blocks service entry |
| `[MEMBER_FAIL] Member not found.` | |
| `[MEMBER_FAIL] Invalid member number (must be 9 digits).` | |
| `[DATE_OK] Date accepted.` | |
| `[DATE_FAIL] Invalid date format. Use MM-DD-YYYY.` | Re-prompts |
| `[CODE_OK] Service confirmed.` | |
| `[CODE_FAIL] Invalid service code (must be 6 digits).` | Re-prompts |
| `[CODE_FAIL] Service code not found.` | Re-prompts |
| `[RECORD_SAVED] Service record saved successfully.` | |
| `[CANCELLED] Cancelled. No record saved.` | Confirm = n |
| `[RECORD_FAIL] Save failed.` | Write error |
| `[NAV] Back` | b pressed |
| `[NAV] Logout` | q pressed |
| `[DIR_OK] Provider Directory created: <path>` | |
| `[DIR_FAIL] Could not write directory. Check output folder.` | |
| `[BATCH_OK] Reports generated in: <folder>` | |
| `[BATCH_FAIL] Invalid date or could not create output folder.` | |
| `[CRUD_OK] ...` / `[CRUD_FAIL] ...` | Operator CRUD result |
| `[UPDATE_OK] ...` / `[UPDATE_FAIL] ...` | Acme import result |

---

## Architecture

```
main.cpp  (terminal selector)
    |
    +-- ProviderTerminalUI
    |     +-- ProviderLoginController      -> ProviderRepository      -> providers.csv
    |     +-- MemberValidationController   -> MemberRepository        -> members.csv
    |     +-- ServiceEntryController       -> ServiceRepository       -> services.csv
    |     |                               -> ServiceRecordRepository  -> service_records.csv
    |     +-- ProviderDirectoryGenerator   -> ServiceRepository
    |
    +-- ManagerTerminalUI
    |     +-- WeeklyBatchGenerator         -> ServiceRecordRepository
    |                                      -> MemberRepository
    |                                      -> ProviderRepository
    |                                      -> ServiceRepository
    |
    +-- OperatorTerminalUI
          +-- OperatorController           -> MemberRepository   (read/write)
                                           -> ProviderRepository (read/write)
```

```
src/
  main.cpp
  models/         Member.h  Provider.h  ServiceCode.h  ServiceRecord.h
  repositories/   MemberRepository  ProviderRepository
                  ServiceRepository  ServiceRecordRepository
  controllers/    ProviderLoginController  MemberValidationController
                  ServiceEntryController   OperatorController
  generators/     ProviderDirectoryGenerator  WeeklyBatchGenerator
  ui/             ProviderTerminalUI  ManagerTerminalUI  OperatorTerminalUI
  util/           Csv  StringUtil
data/
  providers.csv
  members.csv
  services.csv
  acme_updates.csv
  service_records.csv       <- auto-created on first save
  output/                   <- auto-created at startup
    provider_directory_<YYYYMMDD>.txt
    reports/
      week_<YYYYMMDD>/
        weekly_summary.txt
        provider_<number>.txt
        member_<number>.txt
        ap_summary.txt
        eft_output.txt
tests/
  black_box_test_plan.md    <- 24 original test cases with locked expected output strings
  run_tests.py              <- automated runner (41 cases total)
  executed_test_plan.md     <- auto-generated after running run_tests.py
```

---

## Provider Session State Flow

```
Idle
 +- Enter Provider# + PIN
      +- [LOGIN_FAIL]  -> retry
      +- [LOGIN_SUCCESS]
           +- Session Menu
                +- [2] Provider Directory -> [DIR_OK] / [DIR_FAIL] -> menu
                +- [1] Submit Service Record
                      +- Enter Member#
                           +- [MEMBER_FAIL] / [MEMBER_SUSPENDED] -> retry
                           +- [MEMBER_OK]
                                +- Enter Date (MM-DD-YYYY)
                                     +- [DATE_FAIL] -> retry  |  b -> [NAV] Back -> member
                                     +- [DATE_OK]
                                          +- Enter Service Code
                                               +- [CODE_FAIL] -> retry  |  b -> [NAV] Back -> date
                                               +- [CODE_OK]
                                                    +- Confirm (y/n)
                                                         +- n -> [CANCELLED] -> menu
                                                         +- y -> Enter Comments (optional)
                                                                  +- [RECORD_SAVED] -> menu
```
