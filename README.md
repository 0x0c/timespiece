# timespiece

Simple watchdog timer.

## Usage
```
// 3000 ms, repeatedly, dispatch async
timespiece::watchdog w;

// start
w.resume(3 * 1000, true, true, [] (int repeated_count, timespiece::timer *t) {
	// do something
}, [] {
	// completion handler is called when finished
});

// invalidate
w.invalidate();
```

## TODO
- [ ] dispatch function in main thread.
