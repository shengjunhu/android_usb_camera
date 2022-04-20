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
import android.hardware.usb.UsbManager;
import android.os.Build;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;
import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.ActionBarDrawerToggle;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.drawerlayout.widget.DrawerLayout;
import androidx.fragment.app.FragmentManager;
import com.google.android.material.navigation.NavigationView;
import com.google.android.material.snackbar.Snackbar;
import com.hsj.camera.IFrameCallback;
import com.hsj.camera.UsbCamera;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;

/**
 * @Author:Hsj
 * @Date:2020-06-22
 * @Class:MainActivity
 * @Desc:Sample of UVCCamera
 */
public final class MainActivity extends AppCompatActivity implements View.OnClickListener, DrawerLayout.DrawerListener {

    private static final String TAG = "MainActivity";
    private static final int PERMISSION_CODE = 0x0A;
    private static final boolean IS_ROOT_DEVICE = false;
    private static final String[] PERMISSIONS = {Manifest.permission.CAMERA};
    private final String ACTION_USB = "com.hsj.camera.sample.USB_PERMISSION." + hashCode();

    private DeviceInfo deviceInfo;
    private UsbReceiver usbReceiver;
    private PreviewFragment fragment;
    private DrawerLayout drawerLayout;
    private TextView tv_decoder, tv_render;
    private TextView tv_mirror, tv_orientation;
    private TextView tv_camera, tv_format, tv_size;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initUI();
        initData();
    }

    private void initUI() {
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) actionBar.setDisplayHomeAsUpEnabled(true);
        drawerLayout = findViewById(R.id.drawer_layout);
        NavigationView navigationView = findViewById(R.id.navigation_view);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(this, drawerLayout,
                R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawerLayout.addDrawerListener(toggle);
        toggle.syncState();

        //AppCompatSpinner
        View headerView = navigationView.getHeaderView(0);
        tv_camera = headerView.findViewById(R.id.tv_camera);
        tv_format = headerView.findViewById(R.id.tv_format);
        tv_size = headerView.findViewById(R.id.tv_size);
        tv_decoder = headerView.findViewById(R.id.tv_decoder);
        tv_render = headerView.findViewById(R.id.tv_render);
        tv_mirror = headerView.findViewById(R.id.tv_mirror);
        tv_orientation = headerView.findViewById(R.id.tv_orientation);
        tv_camera.setOnClickListener(this);
        tv_format.setOnClickListener(this);
        tv_size.setOnClickListener(this);
        tv_decoder.setOnClickListener(this);
        tv_render.setOnClickListener(this);
        tv_mirror.setOnClickListener(this);
        tv_orientation.setOnClickListener(this);
    }

    private void initData() {
        deviceInfo = new DeviceInfo();
        drawerLayout.addDrawerListener(this);

        //Request Camera Permission.
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (ActivityCompat.checkSelfPermission(this, PERMISSIONS[0]) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, PERMISSIONS, PERMISSION_CODE);
            }
        }

        //Register
        IntentFilter filter = new IntentFilter(ACTION_USB);
        registerReceiver(usbReceiver = new UsbReceiver(), filter);

        //Fragment
        FragmentManager fragmentManager = getSupportFragmentManager();
        fragment = (PreviewFragment) fragmentManager.findFragmentById(R.id.fragment);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_camera:
                searchDevices(v);
                break;
            case R.id.tv_format:
                searchSupportFormats(v);
                break;
            case R.id.tv_size:
                configSupportSize(v);
                break;
            case R.id.tv_decoder:
                configSupportDecoder();
                break;
            case R.id.tv_render:
                configSupportRender();
                break;
            case R.id.tv_mirror:
                configPreviewMirror();
                break;
            case R.id.tv_orientation:
                configPreviewOrientation();
                break;
            default:
                break;
        }
    }

    @Override
    protected void onDestroy() {
        if (usbReceiver != null) {
            unregisterReceiver(usbReceiver);
        }
        super.onDestroy();
        Log.d(TAG, "onDestroy");
        System.exit(0);
    }

    @Override
    public void onBackPressed() {
        if (drawerLayout.isOpen()) {
            drawerLayout.close();
        } else {
            super.onBackPressed();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()) {
            case R.id.item_device:
            case android.R.id.home:
                if (drawerLayout.isOpen()) {
                    drawerLayout.close();
                } else {
                    drawerLayout.open();
                }
                break;
            default:
                break;
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

    @Override
    public void onDrawerSlide(@NonNull View drawerView, float slideOffset) {

    }

    @Override
    public void onDrawerOpened(@NonNull View drawerView) {
        fragment.stop();
    }

    @Override
    public void onDrawerClosed(@NonNull View drawerView) {
        fragment.start(deviceInfo);
    }

    @Override
    public void onDrawerStateChanged(int newState) {

    }

//==================================================================================================

    private int index_camera;
    private int index_format;
    private int index_size;
    private int index_decoder;
    private int index_render;
    private int index_orientation;
    private int index_mirror;

    private void searchDevices(View view) {
        final UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        final UsbDevice[] devices = usbManager.getDeviceList().values().toArray(new UsbDevice[]{});
        final int size = devices.length;
        if (size > 0) {
            String[] items = new String[size];
            for (int i = 0; i < size; ++i) {
                String pn = devices[i].getProductName();
                items[i] = TextUtils.isEmpty(pn) ? "Unknown" : pn;
            }
            if (index_camera >= size) index_camera = 0;
            AlertDialog.Builder ad = new AlertDialog.Builder(this);
            ad.setTitle(R.string.txt_select_camera);
            ad.setSingleChoiceItems(items, index_camera, (dialog, which) -> index_camera = which);
            ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> {
                index_size = 0;
                index_format = 0;
                tv_size.setText("");
                tv_format.setText("");
                tv_camera.setText(items[index_camera]);
                checkDevice(usbManager, devices[index_camera]);
            });
            ad.show();
        } else {
            showBar(view, "Usb camera is empty.");
        }
    }

    private void searchSupportFormats(View view) {
        List<UsbCamera.SupportInfo> supports = fragment.getSupport();
        final int size = supports.size();
        if (size > 0) {
            HashMap<Integer, String> map = new HashMap<>();
            for (UsbCamera.SupportInfo info : supports) {
                if (!map.containsKey(info.getFormat())) {
                    map.put(info.getFormat(), info.getFormatStr());
                }
            }
            final Integer[] keys = map.keySet().toArray(new Integer[]{});
            final String[] values = map.values().toArray(new String[]{});
            if (index_format >= map.size()) index_format = 0;
            AlertDialog.Builder ad = new AlertDialog.Builder(this);
            ad.setTitle(R.string.txt_config_format);
            ad.setSingleChoiceItems(values, index_format, (dialog, which) -> index_format = which);
            ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> {
                index_size = 0;
                tv_size.setText("");
                tv_format.setText(values[index_format]);
                deviceInfo.setFormat(keys[index_format]);
            });
            ad.show();
        } else {
            showBar(view, "Support formats is empty.");
        }
    }

    private void configSupportSize(View view) {
        List<UsbCamera.SupportInfo> supports = fragment.getSupport();
        final int size = supports.size();
        if (deviceInfo.getFormat() == 0) {
            showBar(view, "Select support format at first.");
        } else if (size == 0) {
            showBar(view, "Support size is empty.");
        } else {
            HashMap<String, UsbCamera.SupportInfo> map = new HashMap<>();
            for (UsbCamera.SupportInfo info : supports) {
                if (deviceInfo.getFormat() == info.getFormat()) {
                    String key = String.format(Locale.CHINA, "%dx%d %dfps",
                            info.getWidth(), info.getHeight(), info.getFps());
                    map.put(key, info);
                }
            }
            final String[] keys = map.keySet().toArray(new String[]{});
            final UsbCamera.SupportInfo[] values = map.values().toArray(new UsbCamera.SupportInfo[]{});
            if (index_size >= map.size()) index_size = 0;
            AlertDialog.Builder ad = new AlertDialog.Builder(this);
            ad.setTitle(R.string.txt_config_size);
            ad.setSingleChoiceItems(keys, index_size, (dialog, which) -> index_size = which);
            ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> {
                tv_size.setText(keys[index_size]);
                deviceInfo.setSupportInfo(values[index_size]);
            });
            ad.show();
        }
    }

    private void configSupportDecoder() {
        String[] items = {"Default", "Software", "Hardware"};
        if (index_decoder >= 3) index_decoder = 0;
        AlertDialog.Builder ad = new AlertDialog.Builder(this);
        ad.setTitle(R.string.txt_config_decoder);
        ad.setSingleChoiceItems(items, index_decoder, (dialog, which) -> index_decoder = which);
        ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> {
            tv_decoder.setText(items[index_decoder]);
            deviceInfo.setDecoder(index_decoder);
        });
        ad.show();
    }

    private void configSupportRender() {
        String[] items = {"Default", "Native", "OpenGL"};
        if (index_render >= 3) index_render = 0;
        AlertDialog.Builder ad = new AlertDialog.Builder(this);
        ad.setTitle(R.string.txt_config_render);
        ad.setSingleChoiceItems(items, index_render, (dialog, which) -> index_render = which);
        ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> {
            tv_render.setText(items[index_render]);
            deviceInfo.setRender(index_render);
        });
        ad.show();
    }

    private void configPreviewOrientation() {
        String[] items = {"Default", "90", "180", "270"};
        if (index_orientation >= 4) index_orientation = 0;
        AlertDialog.Builder ad = new AlertDialog.Builder(this);
        ad.setTitle(R.string.txt_config_orientation);
        ad.setSingleChoiceItems(items, index_orientation, (dialog, which) -> index_orientation = which);
        ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> {
            tv_orientation.setText(items[index_orientation]);
            deviceInfo.setRotate(index_orientation);
        });
        ad.show();
    }

    private void configPreviewMirror() {
        String[] items = {"Default", "Mirror-H", "Mirror-V"};
        if (index_mirror >= 3) index_mirror = 0;
        AlertDialog.Builder ad = new AlertDialog.Builder(this);
        ad.setTitle(R.string.txt_config_mirror);
        ad.setSingleChoiceItems(items, index_mirror, (dialog, which) -> index_mirror = which);
        ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> {
            tv_mirror.setText(items[index_mirror]);
            deviceInfo.setMirror(index_mirror);
        });
        ad.show();
    }

    private void showBar(View view, String msg) {
        Snackbar.make(view, msg, Snackbar.LENGTH_LONG)
                .setAction("Action", null).show();
    }

    private void showToast(String msg) {
        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
    }

