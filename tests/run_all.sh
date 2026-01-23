#!/usr/bin/env bash
set -euo pipefail

./tests/run_eval.sh
./tests/run_codegen.sh
./tests/run_diff.sh
echo "ALL OK"