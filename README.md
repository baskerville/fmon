## Description

A daemon for launching programs on the Kobo e-readers.

## Installation

In each of the following situations, your device will automatically reboot after having been disconnected.

### Initial

```
cp -r dist/ SD_ROOT
```

### Firmware Upgrade

```
cp dist/.kobo/KoboRoot.tgz SD_ROOT/.kobo/
```

## Adding an app

Here's how you might add an icon for a program called *chess*:

- Edit `SD_ROOT/.adds/fmon/fmon.sh` and insert *chess* into the list of apps (`for app in koreader sketch chess; do`…).
- Create `SD_ROOT/icons/chess.png`.
- Create `SD_ROOT/.adds/chess/chess.sh`.

After rebooting, open the *chess* icon and close it (this will finalize the thumbnail creation process).

You can then open the *chess* icon to launch its script.

## Replacing an existing icon

In order to replace an existing icon, you must first remove the icon file within *nickel* (the built-in Kobo reader).

This will have the following effects:

- Stop the related *fmon* process.
- Remove the outdated thumbnails.

You can then connect your device and copy the updated icon in `SD_ROOT/icons`.
