package com.example.charlie.snuzz;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothClass;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.content.Context;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import java.util.Queue;
import java.util.UUID;
import java.util.concurrent.ConcurrentLinkedQueue;

/**
 * Created by Charlie on 2015-10-24.
 *
 * Interface between the app and the device
 *
 * Nicely described by http://toastdroid.com/2014/09/22/android-bluetooth-low-energy-tutorial/
 *
 */
public class DeviceCommunication {

    private BluetoothGatt connGatt;
    BluetoothAdapter adapter;
    private static String TAG = "DevComm";

    public static UUID UART_UUID = UUID.fromString("00001800-0000-1000-8000-00805f9b34fb");
    public static UUID TX_UUID   = UUID.fromString("00002a00-0000-1000-8000-00805f9b34fb");
    public static UUID RX_UUID   = UUID.fromString("00002a01-0000-1000-8000-00805f9b34fb");


    // UUIDs for the Device Information service and associated characeristics.
    public static UUID DIS_UUID       = UUID.fromString("0000180a-0000-1000-8000-00805f9b34fb");
    public static UUID DIS_MANUF_UUID = UUID.fromString("00002a29-0000-1000-8000-00805f9b34fb");
    public static UUID DIS_MODEL_UUID = UUID.fromString("00002a24-0000-1000-8000-00805f9b34fb");
    public static UUID DIS_HWREV_UUID = UUID.fromString("00002a26-0000-1000-8000-00805f9b34fb");
    public static UUID DIS_SWREV_UUID = UUID.fromString("00002a28-0000-1000-8000-00805f9b34fb");


    private BluetoothGattCharacteristic tx;
    private BluetoothGattCharacteristic rx;


    // Queues for characteristic read (synchronous)
    private Queue<BluetoothGattCharacteristic> readQueue;

    // Application context
    private Context context;


    public void sendSomeShit() {
        if (tx == null) {
            // Do nothing if there is no connection or message to send.
            Log.d(TAG, "Trying to write something but not connected. Oh no!");
            return;
        }
        byte[] buffer = new byte[4];
        buffer[0] = (byte) 0;
        buffer[1] = (byte) 1;
        buffer[2] = (byte) 200;
        buffer[3] = (byte) 201;
        // Update TX characteristic value.  Note the setValue overload that takes a byte array must be used.
        tx.setValue(buffer);
        connGatt.writeCharacteristic(tx);
    }




    private final BluetoothGattCallback btleGattCallback = new BluetoothGattCallback() {

        @Override
        public void onCharacteristicRead (BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            super.onCharacteristicRead(gatt, characteristic, status);

            if (status == BluetoothGatt.GATT_SUCCESS) {
                //Log.w("DIS", characteristic.getStringValue(0));
                // Check if there is anything left in the queue
                BluetoothGattCharacteristic nextRequest = readQueue.poll();
                if(nextRequest != null){
                    // Send a read request for the next item in the queue
                    gatt.readCharacteristic(nextRequest);
                }
                else {
                    // We've reached the end of the queue
                    Log.d(TAG, "Device info available!");
                }
            }
            else {
                //Log.w("DIS", "Failed reading characteristic " + characteristic.getUuid().toString());
            }
        }
        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, final BluetoothGattCharacteristic characteristic) {
            // this will get called anytime you perform a read or write characteristic operation
            Log.d(TAG, "onCharacteristicChanged: " + gatt.toString());
        }

