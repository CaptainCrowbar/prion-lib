# Miscellanous System Facilities #

By Ross Smith

* `#include "rs-core/system.hpp"`

## Contents ##

[TOC]

## File preview ##

* `void` **`preview_file`**`(const Unicorn::Path& file)`

Attempts to display a file on the screen, using the operating system's default
behaviour for the file type. This will throw `system_error` if the file does
not exist, if the command processor fails to start, or if the file opening
command returns a non-zero exit status. Whether the child process is
synchronous or asynchronous is operating system dependent.
