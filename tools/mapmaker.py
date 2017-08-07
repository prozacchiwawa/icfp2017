#!/bin/env python

import json
import argparse


parser = argparse.ArgumentParser()
parser.add_argument("type", choices=["one", "long", "load", "double"] )
parser.add_argument("--load", type=str)
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

def loadmap_fun(map):
    return json.loads(open(args.load).read())

def doublemap_fun(map):
    map = loadmap_fun(map)
    v = len(map['sites'])
    oldsites = map['sites']
    oldrivers = map['rivers']
    oldmines = map['mines']
    map['mines'] = map['mines'] + [x + v for x in oldmines]
    map['sites'] = map['sites'] + [{'id':x['id']+v} for x in oldsites]
    map['rivers'] = map['rivers'] + [{'target':x['target']+v,'source':x['source']+v} for x in oldrivers]
    map['rivers'] = map['rivers'] + [{'source':v-1,'target':v}]
    return map

# ideas: loop, tree, n-loop(topology)
# every node is a mine

m = {
    "one" : { "map": onemap, "fun": onemap_fun },
    "long" : { "map": emptymap, "fun": longmap_fun },
    "load" : { "map": emptymap, "fun": loadmap_fun },
    "double" : { "map": emptymap, "fun": doublemap_fun }
}

print json.dumps(m[args.type]["fun"](m[args.type]["map"]))



