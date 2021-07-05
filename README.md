# FILESYSTEM Search Tool For Linux

In this repository you find the source code of the tool (search.c) and the binary as well.

The tool basically can perform file searching on a linux system.

The tool is based on stat.

# TOOL SPECIFICATIONS

[directory] : to specify in which directory you want to search for the file which optional, if no directory specified then tool will take by default the current directory (./). You can specify the directory using any type of paths (absolute or relative)

[options] : here we find a few interesting options to make your search more meaningful, this is optional.

-n : to specify depth level of your search, if nothing specified then it's 0 by default.
-d : to print creation date.
-m : to print the date of last modification of founded files if there are any.
-s : to print the size in bytes
-t : to print the type of the file
-p : to print access rights (protection) in the fashion of 'ls'  command
-a : to print all the different information from the above options

file_name : this is a required argument to specify the name of the file to search, file_name may contains wildcards (? and *).

# Usage

Before you run try to make sure that you have the execution privilege.

[path]/search [directory] [options] file_name

While using wildcards (? and *) it's highly recommended to use single quotations '' to avoid the interpretation of these characters by bash.

Example : ./search -4 ex'?'m'*''.t'?'t

# Credits
