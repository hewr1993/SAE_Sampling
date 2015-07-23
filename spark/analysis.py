#!/usr/bin/python
# -*- coding:utf-8 -*-
# Created Time: Thu Jul 23 03:34:46 2015
# Purpose: get performance of spark
# Mail: hewr2010@gmail.com
__author__ = "Wayne Ho"

import os
import time
import json


def perform(cmd):
    st_time = time.time()
    print "[system] " + cmd
    os.system(cmd)
    return time.time() - st_time

if __name__ == "__main__":
    datadir = "../network_data/"
    tests = []
    for line in os.popen("ls " + datadir):
        times = {}
        times["bf-C++"] = perform("./bruteforce < %s > ./standard.txt" % (datadir + line[:-1]))
        times["bf"] = perform("./bruteforce.py -i %s > ./standard.txt" % (datadir + line[:-1]))
        times["smpl-total"], times["smpl-raw"] = [], []
        for i in xrange(4):
            thread_num = 2 ** i
            times["smpl-total"].append(
                perform("spark-submit ./sampling.py -i %s -s %d -g 2> nul > tmp"\
                        % (datadir + line[:-1], thread_num))
            )
            times["smpl-raw"].append(float([l[:-1] for l in open("./result.txt")][1]))
        std_result = int(open("./standard.txt").read())
        smpl_result = int(open("./result.txt").readline())
        err = float(smpl_result - std_result) / std_result
        tests.append({
            "data": datadir + line[:-1],
            "std_result": std_result,
            "smpl_result": smpl_result,
            "error": err,
            "time": times
        })
        json.dump(tests, open("performance.json", "w"), indent=4)
