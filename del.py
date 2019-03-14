import os
from shutil import copy
from sys import argv

def delete_line_indexes(src_path, dst_path):
    with open(src_path, "r") as file, open(dst_path, "w") as new_file:
        for line in file:
            new_file.write(line.lstrip(" 1234567890"))

src = './unp.h'
dst = './unp.h~'
if len(argv) > 1:
    dst = argv[1]
    if len(argv) > 2:
        src = argv[2]

delete_line_indexes(src, dst)
print(":)")
