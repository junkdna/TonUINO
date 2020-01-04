#!/bin/bash

ctags -R --c++-kinds=+p --fields=+iaS --extra=+q *.ino *.h
cscope -Rb *.h *.ino
