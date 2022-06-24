import math
import heapq
from dataclasses import dataclass, field
from pathlib import Path

# Isn't this basically a struct?
@dataclass(order=True)
class CaveNode:
    x: int = field(compare=False)
    y: int = field(compare=False)
    cost: float = field(default=math.inf, compare=True)

def lowestRiskDijkstra(risks: list) -> int:
    # initialize single source @ 0,0
    dists = [[math.inf for j in range(len(risks[0]))] for i in range(len(risks))]
    dists[0][0] = 0

    # min-heap as frontier. Python implements min-heap which is suitable for our case
    frontier = [CaveNode(0, 0, 0)]

    # Relaxation
    while (len(frontier) != 0):
        p = heapq.heappop(frontier)
        if (p.cost > dists[p.x][p.y]): # dist of p updated after p pushed into frontier
            continue
        if (p.x > 0 and 
            dists[p.x][p.y] + risks[p.x - 1][p.y] < dists[p.x - 1][p.y]):
            dists[p.x - 1][p.y] = dists[p.x][p.y] + risks[p.x - 1][p.y]
            heapq.heappush(frontier, CaveNode(p.x - 1, p.y, dists[p.x - 1][p.y]))
        if (p.x + 1 < len(dists) and 
            dists[p.x][p.y] + risks[p.x + 1][p.y] < dists[p.x + 1][p.y]):
            dists[p.x + 1][p.y] = dists[p.x][p.y] + risks[p.x + 1][p.y]
            heapq.heappush(frontier, CaveNode(p.x + 1, p.y, dists[p.x + 1][p.y]))
        if (p.y > 0 and 
            dists[p.x][p.y] + risks[p.x][p.y - 1] < dists[p.x][p.y - 1]):
            dists[p.x][p.y - 1] = dists[p.x][p.y] + risks[p.x][p.y - 1]
            heapq.heappush(frontier, CaveNode(p.x, p.y - 1, dists[p.x][p.y - 1]))
        if (p.y + 1 < len(dists[0]) and 
            dists[p.x][p.y] + risks[p.x][p.y + 1] < dists[p.x][p.y + 1]):
            dists[p.x][p.y + 1] = dists[p.x][p.y] + risks[p.x][p.y + 1]
            heapq.heappush(frontier, CaveNode(p.x, p.y + 1, dists[p.x][p.y + 1]))

    return (dists[len(dists) - 1][len(dists[0]) - 1])

def parser(filepath: Path) -> list:
    risks = []
    with open(filepath, 'r') as f:
        for line in f:
            risks.append([int(c) for c in line.rstrip()])
    return risks

def rollingExtensionParser(filepath: Path) -> list:
    risks_in = []
    with open(filepath, 'r') as f:
        for line in f:
            risks_in.append([int(c) for c in line.rstrip()])
    
    # Extension by modular arithmetics
    risks_out = [[0 for n in range(5*len(risks_in[0]))] for m in range(5*len(risks_in))]
    for idxm in range(len(risks_out)): # m-n coordinate for risks_out
        tile_m = idxm // len(risks_in)
        idxi = idxm % len(risks_in) # i-j coordinate for risks_in
        for idxn in range(len(risks_out[idxm])):
            tile_n = idxn // len(risks_in[0])
            idxj = idxn % len(risks_in[0])
            temp = (risks_in[idxi][idxj] + tile_m + tile_n) % 9
            if (temp == 0):
                risks_out[idxm][idxn] = 9
            else:
                risks_out[idxm][idxn] = temp
    
    return risks_out

def test():
    risks_test = [
        [1,1,6,3,7,5,1,7,4,2], 
        [1,3,8,1,3,7,3,6,7,2], 
        [2,1,3,6,5,1,1,3,2,8], 
        [3,6,9,4,9,3,1,5,6,9], 
        [7,4,6,3,4,1,7,1,1,1], 
        [1,3,1,9,1,2,8,1,3,7], 
        [1,3,5,9,9,1,2,4,2,1], 
        [3,1,2,5,4,2,1,6,3,9], 
        [1,2,9,3,1,3,8,5,2,1], 
        [2,3,1,1,9,4,4,5,8,1]
    ]

    # parser, part 1
    risks_test_parsed = parser(Path.cwd().joinpath("resource/q15/input_test"))
    assert(risks_test == risks_test_parsed)
    assert(lowestRiskDijkstra(risks_test) == 40)

    # part 2
    risks_test_extensionParsed = rollingExtensionParser(Path.cwd().joinpath("resource/q15/input_test"))
    risks_test_parsed = parser(Path.cwd().joinpath("resource/q15/input_test_2"))
    assert(risks_test_extensionParsed == risks_test_parsed)


if __name__ == "__main__":
    test()
    # Part 1
    risks = parser(Path.cwd().joinpath("resource/q15/input"))
    print("Part 1:", lowestRiskDijkstra(risks))

    # Part 2
    risks = rollingExtensionParser(Path.cwd().joinpath("resource/q15/input"))
    print("Part 2:", lowestRiskDijkstra(risks))

