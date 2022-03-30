package com.hsj.camera;

import android.util.Log;
import java.lang.reflect.Method;

/**
 * @Author:Hsj
 * @Date:2022-01-12
 * @Class:Logger
 * @Desc:
 */
final class Logger {

    //////////////////////////////////////////////////////////////
    // Logger 功能如下：
    //      1、增加控制台日志打印长度
    //      2、开关控制日志是否打印
    //      3、可添加自定义tag，默认：[Logger]
    //////////////////////////////////////////////////////////////

    private static String TAG = "[Logger]";

    private static final int MAX_LENGTH = 5000;

    public static boolean OPEN = BuildConfig.DEBUG;

    private Logger() {

    }

    public static void e(Object msg) {
        e(TAG, msg);
    }

    public static void e(String tag, Object msg) {
        e(tag, msg, null);
    }

    public static void e(String tag, Object msg, Throwable tr) {
        log(tag, msg.toString(), tr, 'e');
    }

    public static void w(Object msg) {
        w(TAG, msg);
    }

    public static void w(String tag, Object msg) {
        w(tag, msg, null);
    }

    public static void w(String tag, Object msg, Throwable tr) {
        log(tag, msg.toString(), tr, 'w');
    }

    public static void d(Object msg) {
        d(TAG, msg);
    }

    public static void d(String tag, Object msg) {
        d(tag, msg, null);
    }

    public static void d(String tag, Object msg, Throwable tr) {
        log(tag, msg.toString(), tr, 'd');
    }

    public static void i(Object msg) {
        i(TAG, msg);
    }

    public static void i(String tag, Object msg) {
        i(tag, msg, null);
    }

    public static void i(String tag, Object msg, Throwable tr) {
        log(tag, msg.toString(), tr, 'i');
    }

    public static void v(Object msg) {
        v(TAG, msg);
    }

    public static void v(String tag, Object msg) {
        v(tag, msg, null);
    }

    public static void v(String tag, Object msg, Throwable tr) {
        log(tag, msg.toString(), tr, 'v');
    }

    private static void log(String tag, String msg, Throwable tr, char level) {
        if ('e' == level) {
            print("e", tag, msg, tr);
        } else if ('w' == level) {
            print("w", tag, msg, tr);
        } else if (OPEN) {
            if ('d' == level) {
                print("d", tag, msg);
            } else if ('i' == level) {
                print("i", tag, msg);
            } else {
                print("v", tag, msg);
            }
        }
    }

    private static void print(String method, String tag, String message) {
        int strLength = message.length();
        if (strLength == 0) {
            invokePrint(method, tag, message);
        } else {
            for (int i = 0; i < strLength / MAX_LENGTH + (strLength % MAX_LENGTH > 0 ? 1 : 0); i++) {
                int end = (i + 1) * MAX_LENGTH;
                if (strLength >= end) {
                    invokePrint(method, tag, message.substring(end - MAX_LENGTH, end));
                } else {
                    invokePrint(method, tag, message.substring(end - MAX_LENGTH));
                }
            }
        }
    }

    private static void print(String method, String tag, String message, Throwable e) {
        invokePrint(method, tag, message, e);
    }

    private static void invokePrint(String method, String tag, String message) {
        try {
            Class<Log> logClass = Log.class;
            Method logMethod = logClass.getMethod(method, String.class, String.class);
            logMethod.setAccessible(true);
            logMethod.invoke(null, tag, message);
        } catch (Exception e) {
            System.out.println(tag + ": " + message);
        }
    }

    private static void invokePrint(String method, String tag, String message, Throwable e) {
        try {
            Class<Log> logClass = Log.class;
            Method logMethod = logClass.getMethod(method, String.class, String.class, Throwable.class);
            logMethod.setAccessible(true);
            logMethod.invoke(null, tag, message, e);
        } catch (Exception e1) {
            System.out.println(tag + ": " + message);
        }
    }

}