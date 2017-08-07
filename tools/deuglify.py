import json
import sys
import io

# Convert messages from cpp client to server
def decode(player_commands, player_data):
    cmds = player_commands
    if cmds[0] == "claim":
        msg = {"claim":{"punter": int(cmds[1]),
                        "source":int(cmds[2]),
                        "target":int(cmds[3])},
               "state": { "cpp": player_data }
              }
        return json.dumps(msg)

    if cmds[0] == "pass":
        msg = {"pass":{"punter": int(cmds[1])},
               "state": { "cpp": player_data }
              }
        return json.dumps(msg)

    return None

'''
Whoops - don't need this

def decodeStart(player_commands, player_data):
    state = ""
    substate = ""
    msg = {"sites" :[], "rivers":[], "mines":[]}
    assert player_commands[0] == "setup"
    filling = 0
    filling_list['sites', 'rivers', 'mines']
    i = 3
    while i < len(player_commands):
        tok = player_commands[i]
        if tok == 'end':
            filling += 1
        else:
            key = filling_list[filling]
            if key == 'sites':
                msg['sites'] += [{'id':int(tok)}]
            elif key == 'rivers':
                source = int(tok)
                i += 1
                target = int(player_commands[i])
                msg['rivers'] += [{'source':source, 'target':target}]
            elif key == 'mines':
                msg['mines'] += int(player_commands[i])
        i += 1

if __name__ == '__main__':
    iam = sys.argv[1]
    players = sys.argv[2]
    
    f = json.loads(io.open(sys.argv[3], encoding='utf-8').read())
    print convert_map(iam, players, f)
'''

