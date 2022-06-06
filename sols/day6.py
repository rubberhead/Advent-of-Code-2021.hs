from itertools import chain
import time
import multiprocessing as mp

# Both solutions are not going to solve part 2 due to exponential space use?
# As per Simon Toth we could count by timer state instead (i.e., how many fishes in each timer state per day)
# That yields around O(n*8) time w/ const space, which is both fast and lightweight
def simulate_timer(init_states: list, days: int) -> int:
    start = time.time()
    timer_states = [0 for t in range(0, 9)]
    
    # Initialize
    for s in init_states:
        timer_states[s] += 1
    
    # Simulate
    d = 1
    while (d <= days):
        ready = timer_states[0]
        timer_states.pop(0)
        timer_states.append(ready)
        timer_states[6] += ready
        d += 1
    
    end = time.time()
    print ("Timer:", end - start)
    
    return (sum(timer_states))

# iterative solution, O(n^2)
def simulate_it(init_states: list, days: int) -> int:
    start = time.time()
    d = 1
    while (d <= days):
        l = 0
        while (l < len(init_states)):
            if (init_states[l] == 0):
                init_states[l] = 7 
                init_states.append(9) 
            init_states[l] -= 1
            l += 1
        d += 1
    end = time.time()
    print ("Iter:", end - start)
    return (len(init_states))

# Alternative solution, O(n)
# Parallel processing for simulate_chain
def simulate_chain_per_fish(zero_day, day_limit) -> list:
    resultant_fishes = [(0, zero_day)]
    for (counter, zd) in resultant_fishes:
        while (zd < day_limit):
            resultant_fishes.append((0, zd + 9))
            zd += 7
    return (resultant_fishes)

def simulate_chain(first_zero_days: list, day_limit: int) -> int:
    start = time.time()
    resultant_fishes = [(0, first_zero_day) for first_zero_day in first_zero_days]
    with mp.Pool(mp.cpu_count()) as p:
        async_result = list(p.apply_async(simulate_chain_per_fish, args = (zero_day, day_limit)) for (counter, zero_day) in resultant_fishes)
        resultant_fishes = list(chain(*[r.get() for r in async_result]))
    end = time.time()
    print ("Chain:", end - start)
    return (len(resultant_fishes))

'''
    for (counter, zero_day) in resultant_fishes: # for each initial fish
        while (zero_day < day_limit):
            resultant_fishes.append((0, zero_day + 9))
            zero_day += 7
'''

def getSol1(days: int) -> int:
    init_states = []
    with open("/mnt/c/Users/ASUS/Documents/GitHub/Advent-of-Python-2021/resource/q6/input", 'r') as inputf:
        for line in inputf:
            init_states = [int(s) for s in line.split(',')]
    return (simulate_chain(init_states, days))

def getSol2(days: int) -> int:
    init_states = []
    with open("/mnt/c/Users/ASUS/Documents/GitHub/Advent-of-Python-2021/resource/q6/input", 'r') as inputf:
        for line in inputf:
            init_states = [int(s) for s in line.split(',')]
    return (simulate_timer(init_states, days))

def test():
    init_states = [3, 4, 3, 1, 2]
    #for days in range(, 80):
    assert(simulate_timer(init_states, 80) == simulate_it(init_states, 80))
    print(simulate_timer(init_states, 256))

if __name__ == "__main__":
    test()
    print("--------------")
    print (getSol1(80))
    print (getSol2(256))

