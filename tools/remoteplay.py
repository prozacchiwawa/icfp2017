#!/usr/bin/env python

from __future__ import print_function
import json
import sys
import itertools
import subprocess as sub
import base64
import io # Needed for Python 2.7 to use Python 3-style open with encoding

import gamerunner
import play
import deuglify

def dbg(msg):
    print(msg, file=sys.stderr)

'''
We get re-invoked, even after the first handshake, so we must distinguish
which partof startup we are in by the message sent to us by the server.
'''

gamerunner.send_hello(sys.stdout)
m = gamerunner.read_msg(sys.stdin)

server_msg = gamerunner.read_msg(sys.stdin)

my_id = None
punters = 0
map = None
state = None # Used to pass state into cpp from final stop message

if "map" in server_msg:
    init_state = server_msg
    #dbg ("INIT STATE {}".format(init_state))
    my_id = init_state["punter"]
    dbg ("     ----- We are punter # {} -----".format(my_id))
    punters = init_state["punters"]
    map = init_state["map"]
    p = play.player_run()
    (player_commands, player_data) = play.player_setup(p, my_id, punters, map)
    #dbg("PLAYER_SETUP_STATE: {}".format(player_data))
    gamerunner.send_ready(sys.stdout, my_id, {"cpp":player_data})
elif "move" in server_msg:
    dbg ("MOVE")
    moves = server_msg["move"]["moves"]
    state = server_msg["state"]["cpp"]
    p = play.player_run()
    (player_commands, player_data) = play.player_turn(p, my_id, moves, state)
    msg = deuglify.decode(player_commands, player_data)
    dbg(msg)
    gamerunner.send_msg(sys.stdout, my_id, msg)
elif "stop" in server_msg:
    dbg ("STOP")
    moves = server_msg["stop"]["moves"]
    scores = server_msg["stop"]["scores"]
    dbg("SCORES: {}".format(scores))
    p = play.player_run()
    (player_commands, player_data) = play.player_turn(p, my_id, moves, state)
    msg = deuglify.decode(player_commands, player_data)
    dbg(msg)
    gamerunner.send_msg(sys.stdout, my_id, msg)
else:
    raise "unknown server message: {}".format(server_msg)

sys.exit(0)






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

debug ("CONFIG: {}".format(config))

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
    dbg ("SERVER: {}".format(msg))
    player_tokens = stdoutdata.split()
    player_commands = player_tokens[:-1]
    player_data = player_tokens[-1]
    dbg ("PLAYER: COMMANDS: {} DATA: {}".format(player_commands, player_data))
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

