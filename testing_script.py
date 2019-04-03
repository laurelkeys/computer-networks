#!/usr/bin/python
# -*- coding: iso-8859-15 -*-
from subprocess import call, check_output, Popen, PIPE, STDOUT
from os import walk, remove, path
from sys import exit
from shutil import copyfile


PATH_TESTS = './tests/'
PATH_TIMESTAMPS_FILE = 'timestamps.txt'
PATH_TIMESTAMPS_FOLDER = './timestamps/'
HOST_NAME = 'localhost'

RUNS = 1 # How many times each operation should be run



# Compiling the code
error = check_output(["make"])


# Getting all the tests
input_list = []
for (dirpath, dirnames, filenames) in walk(PATH_TESTS):
    input_list = [name for name in filenames if '.in' in name]


for input_path in input_list:
    # Cleanning the timestamps file 
    if path.isfile(PATH_TIMESTAMPS_FILE):
        remove(PATH_TIMESTAMPS_FILE)

    # Running the code
    for _ in range(RUNS):
        query = "./client " + HOST_NAME + " < " + PATH_TESTS + input_path
        print(query)
        # "./client", HOST_NAME, " < ", PATH_TESTS, input_path
        process = Popen([query],#"./client", HOST_NAME, "<", PATH_TESTS, input_path],
                    stdout=PIPE,
                    stderr=STDOUT,
                    shell=True)#.communicate() 
        out = process.stdout.read()
        print(out)
        print("================================\n")

    # Copy timestamps file
    copyfile(PATH_TIMESTAMPS_FILE, PATH_TIMESTAMPS_FOLDER + input_path[:-3] + ".txt")
    print("Copied to :"+ input_path[:-3] + ".txt")
    print("Done with " + input_path)

print("Done")
