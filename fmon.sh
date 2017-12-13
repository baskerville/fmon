#! /bin/sh

[ -e /mnt/onboard/.kobo/version ] || exit 1

FMON_HOME=/usr/local/fmon
FMON_BIN="${FMON_HOME}/bin/fmon"
SD_ROOT=/mnt/onboard
ICONS_DIR="${SD_ROOT}/icons"
SCRIPTS_DIR="${SD_ROOT}/.adds"

for icon in "${ICONS_DIR}"/*.png; do
	app=$(basename "$icon" .png)
	"$FMON_BIN" "$icon" "${SCRIPTS_DIR}/${app}/${app}.sh" > "${FMON_HOME}/${app}.log" 2>&1 &
done
