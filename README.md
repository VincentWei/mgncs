# mGNCS

mGNCS provides a new control set for MiniGUI application.
By using mGNCS, you can use miniStudio to develop MiniGUI
application in WYSIWYG way. 

This is the mainline release of mGNCS for MiniGUI V3.2.x or later.

## Prerequisites

    * MiniGUI: v3.2.10 or later
    * mGUtils: v1.2.0 or later
    * mGPlus: v1.4.0 or later
    * SQLite3 (optional)
    * XML2 (optional)

## Building

mGNCS uses GNU autoconf/automake scripts to configure and build the project.

Run

    $ ./configure; make; sudo make install

to configure, make, and install the headers and the libraries (libmgncs).
The samples in `samples/` sub directory will be built as well.

mGNCS also provides some configuration options to customize the features.
For more information, please run

    $ ./configure --help

## Copying

Copyright (C) 2008 ~ 2018, Beijing FMSoft Technologies Co., Ltd.

