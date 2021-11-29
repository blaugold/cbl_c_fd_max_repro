This demo demonstrates the behavior of Couchbase Lite C when attempting to
connect to a Sync Gateway, while the process has various numbers of file
descriptors open. Once a process uses more than 1024 file descriptors, behavior
of Couchbase Lite C changes.

# Linux

## Setup

In the `/linux` directory, do the following:

1. Unpack the Couchbase Lite package for the host platform into the `vendor`
   directory.
2. Run `mkdir build && cd build && cmake .. && cmake --build .`
3. Make sure **no** Sync Gateway is running at `localhost:5000`.
4. Run `./build/main <fd-count>` where `fdCount` is the number of file
   descriptors to open before attempting to replicate with a Sync Gateway.

## Observed behavior on Ubuntu 20.04:

When running the demo with a low number (~100) of file descriptors, the demo
will just fail to connect to the sync gateway with POSIX error 111, "Connection
refused".

When running the demo with a high number (1000+) of file descriptors, the demo
will fail to connect with the wrong error: POSIX error 110, "Connection timed
out".

# Android

In the `/android` directory, do the following:

1. Unpack the Couchbase Lite package for android into the `vendor` directory.
   The expected name of the package directory is `libcblite-3.0.0`.
2. Open the project with Android Studio.
3. Run the App until it crashes with the following abort message:
   ```
   Abort message: 'FORTIFY: FD_SET: file descriptor 1076 >= FD_SETSIZE 1024'
   ```
4. Run `adb logcat -d > logcat.txt` to get the logcat output.
5. Use `nkd-stack` to symbolicate the stack trace:
   ```
   ndk-stack -sym vendor/libcblite-3.0.0/lib/$ANDROID_ABI$/ -i logcat.txt
   ```

`/data` contains an example `logcat.txt` file and the corresponding
`stack-trace.txt`.

## Relevant Files

- `CmakeLists.txt`: to build `native-lib` library and package CBL C with the
  app.
- `app/src/main/cpp/native-lib.cc`: Native shared library which calls CBL C.
- `app/src/main/java/com/example/cbl_c_fd_max_demo/NativeLib.java`: Java code
  which calls `native-lib`
- `app/src/main/java/com/example/cbl_c_fd_max_demo/MainActivity.kt`: `runDemo`
  method launches demo code.
