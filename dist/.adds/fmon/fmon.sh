#! /bin/sh

SD_DIR=/mnt/onboard
FMON_BIN=/usr/local/fmon/bin/fmon
ICONS_DIR="${SD_DIR}/apps"
SCRIPTS_DIR="${SD_DIR}/.adds"

[ -e /mnt/onboard/.kobo/version ] || exit 1

for app in koreader sketch; do
	[ -e "${ICONS_DIR}/${app}.png" ] || continue
	"$FMON_BIN" "${ICONS_DIR}/${app}.png" "${SCRIPTS_DIR}/${app}/${app}.sh" > "${SCRIPTS_DIR}/fmon/${app}.log" 2>&1 &
done
