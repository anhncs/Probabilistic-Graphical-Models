# This file is part of libDAI - http://www.libdai.org/
#
# Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
#
# Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.


include ../Makefile.conf

.PHONY:	all

all:			_dai.so dai.oct

_dai.so:		../lib/libdai.a dai.i
			$(SWIG) -python -classic -c++ dai.i
			g++ -Wall -c -O3 -g -fPIC dai_wrap.cxx -I$(INCLUDE_PYTHON) -I$(INCLUDE_BOOST) -I../include
			g++ -shared dai_wrap.o -o _dai.so ../lib/libdai.a

dai.oct:		../lib/libdai.a dai.i
			$(SWIG) -octave -c++ -o dai_wrap.cpp dai.i
			mkoctfile -I$(INCLUDE_BOOST) -I../include -o dai.oct dai_wrap.cpp ../lib/libdai.a

.PHONY: clean

clean:		
			rm -f dai_wrap.cxx dai_wrap.cpp dai_wrap.o _dai.so dai.oct dai.py dai.pyc sprinkler.fg
