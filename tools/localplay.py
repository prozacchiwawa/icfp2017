#!/usr/bin/env python

from __future__ import print_function
import json
import sys
import itertools
import subprocess as sub
import base64
import io # Needed for Python 2.7 to use Python 3-style open with encoding

'''
TODO:
 - Need config (lamduct only accepts one argument for exec()
 - Read maps
 - Pit players against each other
 - Deal with the fact that the local runner controls some functions that the remote runner controls when in remote mode (see remoteplay.py)

'''

def read_json_file(filename):
    with io.open(filename, 'r', encoding="utf-8") as json_file:
        return json.load(json_file)

def read_config():
    return read_json_file("tools/localplayer_config.json")


config = read_config()
map = read_json_file(config["mapfile"])
num_players = config["num_players"]

print ("CONFIG: {}".format(config))

p = sub.Popen(["./bin/main"], stdout=sub.PIPE, stdin=sub.PIPE)

print (p)

# setup <player_id> <num_players> [node_ids] end [edges] end [mines] end
#(stdoutdata, stderrdata) = p.communicate("setup 0 2 0 1 2 3 end 0 1 1 3 0 2 2 3 end 0 end")

#print( "OUT: {} ERR: {}".format(stdoutdata, stderrdata))


# Map contains nodes, rivers & mine IDs
# Number of players is implictly = # num of rivers
def player_setup(player_id, map):
    import uglify
    msg = uglify.convert_map(player_id, len(map['rivers']), map)
    return player_comm(player_id, msg)

# Input state is turns from last time, plus player state from last time
def player_turn(player_id, prev_player_commands, prev_player_data):
    msg  = "move pass 1 {} {}".format(' '.join(prev_player_commands), prev_player_data)
    return player_comm(player_id, msg)

def player_comm(player_id, msg):
    (stdoutdata, stderrdata) = p.communicate(msg)
    print ("SERVER: {}".format(msg))
    player_tokens = stdoutdata.split()
    player_commands = player_tokens[:-1]
    player_data = player_tokens[-1]
    print ("PLAYER: COMMANDS: {} DATA: {}".format(player_commands, player_data))
    return (player_commands, player_data)
    p.wait()

(player_commands, player_data) = player_setup(0, map)

max_moves = 4
moves = 1

#player_commands = {}
#player_data = {}

while moves < max_moves:
    p = sub.Popen(["./bin/main"], stdout=sub.PIPE, stdin=sub.PIPE)
    (player_commands, player_moves) = player_turn(0, player_commands, player_data)
    moves +=1

