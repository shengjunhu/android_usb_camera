package com.hsj.sample;

import android.Manifest;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.TextView;
import android.widget.Toast;
import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.ActionBarDrawerToggle;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.drawerlayout.widget.DrawerLayout;
import com.google.android.material.navigation.NavigationView;
import com.google.android.material.snackbar.Snackbar;
import com.hsj.camera.UsbCamera;
import com.hsj.camera.CameraView;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;

/**
 * @Author:Hsj
 * @Date:2020-06-22
 * @Class:MainActivity
 * @Desc:
 */
public final class MainActivity extends AppCompatActivity implements View.OnClickListener, DrawerLayout.DrawerListener {

    private static final String TAG = "MainActivity";
    private static final int PERMISSION_CODE = 0x0A;
    private static final boolean IS_ROOT_DEVICE = true;
    private static final String[] PERMISSIONS = {Manifest.permission.CAMERA};
    private final String ACTION_USB = "com.hsj.sample.USB_PERMISSION." + hashCode();

    private Handler mainHandler;
    private DeviceInfo deviceInfo;
    private UsbReceiver usbReceiver;
    private CameraView cameraView;
    private DrawerLayout drawerLayout;
    private TextView tv_decoder, tv_render;
    private TextView tv_mirror, tv_orientation;
    private TextView tv_camera, tv_format, tv_size;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.i(TAG, "onCreate");
        initUI();
        initData();
    }

    private void initUI() {
        //DrawerLayout
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) actionBar.setDisplayHomeAsUpEnabled(true);
        drawerLayout = findViewById(R.id.drawer_layout);
        NavigationView navigationView = findViewById(R.id.navigation_view);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(this,
                drawerLayout, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawerLayout.addDrawerListener(toggle);
        toggle.syncState();

        //cameraView
        cameraView = findViewById(R.id.camera_view);
        cameraView.setOnStatusListener(listener);
        //headerView
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
        camera = new UsbCamera();
        camera.setPreview(cameraView);
        deviceInfo = new DeviceInfo();
        drawerLayout.addDrawerListener(this);
        mainHandler = new Handler(Looper.getMainLooper());

        //Screen Param
        deviceInfo.setScreenOrientation(getRequestedOrientation());
        cameraView.post(()->deviceInfo.setMaxContentWidthHeight(
                cameraView.getWidth(),cameraView.getHeight()));

        //Request Camera Permission.
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (ActivityCompat.checkSelfPermission(this, PERMISSIONS[0]) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, PERMISSIONS, PERMISSION_CODE);
            }
        }

        //Register
        IntentFilter filter = new IntentFilter(ACTION_USB);
        registerReceiver(usbReceiver = new UsbReceiver(), filter);
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
    protected void onStart() {
        super.onStart();
        Log.i(TAG, "onStart");
    }

    @Override
    protected void onRestart() {
        super.onRestart();
        Log.i(TAG, "onRestart");
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.i(TAG, "onResume");
        if (cameraView != null) cameraView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.i(TAG, "onPause");
        if (cameraView != null) cameraView.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.i(TAG, "onStop");
        close();
    }

    @Override
    protected void onDestroy() {
        destroy();
        super.onDestroy();
        Log.i(TAG, "onDestroy");
        System.exit(0);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (drawerLayout.isOpen()) {
            drawerLayout.close();
            return true;
        }else {
            destroy();
            return super.onKeyDown(keyCode, event);
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
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        if (deviceInfo.getScreenOrientation() != newConfig.orientation) {
            //Reset mirror of config
            index_mirror = 0;
            deviceInfo.setMirror(index_mirror);
            tv_mirror.setText(ITEM_MIRROR[index_mirror]);
            //Reset orientation of config
            index_orientation = 0;
            deviceInfo.setScreenOrientation(index_orientation);
            tv_orientation.setText(ITEM_ROTATE[index_orientation]);
            //Update screenOrientation and contentSize
            deviceInfo.setScreenOrientation(newConfig.orientation);
            //Get contentView width and height need to delay
            cameraView.post(()->deviceInfo.setMaxContentWidthHeight(cameraView.getWidth(), cameraView.getHeight()));
            //Preview
            mainHandler.postDelayed(()->{stop(); start(deviceInfo);},10);
        }
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

//====================================DrawerLayout==================================================

    @Override
    public void onDrawerSlide(@NonNull View drawerView, float slideOffset) {

    }

    @Override
    public void onDrawerOpened(@NonNull View drawerView) {
        stop();
    }

    @Override
    public void onDrawerClosed(@NonNull View drawerView) {
        start(deviceInfo);
    }

    @Override
    public void onDrawerStateChanged(int newState) {

    }
    
//========================================Configure=================================================

    private int index_camera;
    private int index_format;
    private int index_size;
    private int index_decoder;
    private int index_render;
    private int index_orientation;
    private int index_mirror;
    private static final String[] ITEM_DEFAULT = {"Default"};
    private static final String[] ITEM_RENDER  = {"Native", "OpenGL"};
    private static final String[] ITEM_DECODE  = {"Software", "Hardware"};
    private static final String[] ITEM_ROTATE  = {"Default", "90°", "180°", "270°"};
    private static final String[] ITEM_MIRROR  = {"Default", "Mirror-H", "Mirror-V"};

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
            showBar(view, getString(R.string.txt_not_found_device));
        }
    }

    private void searchSupportFormats(View view) {
        final int size = supportInfos.size();
        if (size > 0) {
            HashMap<Integer, String> map = new HashMap<>();
            for (UsbCamera.SupportInfo info : supportInfos) {
                if (!map.containsKey(info.format)) {
                    map.put(info.format, "Unknown");
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
            showBar(view, getString(R.string.txt_not_found_format));
        }
    }

    private void configSupportSize(View view) {
        final int size = supportInfos.size();
        if (deviceInfo.getFormat() == 0) {
            showBar(view, getString(R.string.txt_hint_config_format));
        } else if (size == 0) {
            showBar(view, getString(R.string.txt_not_found_format));
        } else {
            HashMap<String, UsbCamera.SupportInfo> map = new HashMap<>();
            for (UsbCamera.SupportInfo info : supportInfos) {
                if (deviceInfo.getFormat() == info.format) {
                    String key = String.format(Locale.CHINA, "%dx%d %dfps",
                            info.width, info.height, info.fps);
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
        if (index_decoder >= ITEM_DECODE.length) index_decoder = 0;
        AlertDialog.Builder ad = new AlertDialog.Builder(this);
        ad.setTitle(R.string.txt_config_decoder);
        ad.setSingleChoiceItems(ITEM_DECODE, index_decoder, (dialog, which) -> index_decoder = which);
        ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> {
            tv_decoder.setText(ITEM_DECODE[index_decoder]);
            deviceInfo.setDecoder(index_decoder);
        });
        ad.show();
    }

    private void configSupportRender() {
        if (index_render >= ITEM_RENDER.length) index_render = 0;
        AlertDialog.Builder ad = new AlertDialog.Builder(this);
        ad.setTitle(R.string.txt_config_render);
        ad.setSingleChoiceItems(ITEM_RENDER, index_render, (dialog, which) -> index_render = which);
        ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> {
            deviceInfo.setRender(index_render);
            tv_render.setText(ITEM_RENDER[index_render]);
            if (index_render == 0) {
                index_mirror = 0;
                index_orientation = 0;
                deviceInfo.setMirror(index_mirror);
                deviceInfo.setRotate(index_orientation);
                tv_mirror.setText("");
                tv_orientation.setText("");
            }
        });
        ad.show();
    }

    private void configPreviewOrientation() {
        AlertDialog.Builder ad = new AlertDialog.Builder(this);
        ad.setTitle(R.string.txt_config_orientation);
        if (deviceInfo.getRender() == 0){
            index_orientation = 0;
            deviceInfo.setRotate(index_orientation);
            ad.setSingleChoiceItems(ITEM_DEFAULT, index_orientation, (dialog, which) -> index_orientation = which);
            ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> tv_orientation.setText(ITEM_DEFAULT[index_orientation]));
        } else {
            if (index_orientation >= ITEM_ROTATE.length) index_orientation = 0;
            ad.setSingleChoiceItems(ITEM_ROTATE, index_orientation, (dialog, which) -> index_orientation = which);
            ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> {
                deviceInfo.setRotate(index_orientation);
                tv_orientation.setText(ITEM_ROTATE[index_orientation]);
            });
        }
        ad.show();
    }

    private void configPreviewMirror() {
        AlertDialog.Builder ad = new AlertDialog.Builder(this);
        ad.setTitle(R.string.txt_config_mirror);
        if (deviceInfo.getRender() == 0){
            index_mirror = 0;
            deviceInfo.setMirror(index_mirror);
            ad.setSingleChoiceItems(ITEM_DEFAULT, index_mirror, (dialog, which) -> index_mirror = which);
            ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> tv_mirror.setText(ITEM_DEFAULT[index_mirror]));
        } else {
            if (index_mirror >= ITEM_MIRROR.length) index_mirror = 0;
            ad.setSingleChoiceItems(ITEM_MIRROR, index_mirror, (dialog, which) -> index_mirror = which);
            ad.setPositiveButton(R.string.txt_confirm, (dialog, which) -> {
                tv_mirror.setText(ITEM_MIRROR[index_mirror]);
                deviceInfo.setMirror(index_mirror);
            });
        }
        ad.show();
    }

    private void showBar(View view, String msg) {
        Snackbar.make(view, msg, Snackbar.LENGTH_LONG)
                .setAction("Action", null).show();
    }

    private void showToast(String msg) {
        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
    }

