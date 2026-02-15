#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<USAGE
Usage: $0 <input_dir> <output_dir> [skip_char]

Convert every *.edgelist file in <input_dir> into ThunderRW CSR files.
For each <name>.edgelist, CSR files are written to:
  <output_dir>/<name>/

Arguments:
  input_dir   Directory that contains .edgelist files
  output_dir  Directory where per-dataset CSR folders will be created
  skip_char   Optional skip/comment prefix character (default: #)

Requirements:
  - ThunderRW must be built, so ./build/toolset/EdgeList2CSR.out exists
USAGE
}

if [[ ${1:-} == "-h" || ${1:-} == "--help" ]]; then
  usage
  exit 0
fi

if [[ $# -lt 2 || $# -gt 3 ]]; then
  usage
  exit 1
fi

INPUT_DIR="$1"
OUTPUT_DIR="$2"
SKIP_CHAR="${3:-#}"

if [[ ! -d "$INPUT_DIR" ]]; then
  echo "Error: input directory does not exist: $INPUT_DIR" >&2
  exit 1
fi

if [[ ! -x "./build/toolset/EdgeList2CSR.out" ]]; then
  echo "Error: missing executable ./build/toolset/EdgeList2CSR.out" >&2
  echo "Please build ThunderRW first (mkdir -p build && cd build && cmake .. && make)." >&2
  exit 1
fi

mkdir -p "$OUTPUT_DIR"

shopt -s nullglob
edgelist_files=("$INPUT_DIR"/*.edgelist)
shopt -u nullglob

if [[ ${#edgelist_files[@]} -eq 0 ]]; then
  echo "No .edgelist files found in: $INPUT_DIR"
  exit 0
fi

tmp_root="$(mktemp -d)"
cleanup() {
  rm -rf "$tmp_root"
}
trap cleanup EXIT

count=0
for edgelist_path in "${edgelist_files[@]}"; do
  file_name="$(basename "$edgelist_path")"
  dataset_name="${file_name%.edgelist}"
  work_dir="$tmp_root/$dataset_name"
  dataset_output_dir="$OUTPUT_DIR/$dataset_name"

  mkdir -p "$work_dir" "$dataset_output_dir"

  # EdgeList2CSR expects the input edge list filename to be b_edge_list.bin.
  ln -sf "$edgelist_path" "$work_dir/b_edge_list.bin"

  echo "[$((++count))/${#edgelist_files[@]}] Converting $edgelist_path -> $dataset_output_dir"
  start_time="$(date +%s.%N)"
  ./build/toolset/EdgeList2CSR.out "$work_dir" "$dataset_output_dir" "$SKIP_CHAR"
  end_time="$(date +%s.%N)"

  elapsed_seconds="$(awk -v start="$start_time" -v end="$end_time" 'BEGIN { printf "%.3f", end - start }')"
  echo "*** Time to convert ${file_name} to CSR - ${elapsed_seconds} seconds ***"
done

echo "Done. Converted ${#edgelist_files[@]} file(s) into $OUTPUT_DIR"
