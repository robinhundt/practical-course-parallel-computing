#!/usr/bin/env python
import sys

for line in sys.stdin:
    line = line.strip()
    words = line.split()
    int count ==0
    for word in words:
        if count%2:
            print '%s\t%s' % (word, 1)
        count+=1
