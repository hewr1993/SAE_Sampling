#!/usr/bin/python
# -*- coding:utf-8 -*-
# Created Time: Fri Jul 17 00:58:20 2015
# Purpose: generate a bidirected graph
# Mail: hewr2010@gmail.com
__author__ = "Wayne Ho"

import sys
import random

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("./%s [#vertices] [#edges]" % sys.argv[0])
        exit()
    n, m = int(sys.argv[1]), int(sys.argv[2])
    print n, m
    pool = {}
    for i in xrange(m):
        while True:
            x, y = int(n * random.random()), int(n * random.random())
            if x > y:
                x, y = y, x
            if x == y:
                continue
            if (x, y) not in pool:
                break
        pool[(x, y)] = True
        print x, y
