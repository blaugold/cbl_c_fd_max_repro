package com.example.cbl_c_fd_max_demo;

public class NativeLib {
    static {
        System.loadLibrary("native-lib");
    }

    static native void initCbl(String filesDir);

    static native void runDemo(int fdCount);
}
