from pathlib import Path
PATH = "../embench-wasm/"
p = Path(__file__).parent.joinpath(PATH)


def main():
    benchmarks = list(p.glob("*.wasm"))
    defs = []
    for path in benchmarks:
        name = path.stem.replace("-", "_")
        with open(path, mode='rb') as file:
            content = file.read()
            defs.append(generate_array(name, content))
    content = "\n".join(defs)
    header = f"""
#ifndef H_BENCHMARKS
#define H_BENCHMARKS
#include <stdint.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

{content}

#pragma GCC diagnostic pop
#endif
    """
    with open(p.joinpath("benchmarks.h"), mode='w') as output:
        output.write(header)

def generate_array(name, content):
    content = " ,".join([f'0x{byte:x}'  for byte in content])
    content = f'static uint8_t {name}[] = {{{content}}};\n'
    return content

if __name__ == "__main__":
    main()