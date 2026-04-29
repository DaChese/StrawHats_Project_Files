#!/usr/bin/env python3
#
# Project: ChocAn Data Center System (Straw Hats)
#
# Team + Ownership:
# - Aldo Medina    (UC02 / Submit Service Record; Provider Terminal flow + record save)
# - Lulu Chen      (UC01 / Validate Member Eligibility; Data Management support)
# - Audity Moumi   (UC04 / Weekly Batch + Reports + EFT outputs)
# - Rania Dayekh   (UC05 / Provider Directory generation)
#
# Created on:   2025-04-15
# Last updated: 2025-04-24
# Purpose: Black-box test runner — pipes scripted stdin to the binary and checks locked output tags.
#
# Primary owner: all (shared test infrastructure)
#
# Usage:
#   python tests/run_tests.py [--binary <path>] [--data-dir <path>]
#
# Exit codes:
#   0  All tests passed
#   1  One or more tests failed
#   2  Binary not found

import argparse
import os
import shutil
import subprocess
import sys
import tempfile
from dataclasses import dataclass, field


# ── TestCase ──────────────────────────────────────────────────────────────────

@dataclass
class TestCase:
    id: str
    name: str
    stdin: str
    expected_tag: str
    mutates_csv: bool = False


# ── Binary detection ──────────────────────────────────────────────────────────

def default_binary() -> str:
    if sys.platform == "win32":
        return os.path.join("build", "Release", "strawhats.exe")
    return os.path.join("build", "strawhats")


# ── Fixture helpers ───────────────────────────────────────────────────────────

def snapshot(paths: list) -> dict:
    """Read raw bytes from each path that exists."""
    saved = {}
    for p in paths:
        if os.path.exists(p):
            with open(p, "rb") as f:
                saved[p] = f.read()
        else:
            saved[p] = None  # file didn't exist — restore by deleting
    return saved


def restore(saved: dict) -> None:
    """Write saved bytes back to each path."""
    for p, data in saved.items():
        if data is None:
            if os.path.exists(p):
                os.remove(p)
        else:
            with open(p, "wb") as f:
                f.write(data)


# ── Tag matching ──────────────────────────────────────────────────────────────

def tag_matches(stdout: str, tag: str) -> bool:
    """Exact substring match — no regex, no case folding, no trimming."""
    return tag in stdout


# ── Test runner ───────────────────────────────────────────────────────────────

def run_test(tc: TestCase, binary: str, data_dir: str) -> tuple:
    """Returns (passed: bool, actual_stdout: str)."""
    try:
        result = subprocess.run(
            [binary, "--data-dir", data_dir],
            input=tc.stdin,
            capture_output=True,
            text=True,
            timeout=10,
        )
        passed = tag_matches(result.stdout, tc.expected_tag)
        return passed, result.stdout
    except subprocess.TimeoutExpired:
        return False, "<TIMEOUT: process did not complete within 10 seconds>"
    except Exception as e:
        return False, f"<ERROR: {e}>"


# ── Summary ───────────────────────────────────────────────────────────────────

def format_summary(results: list) -> tuple:
    """Returns (summary_line: str, exit_code: int)."""
    n = len(results)
    failures = sum(1 for r in results if not r)
    if failures == 0:
        return f"All {n} tests passed.", 0
    return f"{failures} of {n} tests FAILED.", 1


# ── Test cases ────────────────────────────────────────────────────────────────

# Helper: build a full stdin sequence that selects Provider Terminal,
# logs in, navigates to service entry, and quits cleanly.
def _prov_login(extra="q\nq\n"):
    """Login as 123456789/1234 then append extra navigation."""
    return f"1\n123456789\n1234\n{extra}q\n"

def _prov_service(member, date, code, confirm, comments=""):
    """Full service entry flow after login."""
    return _prov_login(f"1\n{member}\n{date}\n{code}\n{confirm}\n{comments}\nq\n")


