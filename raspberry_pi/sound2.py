#!/usr/bin/env python
import sys
import random
import os
def play(s):
    sounds = os.listdir(s) # returns list
    sound = random.choice(sounds)
    if not sound == None or sound == '':
        os.system('aplay -q '+ s + '/' + random.choice(sounds) + ' &')

if __name__ == "__main__":
    play(sys.argv[1])
