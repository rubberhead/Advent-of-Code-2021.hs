from functools import reduce
from operator import add

# Bookwork
def memoize(f):
    memo = {}
    def check(t, d):
        if (t, d) not in memo:
            memo[(t, d)] = f(t, d)
        return memo[(t, d)]
    return check

# Simulates resultant fish count from one lanternfish
# Simple recursive solution. Nothing fancy.
def simulate(init_timer: int, days: int) -> int:
    count = 1
    days -= init_timer
    while (days // 6 > 0):
        count += simulate(8, days)
        days -= 6
    return count
    
simulate = memoize(simulate)

def getSol1(init_state: list, days: int) -> int:
    return (sum(map(lambda t: simulate(t, days), init_state)))

if __name__ == "__main__":
    print (getSol1([3,4,3,1,2], 80))

