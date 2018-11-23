#! /bin/sh
find src -name "*.cc" | xargs ./tools/cpplint.py --root=src 
find src -name "*.h"  | xargs ./tools/cpplint.py --root=src 
