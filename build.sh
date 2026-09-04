#!/bin/bash
set -e

ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DIST_DIR="$ROOT_DIR/dist"
CORES="$(nproc --all)"

echo "*** Compiling nx-memdump sysmodule ***"
TITLE_ID="$(grep -oP '"title_id":\s*"0x\K(\w+)' "$ROOT_DIR/sysmodule/perms.json")"

pushd "$ROOT_DIR/sysmodule" > /dev/null
make -j$CORES
popd > /dev/null

mkdir -p "$DIST_DIR/atmosphere/contents/$TITLE_ID/flags"
cp -vf "$ROOT_DIR/sysmodule/mem_dumper.nsp" "$DIST_DIR/atmosphere/contents/$TITLE_ID/exefs.nsp"
>"$DIST_DIR/atmosphere/contents/$TITLE_ID/flags/boot2.flag"

echo
echo "*** Compiling nx-memdump overlay ***"
pushd "$ROOT_DIR/overlay" > /dev/null
make -j$CORES
popd > /dev/null

mkdir -p "$DIST_DIR/switch/.overlays"
cp -vf "$ROOT_DIR/overlay/out/nx-memdump.ovl" "$DIST_DIR/switch/.overlays/nx-memdump.ovl"
echo
