H Testing Plan

There are 8 patterns for Challenger' win and
8 patterns for Host's win, and some draw cases.
for Challenger:
case0  |  case1 | case2  | case3 | case4 | case5 | case6 | case7
c c c  |  ? ? ? | ? ? ?  | c ? ? | ? c ? | ? ? c | c ? ? | c ? ?
? ? ?  |  c c c | ? ? ?  | c ? ? | ? c ? | ? ? c | ? c ? | ? c ?
? ? ?  |  ? ? ? | c c c  | c ? ? | ? c ? | ? ? c | ? ? c | ? ? c

for host
case0  |  case1 | case2  | case3 | case4 | case5 | case6 | case7
H H H  |  ? ? ? | ? ? ?  | H ? ? | ? H ? | ? ? H | H ? ? | H ? ?
? ? ?  |  H H H | ? ? ?  | H ? ? | ? H ? | ? ? H | ? H ? | ? H ?
? ? ?  |  ? ? ? | H H H  | H ? ? | ? H ? | ? ? H | ? ? H | ? ? H

For draw
case0 | case1
H C H | H C C
C H C | C H H
C H C | C H C

there are two accounts
1test2tester
2test2tester

and 8 files for testing challenger wins
challenger-win-pattern[0-7].json
and 8 files for testing host wins
host-win-pattern[0-7].json
and 2 files for draw
draw-pattern[01].json

using
cleos push transactions json-files
