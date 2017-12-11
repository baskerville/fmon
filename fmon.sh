#! /bin/sh

SD_DIR=/mnt/onboard
FMON_BIN=/usr/local/fmon/bin/fmon
ICONS_DIR="${SD_DIR}/icons"
SCRIPTS_DIR="${SD_DIR}/.adds"

[ -e /mnt/onboard/.kobo/version ] || exit 1

for icon in "${ICONS_DIR}"/*.png; do
	app=$(basename "$icon" .png)
	"$FMON_BIN" "$icon" "${SCRIPTS_DIR}/${app}/${app}.sh" > "${SCRIPTS_DIR}/fmon/${app}.log" 2>&1 &
done
