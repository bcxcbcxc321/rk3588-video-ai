#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BIN="${ROOT_DIR}/build/rk_video_ai"

if [[ ! -x "${BIN}" ]]; then
  "${ROOT_DIR}/scripts/build/build.sh"
fi

exec "${BIN}" "$@"
