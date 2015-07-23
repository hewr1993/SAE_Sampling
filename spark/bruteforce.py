#!/usr/bin/python
# -*- coding:utf-8 -*-
# Created Time: 2015年07月24日 星期五 02时29分01秒
# Purpose: bruteforce
# Mail: hewr2010@gmail.com
__author__ = "Wayne Ho"

import argparse
parser = argparse.ArgumentParser()
parser.add_argument("--input", "-i",
                    help="input graph file",
                    required=False,
                    default="./input.txt",
                    type=str)
args = parser.parse_args()

if __name__ == "__main__":
    fin = open(args.input)
    fin.next()
    edges = []
    for line in fin:
        x, y = map(lambda x: int(x), line[:-1].split(" "))
        if x > y:
            x, y = y, x
        if x == y:
            continue
        edges.append((x, y))
    edges.sort()
    doc = {(x, y):True for x, y in edges}
    cnt = 0
    i = 0
    while i < len(edges):
        j = i
        while j < len(edges) and edges[j][0] == edges[i][0]:
            j += 1
        for l in xrange(i, j):
            for r in xrange(i, l):
                x, y, z = edges[l][0], edges[l][1], edges[r][1]
                if y > z:
                    y, z = z, y
                if (y, z) in doc:
                    cnt += 1
        i = j
    print cnt
