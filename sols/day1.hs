-- Solution to Day 1: Sonar Sweep
-- Puzzle input: # lines. Each line representing a measurement
-- of the sea floor depth as the sweep looks further away from
-- you (the sub).

-- Your objective is to figure our how quickly the depth increases.
-- In this Q you need to count the # a depth measurement increases
-- from prev. measurement. First measurement should have good 
-- N/A result.

-- Part 1:
-- Get # of increase in measurements from first to last measurement.
getIncreases :: [Int] -> Int
getIncreases [] = 0
getIncreases (a:[]) = 0
getIncreases (a:b:as) = comp a b + getIncreases (b:as)
    where comp a b = if b > a then 1 else 0

-- Part 2:
-- Get # of 3-measurement sliding windows
getSlideIncreases :: [Int] -> Int
getSlideIncreases [] = 0
getSlideIncreases (_:[]) = 0
getSlideIncreases (_:_:[]) = 0
getSlideIncreases (_:_:_:[]) = 0
getSlideIncreases (a:b:c:d:as) = comp sum1 sum2 + getSlideIncreases (b:c:d:as)
    where sum1 = a + b + c
          sum2 = b + c + d
          comp a b = if b > a then 1 else 0


-- Common:
-- Parse file from filepath to IO
parseFile :: FilePath -> IO [Int]
parseFile filepath = do
    content <- readFile filepath
    return (parseInts content)

parseInts :: String -> [Int]
parseInts string = map readInt $ words string

readInt :: String -> Int
readInt = read

-- Get solutions
getSol1 :: IO()
getSol1 = do{
    dists <- parseFile "./resource/q1/input";
    -- part 1
    print $ getIncreases dists;
    -- part 2
    print $ getSlideIncreases dists;
}


