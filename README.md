# **System programming on Linux**

## `fork/`: multiprocessed application example
Simple application with kernel process that spawns four children to process strings.
Processes communicate via signals (`SUGUSR1`).
Noncanonical input and `<time.h>` are utilized for better CLI experience.
