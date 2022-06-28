# Part 2 only, Part 1 done in pen & paper method
from pathlib import Path
import numpy as np

def findDistinctVelocities(x_range: range, y_range: range) -> set:
    v_xmin = round(np.polynomial.Polynomial([-2*x_range.start, 1, 1]).roots()[1])
    v_xmax = x_range.stop - 1
    v_ymin = -abs(y_range.start) # y_range.start < y_range.stop - 1
    v_ymax = abs(y_range.start)
    result = []

    for v_y in range(v_ymin, v_ymax + 1):
        pos_y = 0
        steps = 0
        v_curr = v_y
        if (v_y > 0): # e.g., aiming upwards, curveball
            steps += 2 * v_y + 1 # v_y steps to travel from y = 0 to max_height and back, plus one step where v_y = 0
            v_curr = -abs(v_y) - 1 # change direction, next step - 1 velo
        # otherwise, aiming downwards w/ no prev steps
        while (pos_y >= y_range.start): # >= y_range_min (inclusive)
            pos_y += v_curr
            v_curr -= 1
            steps += 1
            if (pos_y in y_range):
                # Got step count, find v_x where v_x * steps is in x_range
                for v_x in range(v_xmin, v_xmax + 1):
                    v_xs = [v if v >= 0 else 0 for v in range(v_x, (v_x - steps), -1)]
                    if (sum(v_xs) in x_range):
                        result.append((v_x, v_y))
    
    return set(result)

def answerParser(filepath: Path) -> list:
    result = []
    with open(filepath, 'r') as f:
        for line in f:
            result += [tuple(map(int, css.split(','))) for css in line.split()]
    return result

def test():
    test_result = findDistinctVelocities(range(20, 31), range(-10, -4))
    test_answer = set(answerParser(Path.cwd().joinpath("resource/q17/input_test")))
    print (test_answer.difference(test_result))
    assert(test_result == set(test_answer))

if __name__ == "__main__":
    test()
    result = findDistinctVelocities(range(119, 177), range(-141, -83))
    print ("Part 2:", len(result))