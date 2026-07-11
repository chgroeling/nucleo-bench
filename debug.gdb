# GDB batch script: connect to OpenOCD, load firmware, run with semihosting exit detection.

set confirm off
target extended-remote :3333
load
monitor reset halt

break _exit_breakpoint
commands
    echo \n=== Program exited (breakpoint hit) ===\n
    monitor reset halt
    kill
    quit
end

echo === Check openocd terminal for output ===\n
continue
