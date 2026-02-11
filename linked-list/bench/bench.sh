#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd -- "$(dirname "$0")" && pwd)"
SRC="$ROOT/bench.cpp"
BIN="$ROOT/bin"
mkdir -p "$BIN"

OPS="${OPS:-20000}"
THREADS="${THREADS:-4}"
MODE="${MODE:-both}"   # single | multi | both

OUT="$BIN/bench_run"

run_impl() {
  local macro="$1"
  local label="$2"
  local extra="$3"
  g++ -std=c++20 -O2 -pthread "$SRC" $extra -o "$OUT" "-D${macro}"
  if [[ $label == "first" ]]; then
    "$OUT" --ops "$OPS" --threads "$THREADS" --mode "$MODE"
  else
    "$OUT" --ops "$OPS" --threads "$THREADS" --mode "$MODE" | awk 'NR==1{next} {print}'
  fi
}

echo "Running all implementations (ops=${OPS}, threads=${THREADS}, mode=${MODE})"
echo "# CSV: name,mode,threads,ops_per_thread,attempts,seconds,ops_per_sec,max_rss_kb"

run_impl IMPL_C_STYLE first "../attempt0-c-style/lib.cpp"
run_impl IMPL_CPP rest ""
run_impl IMPL_MUTEX rest ""
run_impl IMPL_SHARE_MUTEX rest ""
run_impl IMPL_ATOMIC rest ""
