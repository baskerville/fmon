## Description

A daemon for launching programs on the *Kobo* e-readers.

## Installation

```
unzip fmon-VERSION.zip -d SD_ROOT
```

This command also needs to be issued after each firmware update.

## Add a shortcut

To create a shortcut for a script named `plato.sh`, you should place the script in `SD_ROOT/.adds/plato` and the icon (named `plato.png`) in `SD_ROOT/icons`.

Add the icon to a collection, set the view to *List View* and open the icon.

On the next restart, opening the icon will execute the corresponding script.

## Remove a shortcut

Remove the corresponding icon within *Kobo*'s environment.

## Replace an icon

Instead of overwriting the existing icon, you'll need to remove it first, within *Kobo*'s environment.