TEST_CASES = [

    # ── BB-PV-01..06 Login ────────────────────────────────────────────────────
    TestCase(
        id="BB-PV-01", name="Login — success",
        stdin="1\n123456789\n1234\nq\nq\nq\n",
        expected_tag="[LOGIN_SUCCESS] Login successful.",
    ),
    TestCase(
        id="BB-PV-02", name="Login — wrong PIN",
        stdin="1\n123456789\n0000\nq\nq\n",
        expected_tag="[LOGIN_FAIL] Incorrect PIN.",
    ),
    TestCase(
        id="BB-PV-03", name="Login — not found",
        stdin="1\n000000000\n1234\nq\nq\n",
        expected_tag="[LOGIN_FAIL] Provider not found.",
    ),
    TestCase(
        id="BB-PV-04", name="Login — bad number (short)",
        stdin="1\n12345\n1234\nq\nq\n",
        expected_tag="[LOGIN_FAIL] Invalid provider number (must be 9 digits).",
    ),
    TestCase(
        id="BB-PV-05", name="Login — bad number (non-digit)",
        stdin="1\n12345678A\n1234\nq\nq\n",
        expected_tag="[LOGIN_FAIL] Invalid provider number (must be 9 digits).",
    ),
    TestCase(
        id="BB-PV-06", name="Login — bad PIN (short)",
        stdin="1\n123456789\n12\nq\nq\n",
        expected_tag="[LOGIN_FAIL] Invalid PIN (must be 4 digits).",
    ),

    # ── BB-PV-07..10 Member validation ───────────────────────────────────────
    TestCase(
        id="BB-PV-07", name="Member — validated",
        stdin="1\n123456789\n1234\n1\n111111111\nb\nq\nq\n",
        expected_tag="[MEMBER_OK] Member is validated.",
    ),
    TestCase(
        id="BB-PV-08", name="Member — suspended",
        stdin="1\n123456789\n1234\n1\n222222222\nb\nq\nq\n",
        expected_tag="[MEMBER_SUSPENDED] Member is suspended.",
    ),
    TestCase(
        id="BB-PV-09", name="Member — not found",
        stdin="1\n123456789\n1234\n1\n999999998\nb\nq\nq\n",
        expected_tag="[MEMBER_FAIL] Member not found.",
    ),
    TestCase(
        id="BB-PV-10", name="Member — bad input (short)",
        stdin="1\n123456789\n1234\n1\n1234\nb\nq\nq\n",
        expected_tag="[MEMBER_FAIL] Invalid member number (must be 9 digits).",
    ),

    # ── BB-PV-11..14 Date validation ─────────────────────────────────────────
    TestCase(
        id="BB-PV-11", name="Date — bad month",
        stdin="1\n123456789\n1234\n1\n111111111\n13-01-2026\nb\nb\nq\nq\n",
        expected_tag="[DATE_FAIL] Invalid date format. Use MM-DD-YYYY.",
    ),
    TestCase(
        id="BB-PV-12", name="Date — wrong separator",
        stdin="1\n123456789\n1234\n1\n111111111\n04/21/2026\nb\nb\nq\nq\n",
        expected_tag="[DATE_FAIL] Invalid date format. Use MM-DD-YYYY.",
    ),
    TestCase(
        id="BB-PV-13", name="Date — non-digit",
        stdin="1\n123456789\n1234\n1\n111111111\nhello\nb\nb\nq\nq\n",
        expected_tag="[DATE_FAIL] Invalid date format. Use MM-DD-YYYY.",
    ),
    TestCase(
        id="BB-PV-14", name="Date — accepted",
        stdin="1\n123456789\n1234\n1\n111111111\n04-21-2026\nb\nb\nq\nq\n",
        expected_tag="[DATE_OK] Date accepted.",
    ),

    # ── BB-PV-15..17 Service code ─────────────────────────────────────────────
    TestCase(
        id="BB-PV-15", name="Code — too short",
        stdin="1\n123456789\n1234\n1\n111111111\n04-21-2026\n1234\nb\nb\nb\nq\nq\n",
        expected_tag="[CODE_FAIL] Invalid service code (must be 6 digits).",
    ),
    TestCase(
        id="BB-PV-16", name="Code — not found",
        stdin="1\n123456789\n1234\n1\n111111111\n04-21-2026\n999999\nb\nb\nb\nq\nq\n",
        expected_tag="[CODE_FAIL] Service code not found.",
    ),
    TestCase(
        id="BB-PV-17", name="Code — accepted",
        stdin="1\n123456789\n1234\n1\n111111111\n04-21-2026\n100001\nn\nq\nq\n",
        expected_tag="[CODE_OK] Service confirmed.",
    ),

    # ── BB-PV-18..20 Confirm / save ───────────────────────────────────────────
    TestCase(
        id="BB-PV-18", name="Cancel — no record written",
        stdin="1\n123456789\n1234\n1\n111111111\n04-21-2026\n100001\nn\nq\nq\n",
        expected_tag="[CANCELLED] Cancelled. No record saved.",
    ),
    TestCase(
        id="BB-PV-19", name="Happy path — record saved",
        stdin="1\n123456789\n1234\n1\n111111111\n04-21-2026\n100001\ny\nAnnual checkup\nq\nq\n",
        expected_tag="[RECORD_SAVED] Service record saved successfully.",
        mutates_csv=True,
    ),
    TestCase(
        id="BB-PV-20", name="Happy path — blank comments",
        stdin="1\n123456789\n1234\n1\n111111111\n04-21-2026\n100001\ny\n\nq\nq\n",
        expected_tag="[RECORD_SAVED] Service record saved successfully.",
        mutates_csv=True,
    ),

    # ── BB-PV-21..24 Navigation ───────────────────────────────────────────────
    TestCase(
        id="BB-PV-21", name="Navigation — back at member",
        stdin="1\n123456789\n1234\n1\nb\nq\nq\n",
        expected_tag="[NAV] Back",
    ),
    TestCase(
        id="BB-PV-22", name="Navigation — back at date",
        stdin="1\n123456789\n1234\n1\n111111111\nb\nq\nq\n",
        expected_tag="[NAV] Back",
    ),
    TestCase(
        id="BB-PV-23", name="Navigation — back at code",
        stdin="1\n123456789\n1234\n1\n111111111\n04-21-2026\nb\nq\nq\n",
        expected_tag="[NAV] Back",
    ),
    TestCase(
        id="BB-PV-24", name="Navigation — logout from menu",
        stdin="1\n123456789\n1234\nq\nq\nq\n",
        expected_tag="[NAV] Logout",
    ),

    # ── UC05-01 Provider Directory ────────────────────────────────────────────
    TestCase(
        id="UC05-01", name="Directory generation — success",
        stdin="1\n123456789\n1234\n2\nq\nq\nq\n",
        expected_tag="[DIR_OK] Provider Directory created:",
    ),

    # ── UC04-01..02 Weekly Batch ──────────────────────────────────────────────
    TestCase(
        id="UC04-01", name="Batch — valid date",
        stdin="2\n1\n04-21-2025\nq\nq\n",
        expected_tag="[BATCH_OK] Reports generated in:",
    ),
    TestCase(
        id="UC04-02", name="Batch — invalid date",
        stdin="2\n1\nnot-a-date\nq\nq\n",
        expected_tag="[BATCH_FAIL] Invalid date or could not create output folder.",
    ),

    # ── OP-MEM-01..07 Member CRUD ─────────────────────────────────────────────
    TestCase(
        id="OP-MEM-01", name="Member add — success",
        stdin="3\n1\n1\n900000001\nTest Member\nVALIDATED\nb\nq\nq\n",
        expected_tag="[CRUD_OK] Member 900000001 added.",
        mutates_csv=True,
    ),
    TestCase(
        id="OP-MEM-02", name="Member add — duplicate",
        stdin="3\n1\n1\n111111111\nAlice Valid\nVALIDATED\nb\nq\nq\n",
        expected_tag="[CRUD_FAIL] Member 111111111 already exists.",
        mutates_csv=True,
    ),
    TestCase(
        id="OP-MEM-03", name="Member add — invalid number",
        stdin="3\n1\n1\n12345\nBad Member\nVALIDATED\nb\nq\nq\n",
        expected_tag="[CRUD_FAIL] Member number must be 9 digits.",
        mutates_csv=True,
    ),
    TestCase(
        id="OP-MEM-04", name="Member update — success",
        stdin="3\n1\n2\n111111111\nUpdated Name\nSUSPENDED\nb\nq\nq\n",
        expected_tag="[CRUD_OK] Member 111111111 updated.",
        mutates_csv=True,
    ),
    TestCase(
        id="OP-MEM-05", name="Member update — not found",
        stdin="3\n1\n2\n000000001\nGhost\nVALIDATED\nb\nq\nq\n",
        expected_tag="[CRUD_FAIL] Member 000000001 not found.",
        mutates_csv=True,
    ),
    TestCase(
        id="OP-MEM-06", name="Member delete — success",
        stdin="3\n1\n3\n111111111\nb\nq\nq\n",
        expected_tag="[CRUD_OK] Member 111111111 deleted.",
        mutates_csv=True,
    ),
    TestCase(
        id="OP-MEM-07", name="Member delete — not found",
        stdin="3\n1\n3\n000000001\nb\nq\nq\n",
        expected_tag="[CRUD_FAIL] Member 000000001 not found.",
        mutates_csv=True,
    ),

    # ── OP-PRV-01..05 Provider CRUD ───────────────────────────────────────────
    TestCase(
        id="OP-PRV-01", name="Provider add — success",
        stdin="3\n2\n1\n900000002\nTest Provider\n9999\nb\nq\nq\n",
        expected_tag="[CRUD_OK] Provider 900000002 added.",
        mutates_csv=True,
    ),
    TestCase(
        id="OP-PRV-02", name="Provider add — duplicate",
        stdin="3\n2\n1\n123456789\nTest Provider\n1234\nb\nq\nq\n",
        expected_tag="[CRUD_FAIL] Provider 123456789 already exists.",
        mutates_csv=True,
    ),
    TestCase(
        id="OP-PRV-03", name="Provider add — invalid PIN",
        stdin="3\n2\n1\n900000003\nBad PIN Provider\n12\nb\nq\nq\n",
        expected_tag="[CRUD_FAIL] PIN must be 4 digits.",
        mutates_csv=True,
    ),
    TestCase(
        id="OP-PRV-04", name="Provider update — success",
        stdin="3\n2\n2\n123456789\nUpdated Provider\n5678\nb\nq\nq\n",
        expected_tag="[CRUD_OK] Provider 123456789 updated.",
        mutates_csv=True,
    ),
    TestCase(
        id="OP-PRV-05", name="Provider delete — success",
        stdin="3\n2\n3\n123456789\nb\nq\nq\n",
        expected_tag="[CRUD_OK] Provider 123456789 deleted.",
        mutates_csv=True,
    ),

    # ── OP-ACM-01..02 Acme Import ─────────────────────────────────────────────
    TestCase(
        id="OP-ACM-01", name="Acme import — default path success",
        stdin="3\n3\n\nq\nq\n",
        expected_tag="[UPDATE_OK] Import complete.",
        mutates_csv=True,
    ),
    TestCase(
        id="OP-ACM-02", name="Acme import — missing file",
        stdin="3\n3\ndata/nonexistent_file.csv\nq\nq\n",
        expected_tag="[UPDATE_FAIL] Cannot open update file:",
    ),
]


