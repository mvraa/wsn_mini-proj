import os

INPUT_FILES = ["outClient.txt","outIntermediate.txt","outServer.txt"]

# Indicate if the measurements were undertaken with our withour Aggregation
# 
MODE = "NoAggregation" 

LINES_PER_MEASURE = 8
AMOUNT_OF_MEASURES = 10


# From node datasheet (see docs directory)
CURRENT_MA = {
    "CPU" : 0.5,
    "LPM" : 0.026,
    "Deep LPM" : 0, # no data available
    "Radio Rx" : 19.7,
    "Radio Tx" : 17.4,
}

STATES = list(CURRENT_MA.keys())

VOLTAGE = 3.0 #data sheet: the mote operating voltage when attached to USB is 3V
RTIMER_ARCH_SECOND = 32768

def is_file_empty(file_path):
    with open(file_path, 'r') as file:
        return not bool(file.read())

def processFile(input_file):
    line_counter = 0

    # initialize ticks to zero
    ticks = { u : 0  for u in STATES }
    total_ticks = 0
    
    with open(input_file, "r") as f:
        for line in f:
            if line_counter == LINES_PER_MEASURE * AMOUNT_OF_MEASURES:
                break
            if "INFO: Energest" not in line:
                continue
            fields = line.split()
            try:
                line_counter += 1
                state_index = 3
                state = fields[state_index]
                tick_index = state_index + 2
                if state not in STATES:
                    state = fields[state_index] + " " + fields[state_index+1]
                    tick_index += 1
                    if state not in STATES:
                        # add to the total time
                        if state == "Total time":
                            total_ticks += int(fields[tick_index])
                        continue
                # add to the time spent in specific state
                stateTicks = int(fields[tick_index][:-1])
                ticks[state] += stateTicks

                
            except Exception as ex:
                print("Failed to process line '{}': {}".format(line, ex))

    
    total_avg_current_mA = 0
    period_seconds = total_ticks / RTIMER_ARCH_SECOND
    for state in STATES:
        stateTicks = ticks.get(state, 0)
        current_mA = CURRENT_MA[state]
        state_avg_current_mA = stateTicks * current_mA / total_ticks
        total_avg_current_mA += state_avg_current_mA
    total_charge_mC = total_ticks * total_avg_current_mA / RTIMER_ARCH_SECOND
    total_energy_mJ = total_charge_mC * VOLTAGE
    print("Energy measure for {} mote: {:.2f} mC ({:.3f} mAh) charge consumption, {:.2f} mJ energy consumption in {:.2f} seconds".format(
        input_file,total_charge_mC, total_charge_mC / 3600.0, total_energy_mJ, period_seconds))


def main():
    for file in INPUT_FILES:
        full_path = os.path.join(MODE,file)
        print (full_path)
        if not is_file_empty(full_path):
            processFile(full_path)
if __name__ == "__main__":
    main()