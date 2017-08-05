#!/usr/bin/env python

from __future__ import print_function
import json
import sys
import itertools
import subprocess as sub
import base64

p = sub.Popen(["./bin/main"], stdout=sub.PIPE, stdin=sub.PIPE)

print (p)

# setup <player_id> <num_players> [node_ids] end [edges] end [mines] end
(stdoutdata, stderrdata) = p.communicate("setup 0 2 0 1 2 3 end 0 1 1 3 0 2 2 3 end 0 end")

#print( "OUT: {} ERR: {}".format(stdoutdata, stderrdata))

player_tokens = stdoutdata.split()
player_commands = player_tokens[:-1]
player_data = player_tokens[-1]
print ("PLAYER COMMANDS: {} DATA: {}".format(player_commands, player_data))

p.wait()

while True:
    p = sub.Popen(["./bin/main"], stdout=sub.PIPE, stdin=sub.PIPE)
    # (stdoutdata, stderrdata) = p.communicate("move pass 1 claim 0 0 2 end {}".format(player_data)) # static test string
    (stdoutdata, stderrdata) = p.communicate("move pass 1 {} {}".format(''.join(player_commands), player_data))
    #print( "OUT: {} ERR: {}".format(stdoutdata, stderrdata))
    player_tokens = stdoutdata.split()
    player_commands = player_tokens[:-1]
    player_data = player_tokens[-1]
    print ("PLAYER COMMANDS: {} DATA: {}".format(player_commands, player_data))
    p.wait()
