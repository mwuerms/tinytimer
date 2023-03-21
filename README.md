# tinytimer

TinyTimer is a small electronic device (using Attiny404) on a pcb, which offers 4 simple timers and each a led to gently inform the user of an alarm.

## User interface

The led shows the status of the timer
+ off (LED off)
+ running (gently fade in 1 - 2 s)
+ pause (double blink shortly in 1 - 2 s)
+ alarm (blink wildly for 20 s)

With the button, the user can control the timer
+ start
+ pause/resume
+ stop/abort

Each timer has a fixed timeout, eg.
1. 3 min (tea)
2. 5 min
3. 10 min
4. 25 min

Each timer can run independently from all others.
