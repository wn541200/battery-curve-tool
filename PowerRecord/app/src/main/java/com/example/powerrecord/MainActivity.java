package com.example.powerrecord;

import androidx.annotation.RequiresApi;
import androidx.annotation.VisibleForTesting;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.FileProvider;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.BatteryManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.PowerManager;
import android.provider.Settings;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.EventListener;

public class MainActivity extends AppCompatActivity {

    private TextView textView;
    private int offset;
    private String path = Environment.getExternalStorageDirectory().getPath() + "/powerRecord.txt";
    private static final String POWER_TEST_WAKE_LOCK = "PowerTestWakeLock";
    @VisibleForTesting
    PowerManager.WakeLock mWakeLock;

    @SuppressLint("InvalidWakeLockTag")
    @RequiresApi(api = Build.VERSION_CODES.M)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        textView = (TextView)findViewById(R.id.valueText);
        textView.setText("电池测试：\n");
        Button setBright = (Button)findViewById((R.id.bright));
        Button cancel = (Button)findViewById((R.id.cancel));
        final PowerManager powerManager = this.getSystemService(PowerManager.class);
        mWakeLock = powerManager.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK
                | PowerManager.ON_AFTER_RELEASE, POWER_TEST_WAKE_LOCK);
        setBright.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mWakeLock != null && !mWakeLock.isHeld()) {
                    Log.d("PowerRecord", "always bright on acquire wakelock");
                    mWakeLock.acquire();
                }
            }
        });

        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mWakeLock != null && mWakeLock.isHeld()) {
                    Log.d("PowerRecord", "release wakelock");
                    mWakeLock.release();
                }
            }
        });


        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            boolean permission = Environment.isExternalStorageManager();
            if (!permission) {
                Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                intent.setData(Uri.fromParts("package", getPackageName(), null));
                intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                this.startActivity(intent);
            }
        }
        IntentFilter intentFilter = new IntentFilter("android.intent.action.BATTERY_CHANGED");
        registerReceiver(new BatteryReceiver(), intentFilter);
    }

    class BatteryReceiver extends BroadcastReceiver
    {
        public void onReceive(Context context, Intent intent)
        {
            if ("android.intent.action.BATTERY_CHANGED".equals(intent.getAction())) {
                //电量
                int batteryLevel = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, 0);
                int batteryScale = intent.getIntExtra(BatteryManager.EXTRA_SCALE, 0);
                //电压
                int batteryVoltage = intent.getIntExtra(BatteryManager.EXTRA_VOLTAGE, 0);
                int batteryTemperature = intent.getIntExtra(BatteryManager.EXTRA_TEMPERATURE, 0);
                int batteryCurrent = intent.getIntExtra("current", 0);

                String time = new SimpleDateFormat("yyyy年MM月dd日  HH:mm:ss").format(new Date(System.currentTimeMillis()));
                String currentValue = time + " 电池电量: " + batteryLevel * 100 / batteryScale + "%" + " , 电压: " + batteryVoltage
                        + " , 电流: " + batteryCurrent + " , 温度: " + batteryTemperature + "\n";

                //Log.d("PowerRecord", "battery test : " + currentValue);
               textView.append(currentValue);
               offset = textView.getLineCount() * textView.getLineHeight();
               if (offset > textView.getHeight()) {
                   textView.scrollTo(0, offset - textView.getHeight());
               }
                try {
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                        boolean permission = Environment.isExternalStorageManager();
                        if (permission) {
                            FileWriter fileWriter = new FileWriter(path, true);
                            fileWriter.write(currentValue);
                            fileWriter.close();
                        } else {
                            Log.d("PowerRecord", "permission denied");
                        }
                    }
                } catch (Exception e) {
                    Log.d("PowerRecord", "FileWrite exception : " + e.toString());
                }
            }
        }
    }
}