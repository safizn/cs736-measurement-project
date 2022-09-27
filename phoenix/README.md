
# phoenix measuring stuff


## about this branch

I wrote this code for benchmarks. Then I pulled in the commit history of
Romit's repository into `../romit` and have been trying to integrate it
with my tests.

## what is successfully done

- rdtsc-based timer
- tests that validate that the timer works correctly
- bandwidth benchmark for pipe
- bandwidth benchmark for shared memory buffer

## how to run

I use the compile-type abstraction technique of defining one header file and
multiple possible implementations. For example, I defined the `ipc_mechanism.h`
header, and there are different `.c` files which define the contained functions
and structs to have implementations such as `ipc_mechanism_buffer.c`,
`ipc_mechanism_pipe.c`, etc. You simply choose which one to pass to the `gcc`
command.

To benchmark throughput with the pipe mechanism:

```
gcc benchmark_throughput.c ipc_mechanism_pipe.c pin_cpu.c rdtsc_timer.c trusted_timer_monotonic_clock.c -o output
./output
```

To benchmark throughput with the buffer mechanism:

```
gcc benchmark_throughput.c ipc_mechanism_buffer.c pin_cpu.c rdtsc_timer.c trusted_timer_monotonic_clock.c -o output
./output
```

Etcetera.

I had originally meant for the `trusted_timer.h` header to also have a
different implementation than `trusted_timer_monotonic_clock.c`, one based on
the `gettime` call, but upon closer reading of the assignment description I no
longer think that's necessary.

## what still needs to be done

- latency benchmark for pipe
    - should be able to do by going into `ipc_mechanism_pipe.c` and defining
      the latency-based headers from `ipc_mechanism.h` by copying and modifying
      the existing code in there to make a two-way trip with a caller-provided
      number of bytes.
- latency benchmark for shared memory buffer
    - similar to above, in `ipc_mechanism_buffer.c`. However I think Romit's
      commented out `schming()` function already does a latency thing, so you
      should possibly just be able to modify that to work.
- bandwidth and latency benchmark for inet sockets
    - Romit's code has `server.c` and `client.c` which contain code for their
      version of TCP profiling. However, in their version, the TCP server sends
      the bytes to the client and measures itself, whereas I think it would fit
      into our testing framework better if we had the client send the bytes to
      the server.

## content for the paper on my timing methodology

The timing is done with a special timer that uses the `rdtscp` instruction
to measure time with higher resolution and less overhead than could be done
with syscalls, so long as proper care is taken to avoid known pitfalls.

According to `lscpu`, the lab computers we're running these benchmarks on have
11th gen intel core i5-11500 processors running at 2700 MHz.

I implemented code to use the `rdtscp` register as described in this paper by
intel:
https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/ia-32-ia-64-benchmark-code-execution-paper.pdf
The machine code stayed basically the same, excepting superficial adaptations
to GCC syntax. As described in the intel paper, this implementation carefully
selects instructions to avoid pitfalls with CPU-level out-of-order execution,
which would result in measurement inaccuracy.

Furthermore I pinned the CPU so it wouldn't have that pitfall
> TODO: elaborate, link article

Furthermore I tried to max out the thread priority but the lab computers wouldn't let me
> TODO: probably superfluous information

The CPU has "invariant TSC" feature which prevents frequency fluctuations from
messing up our measurements, I wrote code that confirms that is true
> TODO: elaborate, link article

To cross-validate the rdtsc-based timer, I implemented a second timer based on
Linux's `clock_gettime` API with `CLOCK_MONOTONIC`. That timer would be
expected to have greater overhead, due to performing a system call, and
possibly lower resolution, but nevertheless be trustworthy in its accuracy,
moreso for larger durations.

I implemented a program, `trusted_timer_resolution_test.c`, to attempt to
determine the resolution of the trusted timer. However, even by only making two
successive timer measurements with nothing in between, the measured duration
changed. This indicates that the smallest measurable period is lesser than the
overhead, which is a problem for me attempting to determine the resolution.
However, it did indicate that the overhead was
> TODO: number, data
which would come in useful later.

I did similar work to measure the overhead of an rdtsc measurement and found
that it was
> TODO: number, elaborate, data

I implemented a program, `sleep_timer_validity_test.c`, to sleep for different
durations, timing it with both the trusted timer and the rdtsc timer, and
measuring the difference. I was unfortunately measuring a discrepancy between
the two timers which scaled linearly with the duration slept. The rdtsc timer
was measuring periods
> TODO: percentage
faster than the trusted timer. Timer overhead was not a sufficient explanation
for this discrepancy, both because the discrepancy was significantly larger
than both the rdtsc timer and trusted timer overhead combined, and because
overhead wouldn't explain why the discrepancy scales linearly.

Eventually I realized that the CPU actually had a timestamp-per-second slightly
higher than the 2700 MHz CPU frequency described by lscpu. By using a trusted
timer to sleep for 5 minutes, and measuring how many timestamps passed in
that duration (using a large duration avoid pitfall) I measured the timestamp-
per-second to be closer to 2711.9914 million than 2700 million.
> TODO: wait I actually ended up doing something even more fancy using point
>       -slope formula 

By using that value in the code instead of 2700 it fixed those inaccuracies
> TODO: elaborate, data

Then I calibrate the rdtsc counter before each run to determine its overhead
and subtract it.
> TODO: elaborate, data, link article

Finally, for each benchmark, I use a loop as such:

- several times:
        - several times:
                - recalibrate the timer
        - several times:
                - do the benchmark

This helps reveal and bypass inaccuracies due to various ways in which modern
computer hardware "warms up" and gets faster after doing the same task several
times, or by doing active CPU work in general.
