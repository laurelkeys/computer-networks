from statistics import stdev
from math import sqrt


RUNS = 3
VERBOSE = True
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


times = [[0]*RUNS]*7
for opt in range(1,2):
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

            communication_time = (t3-t0) - (t2-t1)
            times[opt][run] = communication_time

            if VERBOSE: 
                print("Communication time #" + str(run) + ":" + str(communication_time))
    
    average = sum(times[opt])/len(times[opt])
    print("Average time of #" + str(opt) + ": " + str(average))
    deviation = stdev(times[opt])
    print("Standard Deviation of #" + str(opt) + ": " + str(deviation))
    delta = calculate_confidence_inteval(deviation, RUNS)
    print("95% conficence interval: " + str(average-delta) + " to " + str(average+delta) + "\n")
