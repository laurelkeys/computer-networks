from statistics import stdev
from math import sqrt
from sys import exit
import matplotlib.pyplot as plt

# expect timestamps in the form:
# "[udp/tcp]:t[timestamp_number]:[opt_number]:[ok/timeout]:[timestamp]us"

RUNS = 20
QTY_TESTS = 3
VERBOSE = False
PATH_TIMESTAMPS_FOLDER = './timestamps/'
PATH_CODES = ['/tcp/','/udp/']

QTY_TESTS += 1 # lazy array indexing

def calculate_confidence_inteval(standard_deviation, n_observations):
    return 1.96*(standard_deviation/sqrt(n_observations))

def get_time(string):
    return string.replace('us','')

def get_timestamp(line, code):
    t = 0
    time = -1
    timeout = False
    if line.startswith(code + ":t"):
        t = line[5]
        if t == 4:
            if line[9:] == "timeout":
                timeout = True
        if timeout:
            time = get_time(line[17:])
        else:
            time = get_time(line[12:])

def file_name(code,opt):
    return PATH_TIMESTAMPS_FOLDER + code + "opt" + str(opt) + ".txt"

def clean_array(array):
    a = sorted(array)
    return a[5:-5]

for path_code in PATH_CODES:
    t1 = [[[0]*RUNS] * QTY_TESTS] * 5
    for opt in range(1,QTY_TESTS):
        index = 0
        next = 1
        timeouts = 0
        with open(file_name(opt),'r') as file:
            string = file.read()
            lines = string.split('\n')
            for line in lines:
                t, time, timeout = get_timestamp(line,code)
                if t != next:
                    break
                next += 1
                if next > 4:
                    next = 1
                if timeout:
                    timeouts += 1
        

        times_communication = [[0]*RUNS] * QTY_TESTS
        times_query = [[0]*RUNS] * QTY_TESTS
                
            
        for i in range(len(t1)):

        query_time = (t2-t1)
        total_time = (t3-t0)
        communication_time = total_time - query_time

        if query_time < 0 or total_time < 0 or communication_time < 0:
            if run > 0:
                query_time = times_query[opt][run-1]
                total_time = times_communication[opt][run-1] + times_query[opt][run-1]
                communication_time = times_communication[opt][run-1]
            else:
                print("Inconsistent time - query: opt:"+str(opt)+": run:"+str(run))
                print("Inconsistent time - total: opt:"+str(opt)+": run:"+str(run))
                print("Inconsistent time - communication: opt:"+str(opt)+": run:"+str(run))
                exit(0)
                # if total_time < 0:
                #     if run > 0:
                #         total_time = times_communication[opt][run-1] + times_query[opt][run-1]
                #     else:
                #         print("Inconsistent time - total: opt:"+str(opt)+": run:"+str(run))
                #         exit(0)
                # if communication_time <= 0:
                #     if run > 0:
                #         communication_time = times_communication[opt][run-1]
                #     else:
                #         print("Inconsistent time - communication: opt:"+str(opt)+": run:"+str(run))
                #         exit(0)

                times_query[opt][run] = query_time
                times_communication[opt][run] = communication_time

                if VERBOSE: 
                    print("Communication time #" + str(run) + ":" + str(communication_time))
        
        print("==================#"+str(opt)+"#==================")
        times_communication[opt] = clean_array(times_communication[opt])
        times_query[opt] = clean_array(times_query[opt])

        average = sum(times_communication[opt])/len(times_communication[opt])
        print("Average communication time of #" + str(opt) + ": " + str(average))
        deviation = stdev(times_communication[opt])
        print("Standard Deviation of #" + str(opt) + ": " + str(deviation))
        delta = calculate_confidence_inteval(deviation, len(times_communication[opt]))
        print("95% conficence interval: " + str(average-(2*delta)) + " to " + str(average+(2*delta)))
        query_avg = sum(times_query[opt])/len(times_query[opt])
        print("Average query time of #" + str(opt) + ": " + str(query_avg) + "\n")

        plt.scatter([opt]*len(times_communication[opt]), times_communication[opt], c='cyan')
        plt.scatter(opt, average, c='r', marker='x')
        plt.scatter(opt, average+(2*delta), c='black', marker='_')
        plt.scatter(opt, average-(2*delta), c='black', marker='_')

    plt.xlabel('Opção', fontsize=12)
    plt.ylabel('Tempo (us)', fontsize=11)
    plt.savefig('graph_communication.png')
    plt.show()
