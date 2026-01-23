#!/usr/bin/env bash
set -u

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT_DIR/main"

PROG_DIR="$ROOT_DIR/tests/programs"
EXP_DIR="$ROOT_DIR/tests/expected/codegen"
BUILD_DIR="$ROOT_DIR/tests/_build/codegen"

mkdir -p "$BUILD_DIR"

fail=0
total=0

for f in "$PROG_DIR"/*.woo; do
  name="$(basename "$f" .woo)"
  echo "=== CODEGEN: $name ==="
  total=$((total+1))

  exp_out="$EXP_DIR/$name.out"
  exp_rc="$EXP_DIR/$name.rc"

  if [[ ! -f "$exp_out" || ! -f "$exp_rc" ]]; then
    echo "  MISSING expected: $exp_out or $exp_rc"
    fail=$((fail+1))
    echo
    continue
  fi

  cfile="$BUILD_DIR/$name.c"
  binfile="$BUILD_DIR/$name.bin"
  out="$BUILD_DIR/$name.stdout"
  rcfile="$BUILD_DIR/$name.rc"

  # emit C
  if ! "$BIN" "$f" --emit-c > "$cfile"; then
    echo "  FAIL: emit-c"
    fail=$((fail+1))
    echo
    continue
  fi

  # compile generated C
  if ! gcc "$cfile" -o "$binfile"; then
    echo "  FAIL: gcc $cfile"
    fail=$((fail+1))
    echo
    continue
  fi

  # run binary (UWAGA: exit code może być != 0 i to jest OK)
  "$binfile" >"$out"
  rc=$?
  echo "$rc" >"$rcfile"

  if diff -u "$exp_out" "$out" >/dev/null; then
    echo "  stdout: OK"
  else
    echo "  stdout: FAIL"
    diff -u "$exp_out" "$out" || true
    fail=$((fail+1))
  fi

  if diff -u "$exp_rc" "$rcfile" >/dev/null; then
    echo "  exit_code: OK ($rc)"
  else
    echo "  exit_code: FAIL"
    echo "  expected=$(cat "$exp_rc") got=$rc"
    fail=$((fail+1))
  fi

  echo "  generated: $cfile"
  echo
done

echo "=== SUMMARY (CODEGEN) ==="
echo "total=$total fail=$fail"
exit $((fail!=0))