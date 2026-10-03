#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
ARCH="${1:-armv7hl}"
case "$ARCH" in armv7hl|aarch64) ;; *) echo 'Usage: ./build_sign.sh [armv7hl|aarch64] [--sign] [--deploy]' >&2; exit 2;; esac
SDK_ROOT="${SDK_ROOT:-$HOME/Documents/SDKs/AuroraSDK-5.2.1.200-MB2}"
SFDK="${SFDK:-$SDK_ROOT/bin/sfdk}"
TARGET="${TARGET:-AuroraOS-5.2.1.200-$ARCH}"
SIGNING_KEY="${SIGNING_KEY:-$SDK_ROOT/package-signing/regular_key.pem}"
SIGNING_CERT="${SIGNING_CERT:-$SDK_ROOT/package-signing/regular_cert.pem}"
DEVICE_HOST="${DEVICE_HOST:-192.168.1.247}"
DEVICE_USER="${DEVICE_USER:-defaultuser}"
DEVICE_KEY="${DEVICE_KEY:-$HOME/.ssh/id_rsa_no_key}"
OUTPUT_DIR="${OUTPUT_DIR:-$PWD/artifacts}"
SIGN=0
DEPLOY=0
for arg in "$@"; do
  case "$arg" in --sign) SIGN=1;; --deploy) SIGN=1; DEPLOY=1;; esac
done
mkdir -p work
"$SFDK" -c "target=$TARGET" build 2>&1 | tee "work/build-$ARCH.log"
RPM="$(find RPMS -maxdepth 1 -name "ru.erhoof.scummvm-*.$ARCH.rpm" -printf '%T@ %p\n' | sort -nr | head -1 | cut -d' ' -f2-)"
test -n "$RPM"
if [ "$SIGN" = 1 ]; then
  "$SFDK" engine exec rpmsign-external sign --force -k "$SIGNING_KEY" -c "$SIGNING_CERT" "$PWD/$RPM"
  "$SFDK" engine exec rpmsign-external verify "$PWD/$RPM"
fi
"$SFDK" -c "target=$TARGET" check "$RPM" 2>&1 | tee "work/check-$ARCH.log"
# sfdk clears previous RPMs when switching targets. Preserve validated output.
mkdir -p "$OUTPUT_DIR"
cp -p "$RPM" "$OUTPUT_DIR/$(basename "$RPM")"
RPM="$OUTPUT_DIR/$(basename "$RPM")"
if [ "$DEPLOY" = 1 ]; then
  scp -O -i "$DEVICE_KEY" "$RPM" "$DEVICE_USER@$DEVICE_HOST:$(basename "$RPM")"
  ssh -i "$DEVICE_KEY" "$DEVICE_USER@$DEVICE_HOST" "sdk-deploy-rpm --silent --keepUserData '/home/$DEVICE_USER/$(basename "$RPM")'"
fi
echo "RPM ready: $RPM"
