package com.example.bluetooth;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
//import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CompoundButton;
import android.widget.FrameLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Set;
import java.util.UUID;


import static android.R.layout.simple_list_item_1;

public class MainActivity extends Activity  implements CompoundButton.OnCheckedChangeListener{

    ToggleButton tb1,tb2,tb3,tb4;
    private static final int REQUEST_ENABLE_BT = 1;
    public TextView textInfo;
    BluetoothAdapter bluetoothAdapter;
    ArrayList<String> pairedDeviceArrayList;
    ListView listViewPairedDevice;
    FrameLayout ButPanel;
    ArrayAdapter<String> pairedDeviceAdapter;
    ThreadConnectBTdevice myThreadConnectBTdevice;
    ThreadConnected myThreadConnected;
    private UUID myUUID;
    private StringBuilder sb = new StringBuilder();



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tb1 = (ToggleButton) findViewById(R.id.toggleButton1);
        tb2 = (ToggleButton) findViewById(R.id.toggleButton2);
        tb3 = (ToggleButton) findViewById(R.id.toggleButton3);
        tb4 = (ToggleButton) findViewById(R.id.toggleButton4);

        tb1.setOnCheckedChangeListener(this);
        tb2.setOnCheckedChangeListener(this);
        tb3.setOnCheckedChangeListener(this);
        tb4.setOnCheckedChangeListener(this);
        final String UUID_STRING_WELL_KNOWN_SPP = "00001101-0000-1000-8000-00805F9B34FB";
        textInfo = (TextView)findViewById(R.id.textInfo);
        listViewPairedDevice = (ListView)findViewById(R.id.list);
        ButPanel = (FrameLayout) findViewById(R.id.panel);
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH)){
            Toast.makeText(this, "BLUETOOTH не поддерживаетсбля на вашем устройтсве ", Toast.LENGTH_LONG).show();
            finish();
            return;
        }
        myUUID = UUID.fromString(UUID_STRING_WELL_KNOWN_SPP);
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter == null) {
            Toast.makeText(this, "Bluetooth is not supported on this hardware platform, включи блутуз", Toast.LENGTH_LONG).show();
            finish();
            return;
        }
        String stInfo = bluetoothAdapter.getName() + " " + bluetoothAdapter.getAddress();
        textInfo.setText(String.format("Это устройство: %s", stInfo));
    }

    @Override
    protected void onStart() { // Запрос на включение Bluetooth
        super.onStart();
        if (!bluetoothAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        }
        setup();
    }

    private void setup(){ // Создание списка сопряжённых Bluetooth-устройств
        Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();
        if (pairedDevices.size() > 0) { // Если есть сопряжённые устройства
            pairedDeviceArrayList = new ArrayList<>();
            for (BluetoothDevice device : pairedDevices) { // Добавляем сопряжённые устройства - Имя + MAC-адресс
                pairedDeviceArrayList.add(device.getName() + "\n" + device.getAddress());
            }
            pairedDeviceAdapter = new ArrayAdapter<>(this, simple_list_item_1, pairedDeviceArrayList);
            listViewPairedDevice.setAdapter(pairedDeviceAdapter);
            listViewPairedDevice.setOnItemClickListener(new AdapterView.OnItemClickListener() { // Клик по нужному устройству

                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    listViewPairedDevice.setVisibility(View.GONE); // После клика скрываем список
                    String  itemValue = (String) listViewPairedDevice.getItemAtPosition(position);
                    String MAC = itemValue.substring(itemValue.length() - 17); // Вычленяем MAC-адрес
                    BluetoothDevice device2 = bluetoothAdapter.getRemoteDevice("98d3:51:fda4eb");
                    myThreadConnectBTdevice = new ThreadConnectBTdevice(device2);
                    myThreadConnectBTdevice.start();  // Запускаем поток для подключения Bluetooth
                }
            });
        }
    }


    @Override
    protected void onDestroy() { // Закрытие приложения
        super.onDestroy();
        if(myThreadConnectBTdevice!=null) myThreadConnectBTdevice.cancel();
    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if(requestCode == REQUEST_ENABLE_BT){ // Если разрешили включить Bluetooth, тогда void setup()
            if(resultCode == Activity.RESULT_OK) {
                setup();
            }
            else { // Если не разрешили, тогда закрываем приложение
                Toast.makeText(this, "нужно подключится к bluetoosh. Bluetoosh я дал, РАЗРЕШЕНИЕ Я НЕ ДАМ   ", Toast.LENGTH_SHORT).show();
                finish();
            }
        }
    }
