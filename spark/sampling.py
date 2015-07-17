#!/usr/bin/python
# -*- coding:utf-8 -*-
# Created Time: Fri Jul 17 01:30:55 2015
# Purpose: sampling method for triangles counting
# Mail: hewr2010@gmail.com
__author__ = "Wayne Ho"

import argparse
parser = argparse.ArgumentParser()
parser.add_argument("--input", "-i",
                    help="input graph file",
                    required=False,
                    default="./input.txt",
                    type=str)
parser.add_argument("--output", "-o",
                    help="result file",
                    required=False,
                    default="./result.txt",
                    type=str)
parser.add_argument("-p",
                    help="sample probability",
                    required=False,
                    default=0.1,
                    type=float)
parser.add_argument("--split", "-s",
                    help="splits",
                    required=False,
                    default=1,
                    type=int)
parser.add_argument("-g",
                    help="compare with the standard result",
                    action="store_true")
args = parser.parse_args()

import random
from pyspark import SparkContext
from operator import add


def parseEdge(s):
    x, y = s.split(" ")
    return int(x), int(y)


def sampleEdge(e):
    if random.random() < args.p:
        yield e


def calTriangles(edges_dict, x, _l):
    l = list(_l)
    cnt = 0
    for i, y in enumerate(l):
        for z in l[:i]:
            _y, _z = min(y, z), max(y, z)
            if (_y, _z) in edges_dict:
                cnt += 1. / (args.p ** 3)
    return cnt

if __name__ == "__main__":
    sc = SparkContext(appName="PythonTriangleSampling")
    lines = sc.textFile(args.input, args.split)\
        .zipWithIndex()\
        .filter(lambda x: x[1] > 0)\
        .map(lambda x: x[0])
    edges = lines.map(lambda s: parseEdge(s))
    edges_pool = edges.flatMap(lambda e: sampleEdge(e))
    edges_dict = {e:args.p for e in edges_pool.collect()}
    edges_pool = edges_pool.groupByKey().map(lambda o: calTriangles(edges_dict, o[0], o[1]))
    ans = int(edges_pool.reduce(add))
    if args.g:
        std = int(open("./standard.txt").read())
        err = float(ans - std) / std
    print ans
    if args.g:
        print err
    if args.output != "":
        with open(args.output, "w") as fout:
            print >> fout, ans
            if args.g:
                print >> fout, err
