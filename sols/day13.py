# Try cpp maybe

# Basically, 
# For each fold until the last fold cmd, 
# 1. Filter the set of all dots into two subsets (plz in-place), 
#    one containing dots not folded in and another containing dots folded in
#    (maybe keep in order then find an iterator-like?)
#    (Python keeps dict ordered since 3.7... )
# 2. Apply (fold_threshold - dist_to_threshold) onto each element in folded subset
# 3. Get the disjunction of two subsets for the next fold cmd (either in-place 
#    or with input set deleted from memory

def applyFoldCmd(points: set, cmd: str) -> set: 
    # cmd is of <direction> = <threshold> type


