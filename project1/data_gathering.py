from statistics import stdev
from math import sqrt
from sys import exit
import matplotlib.pyplot as plt


RUNS = 50
VERBOSE = False
PATH_TIMESTAMPS_FOLDER = './timestamps/'


def calculate_confidence_inteval(standard_deviation, n_observations):
    return 1.96*(standard_deviation/sqrt(n_observations))

def get_timestamp(haystack, needle, from_index=0):
    # search for string from index
    begin = haystack.find(needle, from_index)
    if begin == -1:
        return (-1, -1)
    begin +=  + len(needle)

    # search for 'us\n'
    end = haystack.find("us", begin)
    if end == -1:
        return (-1, -1)

    # get the number in between both indexes
    return (int(haystack[begin:end]), end+2)

def file_name(opt):
    return PATH_TIMESTAMPS_FOLDER + "opt" + str(opt) + ".txt"

def clean_array(array):
    a = sorted(array)
    return a[5:-5]


times_communication = [[0]*RUNS]*7
times_query = [[0]*RUNS]*7
for opt in range(1,7):
    from_index0 = 0
    from_index1 = 0
    from_index2 = 0
    from_index3 = 0
    with open(file_name(opt),'r') as file:
        string = file.read()
        for run in range(RUNS):
            t0, from_index0 = get_timestamp(string, "client:opt"+str(opt)+":before input send:", from_index0)
            if from_index0 == -1:
                continue
            t1, from_index1 = get_timestamp(string, "server:opt"+str(opt)+":before query:", from_index1)
            if from_index1 == -1:
                continue
            t2, from_index2 = get_timestamp(string, "server:opt"+str(opt)+":after query:", from_index2)
            if from_index2 == -1:
                continue
            t3, from_index3 = get_timestamp(string, "client:opt"+str(opt)+":after result receive\n\n:", from_index3)
            if from_index3 == -1:
                continue

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
