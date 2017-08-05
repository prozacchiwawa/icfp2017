#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import json
import hashlib
import BaseHTTPServer

data = {}
pagenum = 0
alldata = []

class IndexRequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    def doindex(self,pagenum):
        md5 = hashlib.md5()
        index_data = open('render/index.html').read() % json.dumps(alldata[pagenum])
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
            f = open(os.path.join(os.getcwd(), 'render', self.path[1:])).read()
            self.wfile.write(f)
            self.wfile.close()
        else:
            self.wfile.write("dont know about " + self.path)
            self.wfile.close()

if __name__ == '__main__':
    dataraw = open(sys.argv[1]).read()
    if dataraw[0] == '{':
        data = json.loads(dataraw)
        alldata = [data]
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
                data = json.loads(json.dumps(data))
                if not 'used' in data:
                    data['used'] = {}
                for m in msg[key]['moves']:
                    if 'claim' in m:
                        c = m['claim']
                        data['used']['%s,%s' % (c['source'],c['target'])] = c['punter']
                alldata = alldata + [data]
                print 'pages %s' % len(alldata)
            elif 'claim' in msg:
                data = json.loads(json.dumps(data))
                if not 'used' in data:
                    data['used'] = {}
                c = msg['claim']
                data['used']['%s,%s' % (c['source'],c['target'])] = c['punter']
            else:
                print msg
            
    server = BaseHTTPServer.HTTPServer(('0.0.0.0', 8005), IndexRequestHandler)
    server.serve_forever()
