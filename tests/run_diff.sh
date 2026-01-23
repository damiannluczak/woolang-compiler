#!/usr/bin/env bash
set -u

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PROG_DIR="$ROOT_DIR/tests/programs"
BUILD_EVAL="$ROOT_DIR/tests/_build/eval"
BUILD_CG="$ROOT_DIR/tests/_build/codegen"

fail=0
total=0

mkdir -p "$BUILD_EVAL" "$BUILD_CG"

for f in "$PROG_DIR"/*.woo; do
  name="$(basename "$f" .woo)"
  total=$((total+1))

  echo "=== DIFF (eval vs codegen): $name ==="

  e_out="$BUILD_EVAL/$name.stdout"
  e_rc="$BUILD_EVAL/$name.rc"
  c_out="$BUILD_CG/$name.stdout"
  c_rc="$BUILD_CG/$name.rc"

  if [ ! -f "$e_out" ] || [ ! -f "$e_rc" ] || [ ! -f "$c_out" ] || [ ! -f "$c_rc" ]; then
    echo "  MISSING build outputs (run run_eval.sh and run_codegen.sh first)"
    fail=$((fail+1))
    echo
    continue
  fi

  # 1) stdout: porównuj po odfiltrowaniu "Program result = ..."
  tmp_eval="$BUILD_EVAL/$name.stdout.filtered"
  sed '/^Program result = /d' "$e_out" > "$tmp_eval"

  if diff -u "$tmp_eval" "$c_out" >/dev/null; then
    echo "  stdout: OK"
  else
    echo "  stdout: FAIL"
    diff -u "$tmp_eval" "$c_out" || true
    fail=$((fail+1))
  fi

  # 2) exit_code
  if diff -u "$e_rc" "$c_rc" >/dev/null; then
    echo "  exit_code: OK"
  else
    echo "  exit_code: FAIL"
    diff -u "$e_rc" "$c_rc" || true
    fail=$((fail+1))
  fi

  echo
done

echo "=== SUMMARY (DIFF) ==="
echo "total=$total fail=$fail"
exit $((fail!=0))