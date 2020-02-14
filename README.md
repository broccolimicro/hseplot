# plot
Render state graphs, petri nets, and signal transition graphs as images.

**Usage**: `plot [options] file...`

The input file should be labeled with the appropriate type:
 - file.hse
 - file.astg
 - file.net

**Options**:
 - `-h`,`--help`      Display this information
 -    `--version`   Display version information
 - `-v`,`--verbose`   Display verbose messages
 - `-d`,`--debug`     Display internal debugging messages
 - `-o`             Specify the output file name
    formats other than 'dot' are passed onto graphviz dot for rendering
 - `-s`,`--sync`		Render half synchronization actions

## License

This project is part of the Haystack synthesis engine.

Licensed by Cornell University under GNU GPL v3.

Written by Ned Bingham.
Copyright © 2020 Cornell University.

Haystack is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Haystack is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License may be found in COPYRIGHT.
Otherwise, see <https://www.gnu.org/licenses/>.

