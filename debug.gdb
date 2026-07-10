set confirm off
target extended-remote :4242
load
monitor reset

break _exit_breakpoint
commands
    echo \n=== Program exited (breakpoint hit) ===\n
    monitor reset
    kill
    quit
end

echo === Check st-util terminal for output ===\n
continue
