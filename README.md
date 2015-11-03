# timespiece

Simple watchdog timer.

## Usage
```
// 3000 ms, repeatedly, dispatch async
timespiece::watchdog t(3 * 1000, true, true, [] {
	// do something
}, [] {
	// completion handler is called when finished
});

// start
t.resume();

// stop repeating
t.invalidate();
```