////////////////////////////////////КОНЕКТ И ПРИНЯТИЕ ДАННЫХ/////////////////////////////////////////////
    private class ThreadConnectBTdevice extends Thread { // Поток для коннекта с Bluetooth
        private BluetoothSocket bluetoothSocket = null;
        private ThreadConnectBTdevice(BluetoothDevice device) {
            try {
                bluetoothSocket = device.createRfcommSocketToServiceRecord(myUUID);
            }
            catch (IOException e) {
                e.printStackTrace();
            }
        }


        @Override
        public void run() { // Коннект
            boolean success = false;
            try {
                bluetoothSocket.connect();
                success = true;
            }
            catch (IOException e) {
                e.printStackTrace();
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this, "Нет коннекта, проверьте Bluetooth-устройство с которым хотите соединица!", Toast.LENGTH_LONG).show();
                        listViewPairedDevice.setVisibility(View.VISIBLE);
                    }
                });
                try {
                    bluetoothSocket.close();
                }
                catch (IOException e1) {
                    e1.printStackTrace();
                }
            }
            if(success) {  // Если законнектились, тогда открываем панель с кнопками и запускаем поток приёма и отправки данных
                runOnUiThread(new Runnable() {

                    @Override
                    public void run() {
                        ButPanel.setVisibility(View.VISIBLE); // открываем панель с кнопками
                    }
                });

                myThreadConnected = new ThreadConnected(bluetoothSocket);
                myThreadConnected.start(); // запуск потока приёма и отправки данных
            }
        }

        public void cancel() {
            Toast.makeText(getApplicationContext(), "Close - BluetoothSocket", Toast.LENGTH_LONG).show();
            try {
                bluetoothSocket.close();
            }
            catch (IOException e) {
                e.printStackTrace();
            }
        }

    } // END ThreadConnectBTdevice:

    ////////////////////////////////////ПРИНЯТИЕ И ПАРСИНГ ДАННЫХ/////////////////////////////////////////////

    private class ThreadConnected extends Thread {    // Поток - приём и отправка данных
        private final InputStream connectedInputStream;
        private final OutputStream connectedOutputStream;
        private String sbprint;
        public ThreadConnected(BluetoothSocket socket) {
            InputStream in = null;
            OutputStream out = null;
            try {
                in = socket.getInputStream();
                out = socket.getOutputStream();
            }
            catch (IOException e) {
                e.printStackTrace();
            }
            connectedInputStream = in;
            connectedOutputStream = out;
        }

        @Override
        public void run() { // Приём данных
            while (true) {
                try {
                    byte[] buffer = new byte[1];
                    int bytes = connectedInputStream.read(buffer);
                    String strIncom = new String(buffer, 0, bytes);
                    sb.append(strIncom); // собираем символы в строку
                    int endOfLineIndex = sb.indexOf("\r\n"); // определяем конец строки
                    if (endOfLineIndex > 0) {
                        sbprint = sb.substring(0, endOfLineIndex);
                        sb.delete(0, sb.length());
                        runOnUiThread(new Runnable() { // Вывод данных

                            @Override
                            public void run() {
                                switch (sbprint) {

                                    case "D10 ON":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    case "D10 OFF":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    case "D11 ON":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    case "D11 OFF":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    case "D12 ON":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    case "D12 OFF":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    case "D13 ON":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    case "D13 OFF":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    default:
                                        break;
                                }
                            }
                        });
                    }
                } catch (IOException e) {
                    break;
                }
            }
        }


        public void write(byte[] buffer) {
            try {
                connectedOutputStream.write(buffer);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }

    @Override
    public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
        switch (compoundButton.getId()) {
            case R.id.toggleButton1:
                if(isChecked){
                    if(myThreadConnected!=null) {
                        byte[] bytesToSend = "a".getBytes();
                        myThreadConnected.write(bytesToSend );
                    }
                    Toast.makeText(MainActivity.this, "D10 ON", Toast.LENGTH_SHORT).show();
                }else{
                    if(myThreadConnected!=null) {
                        byte[] bytesToSend = "A".getBytes();
                        myThreadConnected.write(bytesToSend );
                    }
                    Toast.makeText(MainActivity.this, "D10 OFF", Toast.LENGTH_SHORT).show();
                }
                break;
            case R.id.toggleButton2:
                if(isChecked){
                    if(myThreadConnected!=null) {

                        byte[] bytesToSend = "b".getBytes();
                        myThreadConnected.write(bytesToSend );
                    }

                    Toast.makeText(MainActivity.this, "D11 ON", Toast.LENGTH_SHORT).show();
                }else{
                    if(myThreadConnected!=null) {

                        byte[] bytesToSend = "B".getBytes();
                        myThreadConnected.write(bytesToSend );
                    }

                    Toast.makeText(MainActivity.this, "D11 OFF", Toast.LENGTH_SHORT).show();
                }
                break;
            case R.id.toggleButton3:
                if(isChecked){
                    if(myThreadConnected!=null) {

                        byte[] bytesToSend = "c".getBytes();
                        myThreadConnected.write(bytesToSend );
                    }

                    Toast.makeText(MainActivity.this, "D12 ON", Toast.LENGTH_SHORT).show();
                }else{
                    if(myThreadConnected!=null) {

                        byte[] bytesToSend = "C".getBytes();
                        myThreadConnected.write(bytesToSend );
                    }

                    Toast.makeText(MainActivity.this, "D12 OFF", Toast.LENGTH_SHORT).show();
                }
                break;
            case R.id.toggleButton4:
                if(isChecked){
                    if(myThreadConnected!=null) {

                        byte[] bytesToSend = "d".getBytes();
                        myThreadConnected.write(bytesToSend );
                    }

                    Toast.makeText(MainActivity.this, "D13 ON", Toast.LENGTH_SHORT).show();
                }else{
                    if(myThreadConnected!=null) {

                        byte[] bytesToSend = "D".getBytes();
                        myThreadConnected.write(bytesToSend );
                    }

                    Toast.makeText(MainActivity.this, "D13 OFF", Toast.LENGTH_SHORT).show();
                }
                break;
        }
    }




}
