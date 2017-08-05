#!/usr/bin/env python

from __future__ import print_function
import json
import sys
import itertools
import subprocess as sub
import base64

class StreamInput(object):
    def __init__(self, file):
        self.file = file

    def __enter__(self):
        return self                                                             

    def __exit__(self, *args, **kwargs):
        #self.file.close()
        pass

    def __iter__(self):
        return self                                                             

    def next(self):
        c = self.file.read(1)                                             
        if c == None or c == "":
            raise StopIteration

        return c


#print("Hi", file=sys.stderr)

def dbg(msg):
    print(msg, file=sys.stderr)


def send_to_server(f, msg):
    msg_len = len(msg)
    fmsg = "{}:{}".format(msg_len, msg)
    dbg("PLAYER_READY_MSG: {}".format(fmsg))
    f.write(fmsg)
    f.flush()

def send_hello(f):
    hello_msg = json.dumps(json.loads('{ "me" : "Eyes punter" }'));
    send_to_server(f, hello_msg)

def read_msg(infile):
    with StreamInput(infile) as f:
        str_len = ''.join(itertools.takewhile(lambda x: x != ':', f))
        #print("str_len {}".format(str_len) , file=sys.stderr)

        json_len = int(str_len)
        json_str = sys.stdin.read(json_len)
        return json.loads(json_str)

def send_ready(f, my_id, state):
    ready_msg = json.dumps({"ready" : my_id, "state":state});
    send_to_server(f, ready_msg)

def send_msg(f, my_id, msg):
    #ready_msg = json.dumps({"ready" : my_id, "state":state});
    send_to_server(f, msg)


if __name__ == "__main__":

    send_hello()
    hello_reply = read_msg(sys.stdin)
    initial_state = read_msg(sys.stdin)

    my_id = initial_state["punter"]
    
    send_ready(sys.stdout, my_id)

#print(initial_state, file=sys.stderr)


# TODO: try/catch, loop, feed back base64 state

# main is cppgame
    p = sub.Popen(["./bin/main"], stdout=sub.PIPE, stdin=sub.PIPE)

    print (p)

# setup <player_id> <num_players> [node_ids] end [edges] end [mines] end
    (stdoutdata, stderrdata) = p.communicate("setup 0 2 0 1 2 3 end 0 1 1 3 0 2 2 3 end 0 end")

    print( stdoutdata, stderrdata)

    (stdoutdata, stderrdata) = p.communicate("move pass 1 claim 0 0 2 end")

    print( stdoutdata, stderrdata)



