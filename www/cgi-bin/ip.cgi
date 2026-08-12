#!/usr/bin/python3

import os

print("Content-Type: text/html")
print("")
print(os.environ["REMOTE_ADDR"])
