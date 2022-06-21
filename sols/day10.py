# Parsing
# Grammar looks like LL-1 (i.e., we always know from the current token 
# and current non-terminal which rule to apply)

from io import StringIO
from pathlib import Path

GRAMMAR = {'(': ')', 
           '[': ']', 
           '{': '}', 
           '<': '>'}

SCORES = {')': 3, 
         ']': 57, 
         '}': 1197, 
         '>': 25137}

SCORES_PT2 = {')': 1, 
              ']': 2, 
              '}': 3, 
              '>': 4}

def findFirstIllegalChar(line: str) -> str:
    stack = []
    with StringIO(line) as line_st:
        next_char = line_st.read(1)
        
        while (next_char != ''):
            if (next_char in GRAMMAR.values()):
                if (len(stack) == 0):
                    return next_char 
                    # opening char stack exhausted but found unexpected closing char
                elif (GRAMMAR[stack[-1]] != next_char):
                    return next_char
                    # current opening char in stack not closed with correct closing char
                else:
                    stack.pop()
                    # Matching opening/close char pair, exit to outside opening char
            else:
                stack.append(next_char)
                # Otherwise next opening char found, enter inside opening char
            next_char = line_st.read(1)

        if (len(stack) != 0): # Opening char not closed at EOL
            return "SUPPRESSED" # Incomplete errors suppressed

    return "GOOD"

def getCompletionString(line: str) -> str:
    # In true make-do fashion, 
    # This function suppress all corrupted lines much like how above 
    # suppress all incomplete lines.
    # A more versatile solution would be to return a encapsulated data-type
    # for both actions but this is faster.
    stack = []
    with StringIO(line) as line_st:
        next_char = line_st.read(1)
        
        while (next_char != ''):
            if (next_char in GRAMMAR.values()):
                if (len(stack) == 0):
                    return "SUPPRESSED"
                    # opening char stack exhausted but found unexpected closing char
                elif (GRAMMAR[stack[-1]] != next_char):
                    return "SUPPRESSED"
                    # current opening char in stack not closed with correct closing char
                else:
                    stack.pop()
                    # Matching opening/close char pair, exit to outside opening char
            else:
                stack.append(next_char)
                # Otherwise next opening char found, enter inside opening char
            next_char = line_st.read(1)

        if (len(stack) != 0): # Opening char not closed at EOL; incomplete
            stack.reverse()
            completion = [GRAMMAR[c] for c in stack]
            return completion

    return "GOOD"

def getSol1(lines: list) -> int:
    score = 0
    for line in lines:
        try:
            culprit = findFirstIllegalChar(line)
            score += SCORES[culprit]
        except KeyError:
            score += 0
    return score

def getSol2(lines: list) -> int:
    scores = []
    for line in lines:
        try:
            completion = getCompletionString(line)
            score = 0
            for c in completion:
                score *= 5
                score += SCORES_PT2[c]
            scores.append(score)
        except KeyError:
            continue
    scores.sort()
    return(scores[len(scores)//2])

def parser(filepath: Path):
    lines = []
    with open(filepath, 'r') as f:
        for line in f:
            lines.append(line.strip())
    return lines

def test():
    test_line_1 = r"(]"
    test_line_2 = r"{()()()>"
    test_line_3 = r"(((()))}"
    test_line_4 = r"<([]){()}[{}])"
    test_lines = {
        r"[({(<(())[]>[[{[]{<()<>>",
        r"[(()[<>])]({[<{<<[]>>(",
        r"{([(<{}[<>[]}>{[]{[(<()>",
        r"(((({<>}<{<{<>}{[]{[]{}",
        r"[[<[([]))<([[{}[[()]]]",
        r"[{[{({}]{}}([{[{{{}}([]",
        r"{<[[]]>}<{[{[{[]{()[[[]",
        r"[<(<(<(<{}))><([]([]()",
        r"<{([([[(<>()){}]>(<<{{",
        r"<{([{{}}[<[[[<>{}]]]>[]]"
    }

    assert(findFirstIllegalChar(test_line_1) == ']')
    assert(findFirstIllegalChar(test_line_2) == '>')
    assert(findFirstIllegalChar(test_line_3) == '}')
    assert(findFirstIllegalChar(test_line_4) == ')')
    assert(getSol1(test_lines) == 26397)

    test_parsed_lines = parser(Path().absolute().joinpath('resource/q10/input_test'))
    assert(getSol1(test_parsed_lines) == 26397)

    assert(getSol2(test_parsed_lines) == 288957)


if __name__ == "__main__":
    test()
    parsed_lines = parser(Path().absolute().joinpath("resource/q10/input"))
    # 1.
    print ("Part 1:", getSol1(parsed_lines))
    # 2.
    print ("Part 2:", getSol2(parsed_lines))
