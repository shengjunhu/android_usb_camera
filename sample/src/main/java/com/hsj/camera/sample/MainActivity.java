package com.hsj.camera.sample;

import android.Manifest;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.FrameLayout;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import com.hsj.camera.UsbCamera;

import java.util.ArrayList;
import java.util.List;

/**
 * @Author:Hsj
 * @Date:2020-06-22
 * @Class:MainActivity
 * @Desc:Sample of UVCCamera
 */
public final class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";
    private static final int PERMISSION_CODE = 0x0A;
    private static final String[] PERMISSIONS = {Manifest.permission.CAMERA};
    private final String ACTION_USB = "com.hsj.camera.sample.USB_PERMISSION." + hashCode();
    private static final boolean IS_ROOT_DEVICE = false;

    private UsbCamera camera;
    private UsbReceiver usbReceiver;
    private FrameLayout contentView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        contentView = findViewById(R.id.frameLayout);
        //Request Camera Permission.
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (ActivityCompat.checkSelfPermission(this, PERMISSIONS[0]) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, PERMISSIONS, PERMISSION_CODE);
            }
        }
        //Init UsbCamera
        this.camera = new UsbCamera();
        //Register
        IntentFilter filter = new IntentFilter(ACTION_USB);
        registerReceiver(usbReceiver = new UsbReceiver(), filter);
    }

    @Override
    protected void onStop() {
        super.onStop();
        close();
    }

    @Override
    protected void onDestroy() {
        if (usbReceiver != null) {
            unregisterReceiver(usbReceiver);
        }
        super.onDestroy();
        release();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()){
            case R.id.item_device:
                showDeviceDialog();
                break;
            case R.id.item_support:
                showDeviceSupportDialog();
                break;
            case R.id.item_mirror:
                showPreviewMirrorDialog();
                break;
            case R.id.item_orientation:
                showPreviewOrientationDialog();
                break;
            default:break;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == PERMISSION_CODE) {
            if (grantResults.length == PERMISSIONS.length) {
                if (grantResults[0] != PackageManager.PERMISSION_GRANTED) {
                    showToast("Camera permission had been denied.");
                }
            }
        }
    }

//==================================================================================================

    private int index_device;
    private int index_support;
    private int index_orientation;
    private int index_mirror;

    private void showDeviceDialog() {
        final UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        final UsbDevice[] devices = usbManager.getDeviceList().values().toArray(new UsbDevice[]{});
        int size = devices.length;
        if (size > 0) {
            String[] items = new String[size];
            for (int i = 0; i < size; ++i) {
                items[i] = devices[i].getProductName() + "";
            }
            if (index_device >= size) index_device = 0;
            AlertDialog.Builder ad = new AlertDialog.Builder(this);
            ad.setTitle(R.string.txt_select_device);
            ad.setSingleChoiceItems(items, index_device, (dialog, which) -> index_device = which);
            ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> checkDevice(usbManager, devices[index_device]));
            ad.show();
        } else {
            showToast("No Usb device to be found.");
        }
    }

    private void showDeviceSupportDialog() {
        int size = supportInfos.size();
        if (size > 0) {
            String[] items = new String[size];
            for (int i = 0; i < size; ++i) {
                items[i] = supportInfos.get(i).getInfo();
            }
            if (index_support >= size) index_support = 0;
            AlertDialog.Builder ad = new AlertDialog.Builder(this);
            ad.setTitle(R.string.txt_select_support);
            ad.setSingleChoiceItems(items, index_support, (dialog, which) -> index_support = which);
            ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> {});
            ad.show();
        } else {
            showToast("Please select target device at first.");
        }
    }

    private void showPreviewMirrorDialog() {
        String[] items = new String[]{"Default", "Mirror"};
        if (index_mirror >= items.length) index_mirror = 0;
        AlertDialog.Builder ad = new AlertDialog.Builder(this);
        ad.setTitle(R.string.txt_select_mirror);
        ad.setSingleChoiceItems(items, index_mirror, (dialog, which) -> index_mirror = which);
        ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> {});
        ad.show();
    }

    private void showPreviewOrientationDialog() {
        String[] items = new String[]{"Default", "90°", "180°", "270°"};
        if (index_orientation >= items.length) index_orientation = 0;
        AlertDialog.Builder ad = new AlertDialog.Builder(this);
        ad.setTitle(R.string.txt_select_orientation);
        ad.setSingleChoiceItems(items, index_orientation, (dialog, which) -> index_orientation = which);
        ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> {});
        ad.show();
    }

    private void showToast(String msg) {
        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
    }

//==================================================================================================

    private void checkDevice(UsbManager usbManager, UsbDevice device) {
        if (usbManager.hasPermission(device)) {
            open(usbManager, device);
        } else {
            Intent intent = new Intent(ACTION_USB);
            PendingIntent pi = PendingIntent.getBroadcast(this, 0, intent, 0);
            usbManager.requestPermission(device, pi);
        }
    }

    private final class UsbReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (ACTION_USB.equals(intent.getAction())) {
                UsbDevice device = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                if (device != null && intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                    open((UsbManager) getSystemService(Context.USB_SERVICE), device);
                } else {
                    showToast("Usb Permission had been denied.");
                }
            }
        }
    }

//===========================================Camera=================================================

    private List<UsbCamera.SupportInfo> supportInfos = new ArrayList<>();
    private UsbCamera.SupportInfo supportInfo;
    private UsbDeviceConnection udc;

    private void open(UsbManager usbManager, UsbDevice device) {
        close();
        int status;
        if (IS_ROOT_DEVICE) {
            status = camera.open(device.getVendorId(), device.getProductId(), device.getDeviceName());
        } else {
            UsbDeviceConnection connection = usbManager.openDevice(device);
            if (connection != null) {
                this.udc = connection;
                status = camera.open(connection.getFileDescriptor());
            } else {
                status = UsbCamera.STATUS_ERROR;
                showToast("openDevice failed.");
            }
        }
        if (UsbCamera.STATUS_OK == status) {
            supportInfos.clear();
            status = camera.getSupportInfo(supportInfos);
            if (UsbCamera.STATUS_OK != status || supportInfos.isEmpty()) {
                showToast("getSupportInfo failed: " + status);
            } else {
                if (supportInfo == null) supportInfo = supportInfos.get(0);
                status = camera.setSupportInfo(supportInfo);
                if (UsbCamera.STATUS_OK == status) {
                    status = camera.start();
                    if (UsbCamera.STATUS_OK != status){
                        showToast("start failed: " + status);
                    }
                } else {
                    showToast("setSupportInfo failed: " + status);
                }
            }
        } else {
            showToast("open failed: " + status);
        }
    }

    private void close() {
        if (this.camera != null) {
            this.camera.stop();
            this.camera.close();
        }
        if (this.udc != null) {
            this.udc.close();
            this.udc = null;
        }
    }

    private void release() {
        if (this.camera != null) {
            this.camera.destroy();
            this.camera = null;
        }
    }

//==================================================================================================

}


