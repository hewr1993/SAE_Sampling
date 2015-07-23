#!/usr/bin/python
# -*- coding:utf-8 -*-
# Created Time: 2015年07月24日 星期五 03时07分02秒
# Purpose: generate latex report content
# Mail: hewr2010@gmail.com
__author__ = "Wayne Ho"

import json
import os

template_header = """\\documentclass[11pt]{report}
\\begin{document}

\\begin{table}[t]
\\caption{}
{
%\\small
\\label {tb:kl}
\\centering
\\renewcommand\\arraystretch{1.2}
\\begin{tabular}{@{}c|c|c|c|c|c|c|c}
\\hline
{Data Set} & {Standard Result} & {SAE Result} & {Error Rate ($\\%$)} & {BF Time (sec.)} & {Cores} & {Raw Time (sec.)} & {Total Time (sec.)}\\\\
\\hline
"""
template_footer = """\\hline
\\end{tabular}
%\\vspace{-0.1in}
}
\\end{table}

\\end{document}
"""


def getDataName(content):
    content = os.path.basename(content)
    return ".".join(content.split(".")[:-1])

if __name__ == "__main__":
    table_content = ""
    data = json.load(open("./performance.json"))
    for info in data:
        name = getDataName(info["data"])
        for i, (t_raw, t_tot) in enumerate(zip(info["time"]["smpl-raw"], info["time"]["smpl-total"])):
            if i == 1:
                table_content += "%s & %d & %d & %.2f & %.4f & %d & %.4f & %.4f \\\\\n" % (name, info["std_result"], info["smpl_result"], info["error"], info["time"]["bf"], 2 ** i, t_raw, t_tot)
            else:
                table_content += "& & & & & %d & %.4f & %.4f \\\\\n" % (2 ** i, t_raw, t_tot)
        table_content += "\\hline\n"
    print template_header + table_content + template_footer
