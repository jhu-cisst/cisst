1. Repositories
===============

The cisst/SAW code is modular by nature. There is a main repository for the cisst libraries and then a repository per SAW component. To facilitate the code deployment, we also maintain some "meta" projects, each of them consisting of git submodules and a main CMakeLists. These "meta" projects allow to pull a subset of git repositories and provide a CMake configuration that can be tailored for a specific application.

The cisst/SAW code primary repositories are now hosted on github.com. To avoid clutter, the repositories are located in:

-  http://github.com/jhu-cisst

-  cisst.git, all the cisst libraries, recommended for users not interested in SAW components and applications

-  share.git, data used for some of the *cisst* examples and unit tests, this is a git submodule of cisst.git

-  cisst-saw.git, meta project containing cisst and most SAW modules, strongly recommended for cisst/SAW users

-  cisst-ros.git, SAW component and conversion functions used to create bridges between the cisst/SAW components and ROS

-  mechatronics-\*, projects related to the QLA-FPGA controller board

-  http://github.com/jhu-saw

-  Generic SAW components

-  Individual projects, all of them require the cisst libraries

-  If you decided to checkout the SAW components individually, you will need to build them against an existing cisst build tree

-  http://github.com/jhu-dvrk

-  SAW components specific to the da Vinci Research Kit

-  ROS files for the da Vinci Research Kit

.. _2-getting-the-latest-version:

2. Getting the latest version
=============================

The most useful repositories are:

-  http://github.com/jhu-cisst/cisst.git - *cisst* libraries, examples and test programs. Also adds *share* directory as a git submodule
-  http://github.com/jhu-cisst/cisst-saw.git. This repository is useless if the git submodules are not initialized and updated properly.

.. _21-command-line-linux-mac-os-x-:

2.1. Command line (Linux, Mac OS X, ...)
----------------------------------------

.. _cisst-without-the-data-eg-cisstgit:

*cisst* without the data (e.g. cisst.git)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  Initial clone

   .. code:: bash

      git clone https://github.com/jhu-cisst/cisst.git

-  Update your local copy

   .. code:: bash

      git pull origin main

*cisst* with the shared data
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  Initial clone

   .. code:: bash

      git clone https://github.com/jhu-cisst/cisst.git --recursive

-  Update your local copy

   .. code:: bash

      git pull origin main
      git submodule update --recursive

*cisst* and *SAW*
~~~~~~~~~~~~~~~~~

-  Initial clone

   .. code:: bash

      git clone https://github.com/jhu-cisst/cisst-saw.git --recursive

   If you forgot to add the ``--recursive`` options on the command line, you can go in the ``cisst-saw`` directory and retrieve the submodules using:

   .. code:: bash

      git submodule init
      git submodule update --recursive

-  Update your local copy

   .. code:: bash

      git pull origin main
      git submodule update --recursive

-  Update your local copy, if new submodules have been added

   .. code:: bash

      git pull origin main
      git submodule update --init --recursive

.. _22-windows-gui:

2.2. Windows GUI
----------------

There are many GUI based tools for git. You can find a list on http://git-scm.com/downloads/guis.

-  Some of the Windows clients only work on recent OSs, i.e. Windows 7 and 8.
-  On windows XP, we had some success with "Git Extensions". You can install the "Git Extensions" from https://code.google.com/p/gitextensions/.

If you are cloning a project with submodules, make sure you check the "Initialize all submodules"

.. _3-cisstsaw-developers:

3. cisst/SAW developers
=======================

Working with submodules can be a bit tricky. When you are working within a meta project and modify multiple submodules, you will need to:

-  Commit and push your changes in each individual submodule
-  Go the the parent project, i.e. the project containing the submodules
-  Add each modified submodule, i.e. ``git add <modified-module>``
-  Commit and push the meta project to reflect that the meta project now relies on a new version of the submodule(s)

If you omit the last two steps, the meta project will assume you don't want to use the updated submodules.

For a single submodule to update:

.. code:: bash

   git pull
   cd module1
   git checkout main
   git pull
   cd ..
   git status # to check if there is anything different
   git commit -a -m "use latest version of submodules"

For multiple modules, you can use the ``foreach`` syntax in git:

.. code:: bash

   git pull
   git submodule foreach git checkout main 
   git submodule foreach git pull
   git status # to check if there is anything different
   git commit -a -m "use latest version of submodules"
