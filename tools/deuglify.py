import json
import sys
import io

# Convert messages from cpp client to server
def decode(player_commands, player_data):
    cmds = player_commands
    if cmds[0] == "claim":
        msg = {"claim":{"punter":cmds[1], "source":cmds[2], "target":cmds[3]},
               "state": player_data
              }
        return json.dumps(msg)

    if cmds[0] == "pass":
        msg = {"pass":{"punter":cmds[1]},
               "state": player_data
              }
        return json.dumps(msg)

    return None



'''
Whoops - don't need this

    state = ""
    substate = ""
    msg = {"sites" :[], "rivers":[], "mines":[]}
    assert player_commands[0] == "setup"
    for i in range(len(player_commands)):
        tok = player_commands[i]
        if tok == "setup": assert state == ""; state = "setup"; substate = "id"
        if tok == "rivers": assert state == ""; state = "rivers"
        if tok == "mines": assert state == ""; state = "mines"
        if tok == "end": state = ""

        if state == "setup":
            if substate == "id":
                
            msg[state].append(tok)

        if state == "rivers":
            

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
'''

