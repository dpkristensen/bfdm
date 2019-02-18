# Overview

## Binary Format Data Modeler

This is a suite of tools to work with binary data streams that have a well-defined structure as a bit-stream or byte-stream.  The structure of this data is specified in BFSDL.

## Binary Format Stream Definition Language

This is a [Data Definition Language](https://en.wikipedia.org/wiki/Data_definition_language) that allows an object-oriented bit-stream to be defined in a human-readable format.

# Legal Stuff

Please read the Software License ([LICENSE.md](LICENSE.md)) and Contributor License Agreement ([CLA.md](CLA.md)).

# Building

Prior to building, make sure you have Python 3.4 or later installed.

## Environment Setup

To download external dependencies, run `./setup`.  The archive files will be saved in the `_out/archive` directory, and the libraries will be extracted to the `external` folder.  This only needs to be done ONCE.  To perform setup again, delete the files in `_out/archive` and directories in `external`.

## Configure waf

Run `./configure waf` to search the system for build dependencies.  See waf documentation for details.

## Building

To see build options, type `./waf -h`.  Always use the `build` waf command to build, along with `-p` and optionally `-m`; this will ensure the output folders get setup correctly.

For example:

    # Build win32 targets in development (default)
    ./waf -p win32

    # Build win64 targets in release
    ./waf -p win64 -m release
