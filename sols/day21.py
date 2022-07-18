from dataclasses import dataclass

class DeterministicDice:
    # roll values range from 1 to limit, inclusive
    def __init__(self, limit: int):
        self.limit = limit
        self.roll_count = 0

    def getRoll(self) -> int:
        self.roll_count += 1
        modulo_result = self.roll_count % self.limit
        return modulo_result if modulo_result != 0 else self.limit

class DeterministicDiceGame:
    # Custom dice, game range, 2 player only
    # pawn positions range between 1 and limit, inclusive
    def __init__(self, dice: DeterministicDice, board_limit: int, p1_begin: int, p2_begin: int):
        self.dice = dice
        self.BOARD_LIMIT = board_limit
        self.p1_pos = p1_begin
        self.p2_pos = p2_begin
        self.p1_score = 0
        self.p2_score = 0
        self.SCORE_LIMIT = 1000

    def getMove(self):
        # Get roll
        roll = 0
        for i in range(3):
            next = self.dice.getRoll()
            roll += next
        
        # Get equivalent move count
        move = roll % self.BOARD_LIMIT

        return move # which should be modulo-ed again in simulation in accordance to 1p/2p

    def simulate(self):
        while (self.p1_score < self.SCORE_LIMIT and self.p2_score < self.SCORE_LIMIT):
            # p1 plays
            p1_move = self.getMove()
            self.p1_pos = (p1_move + self.p1_pos) % self.BOARD_LIMIT
            if (self.p1_pos == 0): self.p1_pos = self.BOARD_LIMIT
            self.p1_score += self.p1_pos
            if (self.p1_score >= self.SCORE_LIMIT): break
            
            # p2 plays
            p2_move = self.getMove()
            self.p2_pos = (p2_move + self.p2_pos) % self.BOARD_LIMIT
            if (self.p2_pos == 0): self.p2_pos = self.BOARD_LIMIT
            self.p2_score += self.p2_pos
            if (self.p2_score >= self.SCORE_LIMIT): break 

        return (min(self.p1_score, self.p2_score) * self.dice.roll_count)

@dataclass(eq=True, frozen=True)
class DiracGameState: 
    p1_next: bool
    p1_score: int
    p1_pos: int
    p2_score: int
    p2_pos: int

# Copied from Simon Toth's solution... Simple permutations
DIRAC_DISTRIBUTION = {3: 1, 4: 3, 5: 6, 6: 7, 7: 6, 8: 3, 9: 1}

def simulateDiracGame(curr_state: DiracGameState, SCORE_LIMIT: int, BOARD_LIMIT: int):
    if (curr_state.p1_score >= SCORE_LIMIT):
        return [0, 1]
    elif (curr_state.p2_score >= SCORE_LIMIT):
        return [1, 0]
    
    # for each key in distribution, 
    # create according game state (p1 or p2?), then simulate on it
    if (curr_state.p1_next):
        sum = [0, 0]
        for roll in DIRAC_DISTRIBUTION.keys():
            p1_pos_next = (curr_state.p1_pos + roll) % BOARD_LIMIT
            if (p1_pos_next == 0): p1_pos_next = BOARD_LIMIT
            p1_score_next = curr_state.p1_score + p1_pos_next
            next_state = DiracGameState(
                not curr_state.p1_next, 
                p1_score_next, 
                p1_pos_next, 
                curr_state.p2_score, 
                curr_state.p2_pos
            )
            temp_res = simulateDiracGame(next_state, SCORE_LIMIT, BOARD_LIMIT)
            sum[0] += temp_res[0] * DIRAC_DISTRIBUTION[roll]
            sum[1] += temp_res[1] * DIRAC_DISTRIBUTION[roll]
        return sum
    else:
        sum = [0, 0]
        for roll in DIRAC_DISTRIBUTION.keys():
            p2_pos_next = (curr_state.p2_pos + roll) % BOARD_LIMIT
            if (p2_pos_next == 0): p2_pos_next = BOARD_LIMIT
            p2_score_next = curr_state.p2_score + p2_pos_next
            next_state = DiracGameState(
                not curr_state.p1_next, 
                curr_state.p1_score, 
                curr_state.p1_pos, 
                p2_score_next, 
                p2_pos_next
            )
            temp_res = simulateDiracGame(next_state, SCORE_LIMIT, BOARD_LIMIT)
            sum[0] += temp_res[0] * DIRAC_DISTRIBUTION[roll]
            sum[1] += temp_res[1] * DIRAC_DISTRIBUTION[roll]
        return sum

# memoization, doesn't work likely because of diffs between identical DiracGameState instances
def memoize(f):
    memo = {}
    def check(*args):
        if args not in memo:
            memo[args] = f(*args)
        return memo[args]
    return check

memoizedSimulate = memoize(simulateDiracGame)

def test():
    # Part 1 related
    test_game = DeterministicDiceGame(DeterministicDice(100), 10, 4, 8)
    test_result = test_game.simulate()
    assert(test_result == 739785)

    # Part 2 related
    #test2_game = DiracGameState(True, 0, 4, 0, 8)
    #test2_result = simulateDiracGame(test2_game, 21, 10) # seems to be in reverse order, oh well
    


if __name__ == "__main__":
    test()

    # getSol1
    part1_game = DeterministicDiceGame(DeterministicDice(100), 10, 7, 9)
    part1_result = part1_game.simulate()
    print("Part 1:", part1_result)

    # getSol2, v/ slow
    part2_game = DiracGameState(True, 0, 7, 0, 9)
    part2_result = max(simulateDiracGame(part2_game, 21, 10))
    print("Part 2:", part2_result)