package com.example.lxproject;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        testjni();
    }

    private void testjni() {
        NativeInterface nCall = new NativeInterface();
        nCall.use_sticker("a", "b");
    }

    static {
        System.loadLibrary("longxiang");
    }
}
