# Operating-Systems
Implementation of a process manager which supports the following commands
+ **bg** "filename" "arguments" (forks a new process and runs the provided file with any supplied arguments)
+ **bglist** (lists any running processes)
+ **bgkill** "pid" (kills the process with pid)
+ **bgstop** "pid" (stops the process with pid)
+ **bgstart** "pid" (starts the process with pid, that has been previously stopped)
+ **pstat** "pid" (displays various information of process with pid, including state, utime, stime, rss .... etc)

* C ompiling: Run the provided Make file -> rsi exectubale file is created
* Running: Run rsi
