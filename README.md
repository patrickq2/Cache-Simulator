# Cahce-Simulator

Objective:
design and implement a cache simulator. Your simulator will read a memory access 
trace from a file, determine whether each memory access is a hit or a miss, and 
output the hit rate.

You will then analyze the performance (hit rate) of three cache designs. 
You will do this in an analysis paper which should answer the following questions:

- How does the performance of a cache change with its associativity? 
(e.g. direct mapped vs n-way associative vs fully associative)
- How does the performance of a cache change with cache size?
- How does the performance change with the replacement policy?

read in trace file - example:
(1, 0x1fffff78, 2)
- 1st column indicates if load or store; we can ignore this
- 2nd column indicates the address; we must read the addresses
- 3rd column indicates how many bytes requested; we can ignore this

1. Specify Cache Design:
- how many bytes in cache?
- How many bytes in a block/line?a
- How many lines in set? ((if set is needed))
These are input parameters so the user inputs this to their preference.
- use this info to calculate number of lines in the cache with: 
- (bytes in cache)/(bytes in a block/line) = number of lines

2. Make the actual Design:
Vector/Array/Map with element for each line of the Cache
(these are just one POSSIBLE example for each)
- Direct Mapped: store  std::vector<std::tuple<int, int, int, char> > DirectMapExample;
- Set Associative: store std::vector<std::tuple<int, int, int, char> > SetAssociativeExample;
- Fully Associative: store std::vector<std::pair<int, int, char> > FullAssociativeExample;
The size of this structure is determined from the num of lines calculated in part 1
(the char is to store hits/misses see step 4 for that)

3. Reading addresses (2nd column):
0x1fffff78
split into appropriate fields
- if Direct Mapped then: Tag/Line/Offset
- if Set Associative then: Tag/Set/Offset
- if Fully Associative then: Tag/Offset
11111
How do we split it? 
-EX: take the hex (0x`)
given bytes in cache: 512
given bytes in line: 64

DIRECTED MAP 
-- 1. convert to binary(0001 1111 1111 1111 1111 1111 0111 1000)
-- 2. split Tag/Line/Offset
--- width of Offset is Log2(bytes in a line) (Log2(64) = 6)
--- width of line is Log2(number of lines) see step 1 to calc #ofLines: (Log2((512/64)) = 3)
--- separate the binary: tag:00011111111111111111111 offset:101 line:111000
--- convert tag to double for easy storage in map
------- tag:1048575 line:101 offset:111000
------- Your index where you save the data in your structure is the LINE number 
------- (in this ex... line:101 convert this to decimal to get line:5 so store at index 5)
-- 3. store those values in the data structure made previously map/vector/array/etc

FULLY ASSOCIATIVE MAP
-- 1. convert to binary(0001 1111 1111 1111 1111 1111 0111 1000)
-- 2. split Tag/Offset
--- width of Offset is Log2(bytes in a line) (Log2(64) = 6)
--- separate the binary: tag:00011111111111111111111101 offset:111000
--- convert tag to double for easy storage in map
------- tag:8388605 line:101 offset:111000
-- 3. store those values in the data structure made previously map/vector/array/etc
------- find the first open index and yeet it there, iterate till you find one open.

SET ASSOCIATIVE MAP (given associativity of 2 lines in set)
-- 1. convert to binary(0001 1111 1111 1111 1111 1111 0111 1000)
-- 2. split Tag/Set/Offset
--- width of Offset is Log2(bytes in a line) (Log2(64) = 6)
--- width of set is Log2((number of lines)/(lines in set)) see step 1 to calc #ofLines: (Log2((512/64)/3) = 2)
--- separate the binary: tag:000111111111111111111111 offset:01 line:111000
--- convert tag to double for easy storage in map
------- tag:2097151 set:01 offset:111000
------- Your index where you save the data in your structure is: (set number) X (lines in set) 
------- (in this ex... set:01 convert this to decimal to get line:1 so store at index (1) X (2) = 2
------- if it is already full you can go to the next index and enter there (if that new index is empty).
Remember: travel less than the lines in the set. For example this ex has us inserting at index 2, if 2 is full 
we check if index 3 is full and can insert if it is not (this is because the question gives us an associativity
of 2, so we can check the index given at (set number) X (lines in set) (the first index of our set)
AND 1 more after (the 2nd index of our set. If both are full, overwrite the data in the first index given.)
-- 3. store those values in the data structure made previously map/vector/array/etc

4. HIT OR MISS (I guess they never miss huh?)
https://www.youtube.com/watch?v=RqKeEIbcnS8 (if this section is confusing watch this)
The previous step was storing stuff in the cache. Now, if we try in insert something 
in an ALREADY OCCUPIED space that HAS THE SAME TAG (if different tags just replace the data)
then you will do this:

look at the cache structure, I'm using my ex in step 2:
- Direct Mapped: store  std::vector<std::tuple<int, int, int, char> > DirectMapExample;
- Set Associative: store std::vector<std::tuple<int, int, int, char> > SetAssociativeExample;
- Fully Associative: store std::vector<std::pair<int, int, char> > FullAssociativeExample;

the first int for all of these is the tag ^
DirectMapExample.push_back( tuple<int, int, int, char>(Tag, Line, Offset, isFull) );
std::cout << std::get<3>(DirectMapExample[(index you are looking at)]); // gets the char isFull
std::cout << std::get<0>(DirectMapExample[(index you are looking at)]); // gets the tag

when we insert a HEX, the data at that point should be inserted and the char turned to 'F' for Full
you could check if its full other ways, for instance having no variable for it and just checking if
the data is 'null', but in this example I'm using a char with F to signify full and E to signify empty
Now
-- IF it is 'F' (full) when we go to add data, 
-- AND IF std::get<3>(DirectMapExample[(index you are looking at)]) == tag of what we are inserting
-------THEN we increase a variable called 'Hit'. 
-- ELSE IF 'E' (empty), increase a variable called 'miss'.

EX (DIRECTED MAP)
given bytes in cache: 128
given bytes in line: 32

some inserts of different hexes: 
Address     Tag     Set    offset
0x070      00000    11     10000    miss
0x080      00001    00     00000    miss
0x068      00000    11     01000    hit
https://www.youtube.com/watch?v=RqKeEIbcnS8 see this video for step by step explanation.


5. LRUR and FIFO
Least recently used (LRU)
First in, first out (FIFO)
-save the hit counter from step 4.
- if you are in DIRECTED MAP and hit a spot that is already filled with a different tag 
from what you are inserting then just replace the data at the index, its fairly simple.
- If you are in FULLY ASSOCIATIVE and hit a spot that is already filled with a different 
tag from what you are inserting, use the HIT variable to calculate the index to replace
------ LRU and FIFO calculated with index