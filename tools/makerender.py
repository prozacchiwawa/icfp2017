#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import json
from math import sqrt
import hashlib
import BaseHTTPServer

data = {}
pagenum = 0
alldata = []

class IndexRequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    def doindex(self,pagenum):
        md5 = hashlib.md5()
        alldata[pagenum]['page'] = pagenum
        alldata[pagenum]['size'] = 100 * sqrt(len(alldata[pagenum]['sites']))
        alldata[pagenum]['pages'] = len(alldata)
        index_data = open('tools/render/index.html').read() % json.dumps(alldata[pagenum])
        md5.update(index_data)
        self.send_response(200)
        self.send_header('content-type', 'text/html; charset=utf-8')
        self.send_header('etag', md5.hexdigest())
        self.end_headers()
        self.wfile.write(index_data)
        self.wfile.close()
            
    def do_GET(self):
        if self.path == '/index.html' or self.path == "/":
            pagenum = 0
            self.doindex(pagenum)
        elif self.path.endswith('.html'):
            pagenum = int(self.path[1:-5])
            print ('page %s' % pagenum)
            self.doindex(pagenum)
        elif self.path.endswith('.js'):
            f = open(os.path.join(os.getcwd(), 'tools/render', self.path[1:])).read()
            self.wfile.write(f)
            self.wfile.close()
        else:
            self.wfile.write("dont know about " + self.path)
            self.wfile.close()

def interpret_setup(player_commands):
    state = ""
    substate = ""
    msg = {"sites" :[], "rivers":[], "mines":[], "used":{}}
    assert player_commands[0] == "setup"
    filling = 0
    filling_list = ['sites', 'rivers', 'mines', 'moves', 'moves']
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
                msg['mines'] += [int(player_commands[i])]
            elif key == 'moves':
                movetype = tok
                if movetype == 'claim':
                    i += 1
                    punter = int(player_commands[i])
                    i += 1
                    source = int(player_commands[i])
                    i += 1
                    target = int(player_commands[i])
                    if source < target:
                        ukey = "%s,%s" % (source,target)
                    else:
                        ukey = "%s,%s" % (target,source)
                    msg['used'][ukey] = punter
                elif movetype == 'pass':
                    i += 1
        i += 1
    return msg

def interpret_move(player_commands, data):
    i = 0
    while player_commands[i] != 'end':
        if player_commands[i] == 'claim':
            i += 1
            punter = int(player_commands[i])
            i += 1
            source = int(player_commands[i])
            i += 1
            target = int(player_commands[i])
            if source < target:
                ukey = "%s,%s" % (source,target)
            else:
                ukey = "%s,%s" % (target,source)
            data['used'][ukey] = punter
        i += 1
    return data
        
if __name__ == '__main__':
    dataraw = open(sys.argv[1]).read()
    if dataraw[0] == '{':
        data = json.loads(dataraw)
        alldata = [data]
    elif dataraw[0] == 'c':
        stanzas = []
        for ll in dataraw.split('\n'):
            l = ll.strip()
            if len(l) == 0:
                continue
            elif l.startswith('continue'):
                stanzas += [l]
            else:
                stanzas[-1] += ' ' + l
        stanzas = [x.strip().split() for x in stanzas]
        data = interpret_setup(stanzas[0][1:])
        alldata = [data]
        for stanza in stanzas[1:]:
            data = interpret_move(stanza[2:], {'used':json.loads(json.dumps(data['used'])), 'mines':data['mines'], 'rivers':data['rivers'], 'sites':data['sites']})
            alldata += [data]
    else:
        process = [""]
        for ll in dataraw.split('\n'):
            l = ll.strip()
            if len(l) == 0:
                continue
            elif len(l) == 1:
                process[-1] += l
            else:
                if l[:2] == '->' or l[:2] == '<-':
                    process = process + [l[2:]]
                else:
                    process[-1] += l
        process = filter(lambda x: len(x.strip()) > 0, process)
        for msg in [json.loads(m) for m in process]:
            if 'map' in msg:
                data = msg['map']
                alldata = [data]
            elif 'move' in msg or 'stop' in msg:
                key = 'move'
                if 'stop' in msg:
                    key = 'stop'
                data = {'used':json.loads(json.dumps(data['used'])), 'mines':data['mines'], 'rivers':data['rivers'], 'sites':data['sites']}
                if not 'used' in data:
                    data['used'] = {}
                for m in msg[key]['moves']:
                    if 'claim' in m:
                        c = m['claim']
                        data['used']['%s,%s' % (c['source'],c['target'])] = c['punter']
                alldata = alldata + [data]
                print 'pages %s' % len(alldata)
            elif 'claim' in msg:
                data = {'used':json.loads(json.dumps(data['used'])), 'mines':data['mines'], 'rivers':data['rivers'], 'sites':data['sites']}
                if not 'used' in data:
                    data['used'] = {}
                c = msg['claim']
                data['used']['%s,%s' % (c['source'],c['target'])] = c['punter']
            else:
                print msg
            
    server = BaseHTTPServer.HTTPServer(('0.0.0.0', 8005), IndexRequestHandler)
    server.serve_forever()
