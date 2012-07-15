#!/usr/bin/env python2.7

import sys
from pyassimp import pyassimp

print("BEGIN")

for obj in sys.argv[1:]:
    try:
        scene = pyassimp.load(obj)
    except pyassimp.AssimpError:
        print("Ei voitu ladata {}".format(obj))
    else:
        xcoords = []
        ycoords = []
        zcoords = []
        for mesh in scene.meshes:
            for vertex in mesh.vertices:
                xcoords.append(vertex[0])
                ycoords.append(vertex[1])
                zcoords.append(vertex[2])

        mincoord = (min(xcoords), min(ycoords), min(zcoords))
        maxcoord = (max(xcoords), max(ycoords), max(zcoords))

        obj = obj.split('/')

        filename = obj[-1]
        filename = filename.replace('_hd.obj', '')
        path = obj[:-1]

        if (path[0] == 'assets'):
            del path[0]

        if (path[0] == 'obj'):
            del path[0]

        bbsize = (maxcoord[0] - mincoord[0], maxcoord[2] - mincoord[2], maxcoord[1] - mincoord[1])
        center = (mincoord[0] + bbsize[0] / 2, - (mincoord[2] + bbsize[1] / 2), mincoord[1] + bbsize[2] / 2)

        print("{}~\\{}\\~{}~{}".format(filename, '\\'.join(path), ','.join(str(i) for i in center), ','.join(str(i) for i in bbsize)))

print("END")
