#!/usr/bin/env bash
set -u

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PROG_DIR="$ROOT_DIR/tests/programs"
EXP_EVAL_DIR="$ROOT_DIR/tests/expected/eval"
EXP_CG_DIR="$ROOT_DIR/tests/expected/codegen"
BUILD_DIR="$ROOT_DIR/tests/_build"

mkdir -p "$EXP_EVAL_DIR" "$EXP_CG_DIR" "$BUILD_DIR"

# helper: uruchom program, zapisz stdout + rc (NIE przerywaj na non-zero)
run_and_capture() {
  local cmd=("$@")
  local out_file="$1"
  local rc_file="$2"
  shift 2

  # stdout
  "$@" > "$out_file"
  local rc=$?
  echo "$rc" > "$rc_file"
  return 0
}

echo "ROOT_DIR=$ROOT_DIR"
echo "PROG_DIR=$PROG_DIR"
echo

shopt -s nullglob
files=("$PROG_DIR"/*.woo)
shopt -u nullglob

if [ ${#files[@]} -eq 0 ]; then
  echo "No .woo files found in $PROG_DIR"
  exit 1
fi

for f in "${files[@]}"; do
  name="$(basename "$f" .woo)"
  echo "MAKE expected: $name"

  # --- EVAL expected ---
  # (zapisujemy stdout i rc; nie przerywamy na non-zero)
  out_eval="$EXP_EVAL_DIR/$name.out"
  rc_eval="$EXP_EVAL_DIR/$name.rc"

  ./main "$f" --eval > "$out_eval"
  echo $? > "$rc_eval"

  # --- CODEGEN expected ---
  # generujemy C do _build, kompilujemy i uruchamiamy; zapisujemy stdout i rc
  mkdir -p "$BUILD_DIR/codegen/$name"
  c_file="$BUILD_DIR/codegen/$name/$name.c"
  bin_file="$BUILD_DIR/codegen/$name/$name.bin"

  ./main "$f" --emit-c > "$c_file"
  gcc "$c_file" -o "$bin_file"

  out_cg="$EXP_CG_DIR/$name.out"
  rc_cg="$EXP_CG_DIR/$name.rc"

  "$bin_file" > "$out_cg"
  echo $? > "$rc_cg"

  echo
done

echo "DONE."