//=====================================Open Device==================================================

    private UsbDeviceConnection udc;

    private void checkDevice(UsbManager usbManager, UsbDevice device) {
        //Clear
        deviceInfo.setFd(0);
        deviceInfo.setFormat(0);
        deviceInfo.setSupportInfo(null);
        deviceInfo.setDevice(0, 0, null);
        //Close
        close();
        //Check Permission
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
                showToast(getString(R.string.txt_open_device_failed));
                return;
            }
        }
        open(this.deviceInfo);
    }

    private final class UsbReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (ACTION_USB.equals(intent.getAction())) {
                UsbDevice device = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                if (device != null && intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                    openDevice((UsbManager) getSystemService(Context.USB_SERVICE), device);
                } else {
                    showToast(getString(R.string.txt_usb_permission_denied));
                }
            }
        }
    }

//=======================================Camera API=================================================

    private List<UsbCamera.SupportInfo> supportInfos = new ArrayList<>();
    private static final float DECIMAL = 1.00000f;
    private UsbCamera camera;

    public void open(DeviceInfo info) {
        int status;
        if (info.getFd() != 0) {
            status = camera.open(info.getFd());
        } else {
            status = camera.open(info.getVid(), info.getPid(), info.getDeviceName());
        }
        if (UsbCamera.STATUS_SUCCESS == status) {
            supportInfos.clear();
            status = camera.getSupportInfo(supportInfos);
            if (UsbCamera.STATUS_SUCCESS != status) {
                showToast(getString(R.string.txt_get_support_failed) + status);
            }
        } else {
            showToast(getString(R.string.txt_open_camera_failed) + status);
        }
    }

    public void start(DeviceInfo info) {
        UsbCamera.SupportInfo support = info.getSupportInfo();
        if (support != null) {
            support.pixel = UsbCamera.PixelFormat.PIXEL_FORMAT_NV21.getValue();
            camera.setFrameCallback(frameCallback);
            int status = camera.setSupportInfo(support);
            if (UsbCamera.STATUS_SUCCESS == status) {
                createPreview(info);
            } else {
                showToast(getString(R.string.txt_set_support_failed));
            }
        }
    }

    private void createPreview(DeviceInfo info) {
        UsbCamera.SupportInfo support = info.getSupportInfo();
        int rotate = info.getRotate();
        final float frameRatio;
        if (rotate == 90 || rotate == 270){
            frameRatio = DECIMAL * support.height / support.width;
        } else {
            frameRatio = DECIMAL * support.width / support.height;
        }
        final float contentRatio = DECIMAL * info.getMaxContentWidth() / info.getMaxContentHeight();
        int[] size = new int[2];
        if (frameRatio > contentRatio){
            size[0] = info.getMaxContentWidth();
            size[1] = (int) (size[0] / frameRatio);
        } else {
            size[1] = info.getMaxContentHeight();
            size[0] = (int) (size[1] * frameRatio);
        }

        FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(size[0], size[1], Gravity.CENTER);
        cameraView.setViewType(info.getRender(), lp);
    }

    public void stop() {
        if (this.camera != null) {
            this.camera.stop();
        }
    }

    public void close() {
        if (this.camera != null) {
            this.supportInfos.clear();
            this.camera.close();
        }
        if (this.udc != null){
            this.udc.close();
            this.udc = null;
        }
    }

    private void destroy() {
        if (this.usbReceiver != null) {
            unregisterReceiver(usbReceiver);
            this.usbReceiver = null;
        }
        if (this.camera != null) {
            this.camera.destroy();
            this.camera = null;
        }
        if (this.cameraView != null){
            this.cameraView.destroy();
            this.cameraView = null;
        }
    }

    private final UsbCamera.IFrameCallback frameCallback = new UsbCamera.IFrameCallback() {
        @Override
        public void onFrame(ByteBuffer frame) {
            Log.i(TAG, "onFrame");
        }

        @Override
        public void onPhoto(ByteBuffer photo) {
            Log.i(TAG, "onPhoto");
        }
    };

//====================================UsbCameraView.OnStatusListener================================

    private final CameraView.OnStatusListener listener = new CameraView.OnStatusListener() {
        @Override
        public void onCreated() {
            if (camera != null) camera.start();
        }

        @Override
        public void onChanged() {

        }

        @Override
        public void onDestroyed() {
            if (camera != null) camera.stop();
        }
    };

}


