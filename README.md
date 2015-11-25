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
	// completion handler will be called when finished
});

// invalidate
w.invalidate();
```

## TODO
- [x] dispatch function in main thread. → Use parallel_linse.(https://github.com/0x0c/parallel_lines)
