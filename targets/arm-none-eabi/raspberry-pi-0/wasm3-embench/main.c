//
//  Wasm3 - high performance WebAssembly interpreter written in C.
//
//  Copyright © 2019 Steven Massey, Volodymyr Shymanskyy.
//  All rights reserved.
//

#include <stdio.h>
#include <time.h>
#include "wasm3.h"

#include "extra/fib32.wasm.h"
#include "benchmarks.h"

#define FATAL(msg, ...)                            \
    {                                              \
        printf("Fatal: " msg "\n", ##__VA_ARGS__); \
        return;                                    \
    }
#define BENCHMARK aha_mont64
const char *names[] = {"aha_mont64", "crc32", "cubic", "edn", "huffbench", "matmult_int", "md5sum", "minver", "nbody", "nettle_aes", "nettle_sha256", "nsichneu", "picojpeg", "primecount", "qrduino", "sglib_combined", "slre", "st", "statemate", "tarfind", "ud", "wikisort"};
const uint8_t *benchmarks[] = {aha_mont64, crc32, cubic, edn, huffbench, matmult_int, md5sum, minver, nbody, nettle_aes, nettle_sha256, nsichneu, picojpeg, primecount, qrduino, sglib_combined, slre, st, statemate, tarfind, ud, wikisort};
const uint8_t *sizes[] = {sizeof(aha_mont64), sizeof(crc32), sizeof(cubic), sizeof(edn), sizeof(huffbench), sizeof(matmult_int), sizeof(md5sum), sizeof(minver), sizeof(nbody), sizeof(nettle_aes), sizeof(nettle_sha256), sizeof(nsichneu), sizeof(picojpeg), sizeof(primecount), sizeof(qrduino), sizeof(sglib_combined), sizeof(slre), sizeof(st), sizeof(statemate), sizeof(tarfind), sizeof(ud), sizeof(wikisort)};
void run_wasm()
{
    for (int i = 0; i < sizeof(benchmarks) / sizeof(uint8_t *); ++i)
    {
        M3Result result = m3Err_none;

        uint8_t *wasm = benchmarks[i];
        uint32_t fsize = sizes[i];

        printf("Running benchmark: %s\n", names[i]);

        printf("Loading WebAssembly...\n");
        IM3Environment env = m3_NewEnvironment();
        if (!env)
            FATAL("m3_NewEnvironment failed");

        IM3Runtime runtime = m3_NewRuntime(env, 8192, NULL);
        if (!runtime)
            FATAL("m3_NewRuntime failed");

        IM3Module module;
        result = m3_ParseModule(env, &module, wasm, fsize);
        if (result)
            FATAL("m3_ParseModule: %s", result);

        result = m3_LoadModule(runtime, module);
        if (result)
            FATAL("m3_LoadModule: %s", result);

        IM3Function f;
        result = m3_FindFunction(&f, runtime, "_start");
        if (result)
            FATAL("m3_FindFunction: %s", result);

        printf("Running...\n");

        clock_t start = clock();
        result = m3_CallV(f);
        if (result)
            FATAL("m3_Call: %s", result);

        uint32_t value = 0;
        result = m3_GetResultsV(f, &value);
        if (result)
            FATAL("m3_GetResults: %s", result);

        clock_t end = clock();
        printf("Result: %d\n", value);
        printf("Elapsed: %ld ms\n", (end - start) * 1000 / CLOCKS_PER_SEC);
    }
}

int main()
{
    printf("\n");
    printf("Wasm3 v" M3_VERSION " on HiFive1 (" M3_ARCH "), build " __DATE__ " " __TIME__ "\n");
    run_wasm();

}