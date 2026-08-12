#!/usr/bin/env python3
import os

print("Content-type: text/html\n")
for key, value in os.environ.items():
    print(f"{key} --> {value}<br>")
