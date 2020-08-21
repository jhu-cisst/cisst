# Introduction

This directory contains some information to upgrade cisst/SAW components to CRTK.  See https://github.com/collaborative-robotics/documentation/wiki/Robot-API for more information regarding CRTK.

To partially automate the migration, we provided a simple shell script (`replace-symbols.sh`).   This script will recursively replace symbols in all `.cpp`, `.c`, `.h`, `.m` and `.py` files.  The script requires a dictionary stored in a text file using the following format:
```
oldString = newString
```

The script will not work for all the cisst/SAW components and commands since older components were not always following the same naming convention.  Porting will likely require some automated and manual changes.

# Instructions

The file `crtk-commands.dict` contains the command names that had been somewhat standardized across cisst/SAW and their CRTK equivalents.  They mostly address code used to populate `mtsInterfaceProvided` and `mtsInterfaceRequired` using the methods `AddCommand`/`AddEvent` and corresponding `AddFunction`/`AddEventHandler`.  For example, to update all code in the directory `~/catkin_ws/src/cisst-saw/sawIntuitiveResearchKit`

```sh
./replace-symbols.sh -d ~/catkin_ws/src/cisst-saw/sawIntuitiveResearchKit -s crtk-commands.dict
```

The same script is also used to replace other symbols in the code (e.g. methods and data members) so the naming matches closely the CRTK convention.  This doesn't change the code functionalities, it just makes it easier to read and maintain.  The list of substitutions should be kept along the code (for example in sub-directory `crtk-port`).  This can help to reverse the changes but can also be used to update code directly linking against the component (e.g. derived classes).
