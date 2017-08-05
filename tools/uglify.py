import json
import sys
import io

def convert_map(iam, players, json_map):
    msg = ""
    msg += "setup %s %s " % (iam, players)
    for x in json_map['sites']:
        msg += str(x['id']) + " "
    msg += "end "
    for x in json_map['rivers']:
        msg += "%s %s " % (x['source'],x['target'])
    msg += "end "
    for x in json_map['mines']:
        msg += str(x) + " "
    msg += "end " # End mines
    msg += "end " # End initial moves
    return msg

if __name__ == '__main__':
    iam = sys.argv[1]
    players = sys.argv[2]
    
    f = json.loads(io.open(sys.argv[3], encoding='utf-8').read())
    print convert_map(iam, players, f)


