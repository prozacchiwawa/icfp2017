
# Team Name
The team's name is the Aged Aged Man, but the team's name is called Haddock's Eyes

# Team Members
Art Yerkes art.yerkes <at> gmail.com
Adam Kelly adam.q.kelly <at> gmail.com

# To run an online mode game:
```
~/icfp2017/tools$ tools/rungame <port_num>
```

# Viewing sample games:

There is an example game session recorded in examples/sample-game-render.txt
to view it:
```
aqk@ubuntu:~/icfp2017/tools$ (cd tools; python makerender.py ../examples/sample-game-render.txt)
```
Then go to:  http://localhost:8005/


# Profiling

* Set DBG_FLAGS=-pg in Makefile
* ``` make clean && make ```
* ``` tools/rungame ```
* ``` gprof bin/main  | c++filt  | less ```

#Tools
## fightgame
fightgame <our_id> <num_punters> map.crk

To make a map.crk, run tools/uglify.py on a .map file
e.g.
```  tools/uglify.py 0 2 maps/edinburgh-sparse.json  > maps/edinburgh-sparse.crk
 ```

More tools:
sudo apt-get install python-pip
sudo pip install grof2dot

bin/fightgame 0 1 maps/edinburgh-sparse.crk 

gprof bin/fightgame  | c++filt | gprof2dot | dot -Tpng -o output.png

gprof bin/main  | c++filt | gprof2dot | dot -Tpng -o output.png


time (gprof --min-count 100000 bin/main  |  gprof2dot -s -e 99.9 -n 99.9 | dot -Tps -o output.ps)


