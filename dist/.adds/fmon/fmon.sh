#! /bin/sh

SD_DIR=/mnt/onboard
FMON_BIN=/usr/local/fmon/bin/fmon
ICONS_DIR="${SD_DIR}/apps"
SCRIPTS_DIR="${SD_DIR}/.adds"
LOGFILE="${SD_DIR}/.adds/fmon/fmon.log"

[ -e /mnt/onboard/.kobo/version ] || exit 1
[ -e "$LOGFILE" ] && rm "$LOGFILE"

for app in koreader sketch; do
	"$FMON_BIN" "${ICONS_DIR}/${app}.png" "${SCRIPTS_DIR}/${app}/${app}.sh" >> "$LOGFILE" 2>&1 &
done
