package com.example.lxproject;

public class NativeInterface {


    public int use_sticker(String folder, String name) {
        return _native_use_sticker(folder, name);
    }

    private native int _native_use_sticker(String folder, String name);
}
