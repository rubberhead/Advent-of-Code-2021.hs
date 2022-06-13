# CRABS CRABS
# If you think about it... wouldn't this be same problem as residual minimization?
# This is just regression w/ less steps!
from scipy import optimize
from pathlib import Path
from functools import partial

def parser(filepath: Path) -> list:
    positions = []
    with open(filepath, 'r') as f:
        for line in f:
            positions = [int(s) for s in line.split(',')]
    return positions

# 1.
def totalFuelTo(positions: list, y: int) -> int:
    return (sum([abs(p - y) for p in positions]))

def getSol1(positions: list) -> int:
    # This is just cheating honestly... Whatever floats the boat perhaps?
    optimized = optimize.minimize_scalar(partial(totalFuelTo, positions))
    return (round(optimized.fun))

# 2.
def totalFuelTo_2(positions: list, y: int) -> int:
    fuels = [sum(range(1, round(abs(p - y)) + 1, 1)) for p in positions]
    return (sum(fuels))

def getSol2(positions: list) -> int:
    optimized = optimize.minimize_scalar(partial(totalFuelTo_2, positions))
    return (round(optimized.fun))

def test():
    positions = [16,1,2,0,4,2,7,1,2,14]
    test_file_path = Path.cwd().joinpath("resource/q7/input_test")
    assert(parser(test_file_path) == positions)
    assert(getSol1(positions) == 37)
    assert(getSol2(positions) == 168)

if __name__ == "__main__":
    test()
    positions = parser(Path.cwd().joinpath("resource/q7/input"))
    print (getSol1(positions))
    print (getSol2(positions))

