# timespiece

Simple watchdog timer.

## Usage
```
// 3000 ms, repeatedly, dispatch async
timespiece::watchdog t;

// start
t.resume(3 * 1000, true, true, [] {
	// do something
}, [] {
	// completion handler is called when finished
});

// invalidate
t.invalidate();
```

## TODO
- [ ] dispatch function in main thread.
