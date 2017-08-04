#!/bin/env python

import json
import argparse


parser = argparse.ArgumentParser()
parser.add_argument("type", choices=["one", "long"] )
parser.add_argument("--seed", type=int, default=0)
parser.add_argument("--size", type=int, default=10)
args = parser.parse_args()


emptymap = {
    "sites": [        ],
"rivers": [
],
"mines": [],
"seed": args.seed
}

onemap = {
    "sites": [
        {"id":1},{"id":0}
        ],
"rivers": [
    {"source":0, "target":1},
],
"mines": [1],
"seed": args.seed
}



def make_map():
    pass


def onemap_fun(map):
    return map


def longmap_fun(map):
    map["mines"].append(1)
    for i in range(0, args.size):
        map["sites"].append({"id":i})
        river = {"source":i, "target":i+1}
        if i != args.size-1:
            map["rivers"].append(river)
    return map


# ideas: loop, tree, n-loop(topology)

m = {
    "one" : { "map": onemap, "fun": onemap_fun },
    "long" : { "map": emptymap, "fun": longmap_fun }
}

print json.dumps(m[args.type]["fun"](m[args.type]["map"]))



