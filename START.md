Start developing!
=================

We provide you with a repository containing 10 examples that show you how to use the BLE APIs. Those are a great starting point to get a feeling of what you can do with them!

BLE Examples
------------

Clone the ARM mbed Bluetooth Low Energy example applications:
```bash
git clone -b hackathon https://github.com/ARMmbed/ble-examples.git
cd ble-examples
```
Each of the examples is a yotta application. To show you how to build them, let’s pick the `BLE_Beacon` example:
```bash
cd BLE_Beacon
```
For this hackathon we will use the RedBearLab BLE nano target. To select it with yotta:
```bash
yotta target blenano-gcc
```
the `gcc` appendix indicates that we will be using the ARM-GCC toolchain.

As a yotta application, `BLE_Beacon` already specifies all the modules it depends on. If you want to see how it works just give a look to the `module.json` file. To install these dependencies, execute:
```bash
yotta install
```
You can now see how your dependency tree has been fetched by yotta with:
```bash
yotta ls
```
Hopefully, all those dependencies should have been fetched correctly. If you see some error just ask us and we will be able to help you!

Time to build!
```bash
yotta build
```
You will find the final binary in `build/blenano-gcc/source/ble-beacon-combined.hex`. Connect your BLE nano via USB — it will appear as a storage device. Just drag and drop the file onto the device, and you’re done!

Depending on the example, there are specific ways to see if it is working. For the `BLE_Beacon` example, checkout its specific documentation on how to [check for success](https://github.com/ARMmbed/ble-examples/tree/oob-oct15/BLE_Beacon#checking-for-success).

Final Notes
-----------

If you create a new application please remember to include this line in your `module.json` file:
```json
"mbed-hal-nrf51822-mcu": "AlessandroA/mbed-hal-nrf51822-mcu#1.1.1",
```
This is needed to fetch the latest module needed for your target device. We anyway suggest to build your project from within one of the provided examples, unless you want to try and create a module from scratch (see [here](http://yottadocs.mbed.com/tutorial/tutorial.html) for that).
