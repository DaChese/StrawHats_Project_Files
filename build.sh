#!/usr/bin/env bash
set -e

echo ""
echo "=== Straw Hats ChocAn - Build ==="
echo ""

# ── 1. Find cmake ─────────────────────────────────────────────────────────────

if ! command -v cmake &>/dev/null; then
    echo "ERROR: cmake not found."
    echo ""
    echo "Install it:"
    echo "  macOS:          brew install cmake"
    echo "  Ubuntu/Debian:  sudo apt install cmake build-essential"
    echo "  Fedora:         sudo dnf install cmake gcc-c++"
    echo ""
    exit 1
fi

echo "Found cmake: $(cmake --version | head -1)"
echo ""

# ── 2. Configure ─────────────────────────────────────────────────────────────

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

echo ""

# ── 3. Build ──────────────────────────────────────────────────────────────────

cmake --build build

echo ""
echo "============================================================"
echo " Build successful!"
echo ""
echo " Run the app:"
echo "   ./build/strawhats --data-dir data"
echo "============================================================"
echo ""
