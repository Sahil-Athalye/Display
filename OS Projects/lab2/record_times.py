import csv
import re
import os
import subprocess
import time
import fileinput
import sys
from tempfile import mkstemp
from shutil import move, copymode
from os import fdopen, remove


trials = 10
binary_name = "dense_mm"
results_file = sys.argv[-1] or "results.csv"

rows = []

def run_with_args(args):
    res = subprocess.check_output(f"./{binary_name} {args}", shell=True)
    res = res.decode("utf-8")
    print(res)
    times = re.findall("\d+\.\d+", res)
    return times

with open(results_file, "w") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(["size", "trial", "mmap_time", "matrix_mul_time"])

    for size in [64, 128, 256, 512]:
        print(f"Testing with size {size}")
        for trial in range(1, trials+1):
            mmap_time, matrix_mul_time = run_with_args(size)
            row = [size, trial, mmap_time, matrix_mul_time]
            writer.writerow(row)
            csvfile.flush()
