import json
import sys
import io

if __name__ == '__main__':
    iam = sys.argv[1]
    players = sys.argv[2]
    
    f = json.loads(io.open(sys.argv[3], encoding='utf-8').read())
    print "setup %s %s" % (iam, players)
    for x in f['sites']:
        print x['id']
    print "end"
    for x in f['rivers']:
        print "%s %s" % (x['source'],x['target'])
    print "end"
    for x in f['mines']:
        print x
    print "end" # End mines
    print "end" # End initial moves

