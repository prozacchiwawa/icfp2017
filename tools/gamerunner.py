#!/usr/bin/env python

from __future__ import print_function
import json
import sys
import itertools

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

hello_msg = json.dumps(json.loads('{ "me" : "Eyes punter" }'));
hello_len = len(hello_msg)

sys.stdout.write("{}:{}".format(hello_len, hello_msg))


def read_msg(infile):
    with StreamInput(infile) as f:
        str_len = ''.join(itertools.takewhile(lambda x: x != ':', f))
        #print("str_len {}".format(str_len) , file=sys.stderr)

        json_len = int(str_len)
        json_str = sys.stdin.read(json_len)
        return json.loads(json_str)


hello_reply = read_msg(sys.stdin)
initial_state = read_msg(sys.stdin)

my_id = initial_state["punter"]

ready_msg = json.dumps({"ready" : my_id});
ready_len = len(ready_msg)

sys.stdout.write("{}:{}".format(ready_len, ready_msg))

#print(initial_state, file=sys.stderr)



#punter punters map

# 21:{"me": "Eyes punter"}
# 
