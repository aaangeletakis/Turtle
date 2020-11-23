import re
import json
import sys
import os.path
from os import path

if len(sys.argv) != 2:
    print("error - file not specified")
    quit(1)
if path.isfile(sys.argv[1]) != True:
    print("error - '%s' not a file"%sys.argv[1])
    quit(1)

regex = r"[rR]?\"\"\"(?:[^\\\"]|\\.)*\"\"\"|([rR]?'(?:[^\\']|\\.)*'|[rR]?\"(?:[^\\\"]|\\.)*\")|(#[^\r\n]*)|(\n[ \t]*)|(\\[^\r\n]*)|^|(([<>*\/]{2}=?)|([=<>!+\-*\\&|]=))|([!\"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]|[^\s!\"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]+)"

text = ""
with open(sys.argv[1], 'r') as f:
    text = f.read()

matches = re.finditer(regex, text, re.MULTILINE)

res=[]
for match in enumerate(matches, start=1):
    res.append(match.group())
j = json.JSONEncoder().encode(res)
with open("tokens.ttl", 'w') as f:
  f.write(j)