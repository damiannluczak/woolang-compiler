#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT_DIR/main"

PROG_DIR="$ROOT_DIR/tests/programs"
EXP_DIR="$ROOT_DIR/tests/expected/eval"
BUILD_DIR="$ROOT_DIR/tests/_build/eval"

mkdir -p "$BUILD_DIR"

fail=0
total=0

for f in "$PROG_DIR"/*.woo; do
  name="$(basename "$f" .woo)"
  echo "=== EVAL: $name ==="
  total=$((total+1))

  exp_out="$EXP_DIR/$name.out"
  exp_rc="$EXP_DIR/$name.rc"

  if [[ ! -f "$exp_out" || ! -f "$exp_rc" ]]; then
    echo "  MISSING expected: $exp_out or $exp_rc"
    fail=$((fail+1))
    echo
    continue
  fi

  out="$BUILD_DIR/$name.stdout"
  rcfile="$BUILD_DIR/$name.rc"

  "$BIN" "$f" --eval >"$out"
  echo "$?" >"$rcfile"

  if diff -u "$exp_out" "$out" >/dev/null; then
    echo "  stdout: OK"
  else
    echo "  stdout: FAIL"
    diff -u "$exp_out" "$out" || true
    fail=$((fail+1))
  fi

  if diff -u "$exp_rc" "$rcfile" >/dev/null; then
    echo "  exit_code: OK ($(cat "$rcfile"))"
  else
    echo "  exit_code: FAIL"
    echo "  expected=$(cat "$exp_rc") got=$(cat "$rcfile")"
    fail=$((fail+1))
  fi

  echo
done

echo "=== SUMMARY (EVAL) ==="
echo "total=$total fail=$fail"
exit "$fail"