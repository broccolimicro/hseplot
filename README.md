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
