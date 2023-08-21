#!/usr/bin/env python3

from pathlib import Path
import sys


def err_abort(msg):
    print(msg, file=sys.stderr)
    exit(1)

def main(source_dir, target_dir):
    p = Path(source_dir)
    benchmarks = list(p.glob("**/*.aot"))
    benchmarks += list(p.glob("**/*.wasm"))
    defs = []
    names = set()
    for path in benchmarks:
        name = path.stem.replace("-", "_")
        if name in names:
            err_abort(f"Dual definition of WASM module: {name}\n")
        names.add(name)
        with open(path, mode='rb') as file:
            content = file.read()
            defs.append(generate_array(name, content))
    content = "\n".join(defs)
    header = f"""#ifndef H_BENCHMARKS
#define H_BENCHMARKS
#include <stdint.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored \"-Wunused-variable\"

{content}

#pragma GCC diagnostic pop
#endif
    """
    with open(Path(target_dir) / "./benchmarks.h", mode='w') as output:
        output.write(header)

def generate_array(name, content):
    content = " ,".join([f'0x{byte:x}'  for byte in content])
    content = f'static uint8_t {name}[] __attribute__((aligned (4))) = {{{content}}};\n'
    return content

if __name__ == "__main__":
    if len(sys.argv) < 3:
        err_abort("Usage: python3 generate-headers.py [MODULE_DIR] [TARGET_DIR]\n")
    SOURCE_DIR = sys.argv[1]
    OUT_DIR = sys.argv[2]
    main(SOURCE_DIR, OUT_DIR)