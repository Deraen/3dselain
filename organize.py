#!/usr/bin/env python3.2

import os
import shutil

ORGFILES = '/media/levyt/bak/hesa-3dmalli/obj/'
TARGET = 'assets/obj/'

with open("assets/obj/000-helsinki-2009_nowa.lpf") as f:

    lue = False
    while True:
        line = f.readline()
        if not line:
            break

        line = line.rstrip()

        if line == 'BEGIN':
            lue = True
            continue

        if line == 'END':
            break

        if not lue or len(line) == 0 or line[0] == ';' or line[0] == '#':
            continue

        line = line.split('~')

        filename = line[0] + '_hd'

        dir = line[1]
        dir = dir.replace('\\', '/')
        dir = dir[1:]

        # print("Filename: {}. Dir: {}".format(filename, TARGET + dir))

        try:
            with open(ORGFILES + filename + '.obj') as f2:
                pass
        except IOError:
            print(filename + '.obj ei löytynyt')
            continue

        try:
            with open(ORGFILES + filename + '.mtl') as f2:
                pass
        except IOError:
            print(filename + '.mtl ei löytynyt')
            continue

        if not os.path.exists(TARGET + dir):
            os.makedirs(TARGET + dir)

        shutil.copy(ORGFILES + filename + '.obj', TARGET + dir + filename + '.obj')
        shutil.copy(ORGFILES + filename + '.mtl', TARGET + dir + filename + '.mtl')