# ── Executed test plan generation ─────────────────────────────────────────────

def generate_executed_plan(results: list, output_path: str) -> None:
    """Write tests/executed_test_plan.md with Actual Results + Pass/Fail filled in."""
    lines = [
        "# Executed Test Plan — Straw Hats ChocAn\n",
        "\n",
        "## Locked Output Tag Reference\n",
        "\n",
        "| Tag | Exact string printed |\n",
        "|-----|----------------------|\n",
        "| Login success | `[LOGIN_SUCCESS] Login successful.` |\n",
        "| Login fail — not found | `[LOGIN_FAIL] Provider not found.` |\n",
        "| Login fail — wrong PIN | `[LOGIN_FAIL] Incorrect PIN.` |\n",
        "| Login fail — bad number | `[LOGIN_FAIL] Invalid provider number (must be 9 digits).` |\n",
        "| Login fail — bad PIN | `[LOGIN_FAIL] Invalid PIN (must be 4 digits).` |\n",
        "| Member OK | `[MEMBER_OK] Member is validated.` |\n",
        "| Member suspended | `[MEMBER_SUSPENDED] Member is suspended.` |\n",
        "| Member not found | `[MEMBER_FAIL] Member not found.` |\n",
        "| Member bad input | `[MEMBER_FAIL] Invalid member number (must be 9 digits).` |\n",
        "| Date accepted | `[DATE_OK] Date accepted.` |\n",
        "| Date rejected | `[DATE_FAIL] Invalid date format. Use MM-DD-YYYY.` |\n",
        "| Code accepted | `[CODE_OK] Service confirmed.` |\n",
        "| Code bad format | `[CODE_FAIL] Invalid service code (must be 6 digits).` |\n",
        "| Code not found | `[CODE_FAIL] Service code not found.` |\n",
        "| Record saved | `[RECORD_SAVED] Service record saved successfully.` |\n",
        "| Cancelled | `[CANCELLED] Cancelled. No record saved.` |\n",
        "| Save failed | `[RECORD_FAIL] Save failed.` |\n",
        "| Back | `[NAV] Back` |\n",
        "| Logout | `[NAV] Logout` |\n",
        "| Directory OK | `[DIR_OK] Provider Directory created: <path>` |\n",
        "| Directory fail | `[DIR_FAIL] Could not write directory. Check output folder.` |\n",
        "| Batch OK | `[BATCH_OK] Reports generated in: <folder>` |\n",
        "| Batch fail | `[BATCH_FAIL] Invalid date or could not create output folder.` |\n",
        "| CRUD OK | `[CRUD_OK] ...` |\n",
        "| CRUD fail | `[CRUD_FAIL] ...` |\n",
        "| Update OK | `[UPDATE_OK] ...` |\n",
        "| Update fail | `[UPDATE_FAIL] ...` |\n",
        "\n",
        "---\n",
        "\n",
        "## Test Results\n",
        "\n",
        "| ID | Category | Name | Expected Tag | Actual Results | Pass/Fail |\n",
        "|----|----------|------|-------------|----------------|-----------|\n",
    ]

    result_map = {tc_id: (passed, stdout) for tc_id, passed, stdout in results}

    for tc in TEST_CASES:
        passed, stdout = result_map.get(tc.id, (False, ""))
        pf = "PASS" if passed else "FAIL"
        # Extract the matching line from stdout for actual results
        actual = ""
        for line in stdout.splitlines():
            if tc.expected_tag in line:
                actual = line.strip()
                break
        if not actual and not passed:
            # Show first non-empty line of stdout as evidence
            for line in stdout.splitlines():
                if line.strip():
                    actual = line.strip()[:80]
                    break
        # Escape pipes in actual output
        actual = actual.replace("|", "\\|")
        expected_escaped = tc.expected_tag.replace("|", "\\|")
        lines.append(
            f"| {tc.id} | — | {tc.name} | `{expected_escaped}` | {actual} | {pf} |\n"
        )

    with open(output_path, "w", encoding="utf-8") as f:
        f.writelines(lines)


# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description="ChocAn black-box test runner")
    parser.add_argument("--binary", default=None,
                        help="Path to strawhats binary (auto-detected if omitted)")
    parser.add_argument("--data-dir", default="data",
                        help="Path to data directory (default: data)")
    parser.add_argument("--report", default="tests/executed_test_plan.md",
                        help="Path to write executed test plan (default: tests/executed_test_plan.md)")
    args = parser.parse_args()

    binary = args.binary or default_binary()
    data_dir = args.data_dir

    if not os.path.isfile(binary):
        print(f"ERROR: Binary not found: {binary}", file=sys.stderr)
        print("Build the project first (see README.md)", file=sys.stderr)
        sys.exit(2)

    # Paths that may be mutated by CRUD/import tests
    fixture_paths = [
        os.path.join(data_dir, "members.csv"),
        os.path.join(data_dir, "providers.csv"),
        os.path.join(data_dir, "service_records.csv"),
    ]

    results = []       # list of (id, passed, stdout)
    pass_flags = []    # list of bool for summary

    print(f"Running {len(TEST_CASES)} test cases against: {binary}\n")

    for tc in TEST_CASES:
        saved = snapshot(fixture_paths) if tc.mutates_csv else {}

        passed, stdout = run_test(tc, binary, data_dir)

        if tc.mutates_csv:
            restore(saved)

        pass_flags.append(passed)
        results.append((tc.id, passed, stdout))

        if passed:
            print(f"PASS  {tc.id:<12}  {tc.name}")
        else:
            print(f"FAIL  {tc.id:<12}  {tc.name}")
            print(f"  Expected tag : {tc.expected_tag}")
            # Show relevant stdout lines only
            relevant = [l for l in stdout.splitlines() if l.strip()][:10]
            print(f"  Actual stdout:")
            for line in relevant:
                print(f"    {line}")

    print()
    summary, exit_code = format_summary(pass_flags)
    print(summary)

    # Generate executed test plan
    try:
        generate_executed_plan(results, args.report)
        print(f"Executed test plan written to: {args.report}")
    except Exception as e:
        print(f"Warning: could not write test plan: {e}", file=sys.stderr)

    sys.exit(exit_code)


if __name__ == "__main__":
    main()
