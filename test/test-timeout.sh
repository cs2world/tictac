#!/bin/bash
##################################################
##
## Tictactoe time limit test
## version 0.0.1
## by  PY Cheng
## April. 19th, 2021
##
##################################################
# the host
unlock.sh 1test2tester
unlock.sh 3test2tester
# the challenger
unlock.sh 2test2tester
unlock.sh 4test2tester
# test case 1
# create two new games (1test2tester:2test2tester and 3test2tester:4test2tester)
# waiting for 1 minute+, then
# 1test2tester as a host does next move(not its turn), while
# 4test2tester as a challenger does the next move (its turn)
# result, winner=1, game over, host wins.
# as this one is simple, just do it manual.
