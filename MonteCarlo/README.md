# Monte Carlo
## What's this
This is tic tac toe AI.  
Outputs the probability of winning with a certain move.  

https://en.wikipedia.org/wiki/Monte_Carlo_method

## Build and Run

```
g++ main.cpp
./a.out
```

```
Result Example
Run 100000 times simulation
[ cellNumber] : probability_of_win (#ofWin / #ofTrials)
[ 0]: 60.44% ( 6670 / 11036)
[ 1]: 53.37% ( 5901 / 11056)
[ 2]: 59.75% ( 6706 / 11223)
[ 3]: 53.10% ( 5938 / 11183)
[ 4]: 69.40% ( 7646 / 11018)
[ 5]: 52.67% ( 5777 / 10968)
[ 6]: 60.47% ( 6797 / 11240)
[ 7]: 53.44% ( 5916 / 11071)
[ 8]: 60.04% ( 6728 / 11205)

cellNumber is the following
+-+-+-+
|0|1|2|
+-+-+-+
|3|4|5|
+-+-+-+
|6|7|8|
+-+-+-+
```
