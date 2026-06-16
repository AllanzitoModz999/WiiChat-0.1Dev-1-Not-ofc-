# WiiChat v0.1-dev1

A minimal Nintendo Wii homebrew application built with [devkitPro](https://devkitpro.org/) (libogc).

## Project structure

```
WiiChat/
├── source/
│   └── main.c                   # Application entry point
├── Makefile                     # devkitPro-compatible build file
└── .github/
    └── workflows/
        └── build.yml            # CI: builds boot.dol via devkitpro/devkitppc
```

## Features

- Displays a simple text UI on the Wii screen: **WiiChat v0.1-dev1**
- Wiimote (WPAD) input support
- Press **HOME** to exit back to the Homebrew Channel loader

## Building locally

### Prerequisites

- [devkitPro](https://devkitpro.org/wiki/Getting_Started) with **Wii development** selected
- `DEVKITPRO` environment variable set (e.g. `/opt/devkitpro`)

### Steps

```bash
export DEVKITPRO=/opt/devkitpro
export DEVKITPPC=$DEVKITPRO/devkitPPC
export PATH="$DEVKITPPC/bin:$DEVKITPRO/tools/bin:$PATH"

make
```

The output file `boot.dol` is placed in the project root.

## Running on hardware / Dolphin

- **Homebrew Channel** – copy `boot.dol` to `SD:/apps/wiichat/boot.dol`
- **Dolphin Emulator** – open `boot.dol` directly via *File → Open*

## CI

GitHub Actions automatically builds `boot.dol` on every push/PR using the
official `devkitpro/devkitppc` Docker image and uploads it as a workflow
artifact (`wiichat-boot-dol`).