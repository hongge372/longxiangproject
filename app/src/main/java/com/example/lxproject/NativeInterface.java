package com.example.lxproject;

public class NativeInterface {

    public void callSayHello(){
         sayHello();
    }
    private native void sayHello();
}