//==================================================================================================

    private UsbDeviceConnection udc;

    private void checkDevice(UsbManager usbManager, UsbDevice device) {
        fragment.close();
        deviceInfo.setFd(0);
        deviceInfo.setFormat(0);
        deviceInfo.setSupportInfo(null);
        deviceInfo.setDevice(0, 0, null);
        if (this.udc != null) {
            this.udc.close();
            this.udc = null;
        }
        if (usbManager.hasPermission(device)) {
            openDevice(usbManager, device);
        } else {
            Intent intent = new Intent(ACTION_USB);
            PendingIntent pi = PendingIntent.getBroadcast(this, 0, intent, 0);
            usbManager.requestPermission(device, pi);
        }
    }

    private void openDevice(UsbManager usbManager, UsbDevice device) {
        if (IS_ROOT_DEVICE) {
            this.deviceInfo.setDevice(device.getVendorId(), device.getProductId(), device.getDeviceName());
        } else {
            UsbDeviceConnection connection = usbManager.openDevice(device);
            if (connection != null) {
                this.udc = connection;
                this.deviceInfo.setFd(connection.getFileDescriptor());
            } else {
                showToast("openDevice failed.");
            }
        }
        this.fragment.open(this.deviceInfo);
    }

    private final class UsbReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (ACTION_USB.equals(intent.getAction())) {
                UsbDevice device = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                if (device != null && intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                    openDevice((UsbManager) getSystemService(Context.USB_SERVICE), device);
                } else {
                    showToast("Usb Permission had been denied.");
                }
            }
        }
    }

}


