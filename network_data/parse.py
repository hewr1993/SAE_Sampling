#!/usr/bin/python
# -*- coding:utf-8 -*-
# Created Time: Thu Jul 23 00:53:52 2015
# Purpose: TODO
# Mail: hewr2010@gmail.com
__author__ = "Wayne Ho"

import sys

if __name__ == "__main__":
    V = {}
    E = []
    for line in sys.stdin:
        if line[0] == "#":
            continue
        splitter = " " if line.find(" ") != -1 else "\t"
        x, y = map(lambda x: int(x), line[:-1].split(splitter))
        if x not in V:
            V[x] = len(V)
        if y not in V:
            V[y] = len(V)
        E.append((V[x], V[y]))
    print len(V), len(E)
    for x, y in E:
        print x, y
