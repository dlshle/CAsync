fuck fuck fuck fuck fuck
Queue safe lock error??????

OK, so the join works as expected, but we gotta figure out how to wait for the current running tasks(wait till current task procedures are done).

I was thinking about using a semaphore to do the blocking and use a variable to represent # running tasks. All we gotta do is wait the sem when # running tasks > 0 and post sem when # running tasks = 0.

For wait all, we can use queue size and a global sem to do the same thing.
