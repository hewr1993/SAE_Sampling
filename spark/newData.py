#!/usr/bin/python
# -*- coding:utf-8 -*-
# Created Time: Fri Jul 17 01:05:00 2015
# Purpose: prepare a new graph
# Mail: hewr2010@gmail.com
__author__ = "Wayne Ho"

import os
import sys


def perform(cmd):
    print cmd
    os.system(cmd)

if __name__ == "__main__":
    points, edges = 1000, 100000
    perform("./genGraph.py %d %d > ./input.txt" % (points, edges))
    perform("./bruteforce < ./input.txt > ./standard.txt")
    perform("cat ./standard.txt")
    #perform("spark-submit ./sampling.py")
