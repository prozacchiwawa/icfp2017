#!/usr/bin/env python

from __future__ import print_function
import json
import sys
import itertools
import subprocess as sub
import base64
import io # Needed for Python 2.7 to use Python 3-style open with encoding
import uglify

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

def player_run():
    return sub.Popen(["./bin/main"], stdout=sub.PIPE, stdin=sub.PIPE)

def dbg(msg):
    print(msg, file=sys.stderr)

# Map contains nodes, rivers & mine IDs
# Number of players is implictly = # num of rivers

def player_setup(p, player_id, num_players, map):
    import uglify
    #dbg ("MAP: {}".format(map))
    msg = uglify.convert_map(player_id, num_players, map)
    # no commands (claim,pass) are sent to the server during setup
    return player_comm(p, player_id, msg)[-1]

# Input state is turns from last time, plus player state from last time
# prev_round_moves, prev_player_data are rom the SERVER
def player_turn(p, player_id, prev_round_moves, prev_player_data):
    dbg("PREV_ROUND_MOVES: {}".format (prev_round_moves))
    prev_moves_str = uglify.convert_moves(prev_round_moves)
    msg  = "{} {}".format(prev_moves_str, prev_player_data)
    player_tokens = player_comm(p, player_id, msg)
    player_data = player_tokens[-1]
    player_commands = player_tokens[:-1]
    return (player_commands, player_data)

def player_comm(p, player_id, msg):
    dbg ("SERVER->client: {}".format(msg))
    (stdout_data, stderr_data) = p.communicate(msg)
    dbg ("CPP-stdout: {}".format(stdout_data))
    dbg ("CPP-errors: {}".format(stderr_data))
    player_tokens = stdout_data.split()
    p.wait()
    return player_tokens
    #player_commands = player_tokens[:-1]

    #dbg ("PLAYER COMMANDS: {}\n PLAYER DATA: {}".format(player_commands, base64.b64decode(player_data)))
    #return (player_commands, player_data)



if __name__ == '__main__':

    config = read_config()
    map = read_json_file(config["mapfile"])
    num_players = config["num_players"]

    dbg ("CONFIG: {}".format(config))

    p = player_run()

# setup <player_id> <num_players> [node_ids] end [edges] end [mines] end
#(stdoutdata, stderrdata) = p.communicate("setup 0 2 0 1 2 3 end 0 1 1 3 0 2 2 3 end 0 end")

#print( "OUT: {} ERR: {}".format(stdoutdata, stderrdata))



    (player_commands, player_data) = player_setup(p, 0, map)

    max_moves = 4
    moves = 1

#player_commands = {}
#player_data = {}


    while moves < max_moves:
        p = player_run()
        (player_commands, player_moves) = player_turn(p, 0, player_commands, player_data)
        moves +=1

