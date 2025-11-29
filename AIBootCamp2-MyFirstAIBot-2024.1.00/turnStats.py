from datetime import datetime
from subprocess import call
import ctypes
import numpy as np
import os
import sys

def RepairReplayLog(replayLog):
    isValid = False
    data = []
    dataSize = len(data)
    with open(replayLog) as f:
        for line in f:
            data.append(line)
        if "resultData" in line:
            isValid = True
    if not isValid:
        data.append("], \"resultData\":{\"hasWin\":false,\"resultPayload\":\"AIBot Crashed Detected!\"}}")
    with open(replayLog, "w+") as f:
        f.truncate(0)
        for line in data:
            f.write(line)

def runLevel(level):
    mapname = level
    AIBOOTCAMP2_ENGINE_DIR = r"..\AIBootCamp2"
    AIBOT_PATH = r"build-solution-x64\Release\myfirstaibot.dll"
    TEAM = "Ekip_1"
    ConnectToProcessDelay = "-1"
    InitTime = "-1"
    TurnTime = "-1"
    now = datetime.now()
    replayfile=now.strftime("%Y%m%d_%H%M%S")
    result = call([AIBOOTCAMP2_ENGINE_DIR+"/AIBootCamp2.exe", "-dllpath", AIBOT_PATH, "-mode", "match", "-scene", mapname, "-team", TEAM, "-replayfile", replayfile, "-connecttoprocessdelay", ConnectToProcessDelay, "-initdelay", InitTime, "-turndelay", TurnTime, "-quit", "-batchmode"])    
    result = ctypes.c_int32(result).value
    if result == 0:
        print(f"Level {level} completed : Victory!")
        return True
    elif result == -1 or result == 3:
        print(f"Level {level} completed : Failure")
        RepairReplayLog(".\\Replays\\" + replayfile + "\\" + mapname + "_" + replayfile + ".replay")
    elif result == -2:
        print("Invalid Map Name [" + mapname + "]")
    return False

# MAIN
nums = ["00","01","02","03","04","05","10","11","12","13","20","21","22","23","30","31","32","33","34","40","41","42","50","60","61","62","63","64","65","66","70","71","72","73","80","81","82","83"]

measures = "./measures/measures.txt"

min_time = float('inf')
max_time = -float('inf')
min_level = "def"
max_level = "def"
successes = 0

for to_play in nums:

    # Prepare for execution
    level = f"L_0{to_play}"

    if os.path.exists(measures):
        os.remove(measures)

    # Run the level
    successes += runLevel(level)

    # Update stats
    data = np.loadtxt(measures)
    min_val = int(data.min())
    max_val = int(data.max())
    if min_val < min_time:
        min_time = min_val
        min_level = level
    if max_val > max_time:
        max_time = max_val
        max_level = level

print()
print(f"Successes = {successes} ({successes / len(nums) * 100}%)")
print(f"Min turn time: {min_time}, measured on level: {min_level}")
print(f"Max turn time: {max_time}, measured on level: {max_level}")
