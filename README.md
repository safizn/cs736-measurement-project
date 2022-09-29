# cs736-measurement-project

## Testing - Performance Measures: 
- Latency
- Throughput

## Implementation - IPC mechanisms:
- Unix pipe communication (https://elixir.bootlin.com/linux/latest/source/fs/pipe.c#L1387)
  - http://web.cse.ohio-state.edu/~mamrak.1/CIS762/pipes_lab_notes.html
  - How much data the kernel pipe can buffer/handle: $"dd if=/dev/zero bs=1024k count=10240 2>/dev/null | (sleep 10; wc -c; )"
  - Modern internal pipe buffer size ranges from 16k to 64k. 
  - Linux let's you increase the size of the pipe buffer via the F_SETPIPE_SZ fcntl(2) (as a multiple of the page size and up to the max size defined in /proc/sys/fs/pipe-max-size for normal users).  pipe-max-size = 2^20 = 1048576
  - TESTED: max pipe write size = 2^16 = 65536 = 64K
- Socket (TCP/IP)
  - https://inet.omnetpp.org/docs/developers-guide/ch-sockets.html
  - https://www.softprayog.in/programming/network-socket-programming-using-tcp-in-c
  - https://www.youtube.com/watch?v=WDn-htpBlnU & https://www.youtube.com/watch?v=fmn-pRvNaho
  - https://www.linuxhowtos.org/C_C++/socket.htm
  - https://biendltb.github.io/tech/inter-process-communication-ipc-in-cpp/
  - https://man7.org/linux/man-pages/man7/unix.7.html
  - Check Unix opened sockets "$ ss -s" 
- Shared mem channel
  - https://www.youtube.com/watch?v=Yb6pc_OU5x8
  - https://www.youtube.com/watch?v=dJuYKfR8vec
  - 

# Resources: 
- https://medium.com/swlh/reliable-performance-testing-in-c-1df7a3ba398
- https://mazzo.li/posts/fast-pipes.html
- https://brandonwamboldt.ca/how-linux-pipes-work-under-the-hood-1518/
- https://stackoverflow.com/questions/11563963/how-to-write-a-large-buffer-into-a-binary-file-in-c-fast
- https://www.netmeister.org/blog/ipcbufs.html
- https://biendltb.github.io/tech/inter-process-communication-ipc-in-cpp/


