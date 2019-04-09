# mGNCS

mGNCS provides a new control set for MiniGUI application.

This is the mainline release of mGNCS for MiniGUI V4.0.x or later.

## Prerequisites

  * MiniGUI: v4.0.0 or later
  * mGUtils: v1.2.2 or later
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

    Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.

