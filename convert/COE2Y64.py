"""
COE to Y64 Converter
Usage: python COE2Y64.py input_file output_file [initial_RIP] [initial_RSP]
"""

"""
Copyright 2024  Li Zihan

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

import sys
import re
import math


def getSize(xs, b):
    ms1 = "1" + ("0" * (len(xs) - 1))
    m1 = int(ms1, b) - 1
    m2 = int(xs, b)
    return getSize1(m1 if m1 > m2 else m2)


def getSize1(m):
    if m < 256:
        return 1
    else:
        return getSize1(math.floor(math.sqrt(m))) * 2


def toArray(s, b):
    l = getSize(s, b)
    x = int(s, b)
    return toArray1(x, l)


def toArray1(x, l):
    b = [0] * l
    for i in range(l):
        b[i] = x % 256
        x = x // 256
    return b


argc = len(sys.argv)
argv = sys.argv

ifn = ""
ofn = ""
rip = 0x10000
rsp = 0x20000
mrsp = False

if argc > 1:
    ifn = argv[1]
    ofn = argv[2] if argc > 2 else re.sub(r"\.[cC][oO][eE]", r".Y64", ifn)
else:
    ifn = input("Input:  ")
    ofn = input("Output: ")

if argc > 3:
    rip = int(argv[3], 16)
if argc > 4:
    rsp = int(argv[4], 16)
    mrsp = True

ifl = open(ifn, "r")
ofl = open(ofn, "wb")

L1 = ifl.readline()
m1 = re.match(r"MEMORY_INITIALIZATION_RADIX=(\d+);\w*", L1, re.I)
base = 16
if m1:
    base = int(m1.group(1))
L2 = ifl.read().replace("\n", "").replace(" ", "")
m2 = re.match(r"MEMORY_INITIALIZATION_VECTOR=(.*);\w*", L2, re.I)
content = re.compile(r"[0-9a-fA-F]+").findall(m2.group(1))
lb = []
for xs in content:
    lb = lb + toArray(xs, base)

while mrsp and rsp - rip < len(lb) and rip - rsp > 0x10000:
    rsp += 0x10000

lip = toArray1(rip, 8)
lsp = toArray1(rsp, 8)
llb = toArray1(len(lb), 8)
efb = toArray1(0xFFFF_FFFF_FFFF_FFFF, 8)

fb = lip + lsp + llb + lip + lb + efb
bb = bytes(fb)
ofl.write(bb)

ifl.close()
ofl.close()
