# Building the WiUS firmware

This guide will show you how to build the WiUS firmware for the SiWG917 SoC.

## Prerequisites

Please read the [Getting Started](../siwg917_getting_started/siwg917_getting_started.md) guide to set up your development environment.

## Importing the project

To import the firmware, do the following:

1. Open Simplicity Studio.
2. Click on `File` -> `Import`.
3. Choose `More Import Options...` at the bottom of the opened window.
4. Select `General` -> `Existing Projects into Workspace`.
5. Under `Select root directory`, navigate to the [`wius_firmware/`](../../../wius_firmware/) subfolder of this repository.
6. Under `Projects`, the `wius_firmware` project should appear. Make sure it is checked.
7. Under `Options`, make sure nothing is checked.
8. Click `Finish`.

The project should now be imported into Simplicity Studio and should appear in the Project Explorer.

## Building the project

To build the project, do the following:

1. Right-click on the `wius_firmware` project in the Project Explorer.
2. Click on `Build Project`.

The project should now build successfully.

## Flashing the firmware

To flash the firmware, do the following:

1. Connect the board in question to your computer.
2. Right-click on the `wius_firmware` project in the Project Explorer.
3. Click on `Run As` -> `1 Silicon Labs ARM Program`.

The firmware should now be flashed to the development board.