        @Override
        public void onConnectionStateChange(final BluetoothGatt gatt, final int status, final int newState) {
            super.onConnectionStateChange(gatt, status, newState);

            // this will get called when a device connects or disconnects
            Log.d(TAG, "onConnectionStateChange: " + newState);


            if (newState == BluetoothGatt.STATE_CONNECTED) {
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    // Connected to device, start discovering services.
                    if (!gatt.discoverServices()) {
                        // Error starting service discovery.
                        Log.d(TAG, "Error starting service discovery!");
                    }
                }
                else {
                    // Error connecting to device.
                    Log.d(TAG, "Error connecting!");
                }
            }
            else if (newState == BluetoothGatt.STATE_DISCONNECTED) {
                // Disconnected, notify callbacks of disconnection.
                rx = null;
                tx = null;
                Log.d(TAG, "Disconnected!");
                //TODO gracefully handle being disconnected
            }
        }

        @Override
        public void onServicesDiscovered(final BluetoothGatt gatt, final int status) {
            super.onServicesDiscovered(gatt, status);

            // this will get called after the client initiates a BluetoothGatt.discoverServices() call
            Log.d(TAG, "onServicesDiscovered: status "+status);

            //GATT_INTERNAL_ERROR
            if(status == 129) {
                Log.d(TAG, "GATT_ERROR!");
            }


            // Notify connection failure if service discovery failed.
            if (status == BluetoothGatt.GATT_FAILURE) {
                Log.e(TAG, "GATT Failure on services discovered.");
                return;
            }

            Log.d(TAG, "Listing services:");
            for(BluetoothGattService s: gatt.getServices()) {
                Log.d(TAG, "Found service with UUID "+ s.getUuid().toString());
            }
            Log.d(TAG, "Finished listing services.");

            // Save reference to each UART characteristic.
            BluetoothGattService uartService = gatt.getService(UART_UUID);
            if(uartService == null) {
                Log.d(TAG, "Unable to get the UART service!");
                return;
            }


            for(BluetoothGattCharacteristic c: uartService.getCharacteristics()) {
                Log.d(TAG, "Found characteristic: "+c.getUuid().toString() +", props "+c.getProperties()+", "+c.getPermissions());
            }

            tx = uartService.getCharacteristic(TX_UUID);
            rx = uartService.getCharacteristic(RX_UUID);

            if(tx == null) {
                Log.d(TAG, "TX characteristic is null");
            }
            if(rx == null) {
                Log.d(TAG, "RX Characteristic is null");
            }

            Log.d(TAG, "Success! As long as you're lucky...");
            connGatt = gatt;

        }
    };

    public DeviceCommunication(Context c) {
        this.context = c;
        this.readQueue = new ConcurrentLinkedQueue<BluetoothGattCharacteristic>();
        //TODO init other variables
    }




    private BluetoothAdapter.LeScanCallback leScanCallback = new BluetoothAdapter.LeScanCallback() {
        @Override
        public void onLeScan(final BluetoothDevice device, final int rssi, final byte[] scanRecord) {

            if(device.getName() == null)
                return;

            if (!device.getName().equals("CJB123")) {
                Log.d(TAG, "(found useless device: " + device.getName()+")");
            } else {
                Log.d(TAG, "Found a relevant device: " + device.getName());

                //We've found the desired device, now we can stop scanning
                adapter.stopLeScan(leScanCallback);
                //Initiate connection
                connGatt = device.connectGatt(context, true, btleGattCallback);

            }


        }
    };

    void startScan() {

        this.adapter = BluetoothAdapter.getDefaultAdapter();
        if (adapter != null && !adapter.isEnabled()) {
            Log.e(TAG, "You've disabled Bluetooth, you idiot.");
            return;
        }

        try {
            adapter.startLeScan(leScanCallback);
        } catch (Exception ex) {
            Log.d(TAG, "Unable to start LE startScan: "+ex.toString());
        }
    }




    // Filtering by custom UUID is broken in Android 4.3 and 4.4, see:
    //   http://stackoverflow.com/questions/18019161/startlescan-with-128-bit-uuids-doesnt-work-on-native-android-ble-implementation?noredirect=1#comment27879874_18019161
    // This is a workaround function from the SO thread to manually parse advertisement data.
    private List<UUID> parseUUIDs(final byte[] advertisedData) {
        List<UUID> uuids = new ArrayList<UUID>();

        int offset = 0;
        while (offset < (advertisedData.length - 2)) {
            int len = advertisedData[offset++];
            if (len == 0)
                break;

            int type = advertisedData[offset++];
            switch (type) {
                case 0x02: // Partial list of 16-bit UUIDs
                case 0x03: // Complete list of 16-bit UUIDs
                    while (len > 1) {
                        int uuid16 = advertisedData[offset++];
                        uuid16 += (advertisedData[offset++] << 8);
                        len -= 2;
                        uuids.add(UUID.fromString(String.format("%08x-0000-1000-8000-00805f9b34fb", uuid16)));
                    }
                    break;
                case 0x06:// Partial list of 128-bit UUIDs
                case 0x07:// Complete list of 128-bit UUIDs
                    // Loop through the advertised 128-bit UUID's.
                    while (len >= 16) {
                        try {
                            // Wrap the advertised bits and order them.
                            ByteBuffer buffer = ByteBuffer.wrap(advertisedData, offset++, 16).order(ByteOrder.LITTLE_ENDIAN);
                            long mostSignificantBit = buffer.getLong();
                            long leastSignificantBit = buffer.getLong();
                            uuids.add(new UUID(leastSignificantBit,
                                    mostSignificantBit));
                        } catch (IndexOutOfBoundsException e) {
                            // Defensive programming.
                            //Log.e(LOG_TAG, e.toString());
                            continue;
                        } finally {
                            // Move the offset to read the next uuid.
                            offset += 15;
                            len -= 16;
                        }
                    }
                    break;
                default:
                    offset += (len - 1);
                    break;
            }
        }
        return uuids;
    }
}
