#! /bin/sh
find src -name "*.cc" | xargs -r ./tools/cpplint.py --root=src 
find src -name "*.h"  | xargs -r ./tools/cpplint.py --root=src 
find cmd -name "*.cc" | xargs -r ./tools/cpplint.py --root=cmd 
find cmd -name "*.h"  | xargs -r ./tools/cpplint.py --root=cmd 
