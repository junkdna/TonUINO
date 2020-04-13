#!/bin/bash

ctags -R --c++-kinds=+p --fields=+iaS --extra=+q *.cpp *.h
cscope -Rb *.h *.cpp
