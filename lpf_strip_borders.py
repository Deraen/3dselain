#!/usr/bin/env python3.2

INFILE = "assets/obj/000-helsinki-2009_nowa.lpf"
OUTFILE = "assets/obj/000-helsinki-2009_stripped.lpf"

with open(INFILE) as f:
    with open(OUTFILE, 'w') as out:

        while True:
            line = f.readline()
            if not line:
                break

            line = line.rstrip()

            if line == 'BEGIN':
                out.write(line + '\n')
                continue

            if line == 'END':
                out.write(line + '\n')
                continue

            if len(line) == 0 or line[0] == ';' or line[0] == '#':
                out.write(line + '\n')
                continue

            line = line.split('~')
            out.write("~".join(line[0:4]) + "\n")
