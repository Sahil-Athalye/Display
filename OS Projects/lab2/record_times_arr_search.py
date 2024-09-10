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
binary_name = "arr_search"
results_file = sys.argv[-1] or "results.csv"
code_file = "arr_search.c"

rows = []


def compile(size=500):
    fh, abs_path = mkstemp()
    with fdopen(fh, "w") as new_file:
        with open(code_file) as old_file:
            for line in old_file:
                updated_line = line
                if "#define ARR_SIZE" in line:
                    tokens = line.split(" ")
                    tokens[-1] = f"{size}\n"
                    updated_line = " ".join(tokens)
                    print("Editing code file to have line:")
                    print(updated_line)
                new_file.write(updated_line)
    # Copy the file permissions from the old file to the new file
    copymode(code_file, abs_path)
    # Remove original file
    remove(code_file)
    # Move new file
    move(abs_path, code_file)

    # cmd = f"gcc -Wall {olevel} -lm -fopenmp -o {binary_name} {code_file}"
    # print(cmd)
    cmd = "make arr_search"
    subprocess.run(cmd, shell=True)


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
        print(size)
        print(f"Testing with size {size}")
        compile(size=size**2)
        for trial in range(1, trials+1):
            mmap_time, matrix_mul_time = run_with_args(1)
            row = [size, trial, mmap_time, matrix_mul_time]
            writer.writerow(row)
            csvfile.flush()
