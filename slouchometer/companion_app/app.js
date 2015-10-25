/*
Description:
	JavaScript code for the Slouch-o-meter.

Credits:
	BAB, OJD, OLC, SG.
	ARM mbed [-_-]~
	http://mbed.org
*/

/**
 * Object that holds application data and functions.
 */
var app = {};
var allData = "All Data ...";
var slouching = 0;
var counter = 0;

/**
 * Name of device to connect to.
 */
app.deviceName = 'Pemgineers'

/**
 * Connected device.
 */
app.device = null;

/**
 * Initialise the application.
 */
app.initialize = function()
{
	document.addEventListener(
		'deviceready',
		function() { evothings.scriptsLoaded(app.onDeviceReady) },
		false);
};

/**
 * When low level initialization complete, this function is called.
 */
app.onDeviceReady = function()
{
	// Report status.
	app.showInfo('Enter BLE device name and tap Connect');

	// Show the saved device name, if any.
	var name = localStorage.getItem('deviceName');
	if (name)
	{
		app.deviceName = name;
	}
	$('#deviceName').val(app.deviceName);
};

/**
 * Print debug info to console and application UI.
 */
app.showInfo = function(info)
{
	document.getElementById('info').innerHTML = info;
	console.log(info);
};

/**
 * Scan for device and connect.
 */
app.startScan = function()
{
	evothings.easyble.startScan(
		function(device)
		{
			// Do not show un-named devices.
			var deviceName = device.advertisementData ?
				device.advertisementData.kCBAdvDataLocalName : null
			if (!device.name) { return }

			// Print "name : mac address" for every device found.
			console.log(device.name + ' : ' + device.address.toString().split(':').join(''))

			// If my device is found connect to it.
			if (device.hasName(app.deviceName))
			{
				app.showInfo('Status: Device found: ' + deviceName);
				evothings.easyble.stopScan();
				app.connectToDevice(device);
			}
		},
		function(error)
		{
			app.showInfo('Error: startScan: ' + error);
		});
};

/**
 * Read services for a device.
 */
app.connectToDevice = function(device)
{
	app.showInfo('Status: Connecting...');
	device.connect(
		function(device)
		{
			app.device = device;
			app.readServices(app.device);
			app.showInfo('Status: Connected');
		},
		function(errorCode)
		{
			app.showInfo('Error: Connection failed: ' + errorCode);
		});
};

/**
 * Dump all information on named device to the console
 */

app.readServices = function(device)
{
	// Read all services.
	device.readServices(
		null,
		function()
		{
			console.log("readServices success");

			// Debug logging of all services, characteristics and descriptors
			// reported by the BLE board.
			app.logAllServices(app.device);
		},
		function(error)
		{
			console.log('Error: Failed to read services: ' + error);
		});
		
		
		app.device.readCharacteristic(
		'00002a00-0000-1000-8000-00805f9b34fb',
		function(data)
		{
			console.log("DATA = " + data);
		},
		function(error)
		{
			console.log('Error: Read characteristic failed: ' + error);
		});
		
};

/**
 * when low level initialization complete,
 * this function is called
 */
app.onConnectButton = function()
{
	// Get device name from text field.
	app.deviceName = $('#deviceName').val();

	// Save it for next time we use the app.
	localStorage.setItem('deviceName', app.deviceName);

	// Call stop before you start, just in case something else is running.
	evothings.easyble.stopScan();
	evothings.easyble.closeConnectedDevices();

	// Only report devices once.
	evothings.easyble.reportDeviceOnce(true);

	// Start scanning.
	app.startScan();
	app.showInfo('Status: Scanning...');
};

/**
 * Toggle the LED on/off.
 */

var previousSlouching = 0;

var buttonToggle = function()
{
	app.device.readCharacteristic(
		'00002a1c-0000-1000-8000-00805f9b34fb',
		function(data)
		{
			previousSlouching = slouching;
			var testend = new DataView(data).getFloat32(1, false);
			//var test = evothings.util.bigEndianToInt32(data, 1);
			//var str = String.fromCharCode.apply(null, testend);
			console.log("DATA = " + testend);
			if(testend > 1) { 
				slouching = 1;
			} else {
				slouching = 0;
			}
		},
		function(error)
		{
			console.log('Error: Read characteristic failed: ' + error);
		});
		
		// slouching increment logic:
		
		if((slouching == 1) && (previousSlouching == 0)) {
			counter++;
		}
}

app.onToggleButton = function() {
	counter = 0;
}

/**
 * Debug logging of found services, characteristics and descriptors.
 */

app.logAllServices = function(device)
{
	// Here we simply print found services, characteristics,
	// and descriptors to the debug console in Evothings Workbench.

	// Notice that the fields prefixed with "__" are arrays that
	// contain services, characteristics and notifications found
	// in the call to device.readServices().

	// Print all services.
	console.log('Found services:');
	allData += 'Found services:';
	for (var serviceUUID in device.__services)
	{
		var service = device.__services[serviceUUID];
		console.log('  service: ' + service.uuid);
		allData += '  service: ' + service.uuid;

		// Print all characteristics for service.
		for (var characteristicUUID in service.__characteristics)
		{
			var characteristic = service.__characteristics[characteristicUUID];
			console.log('    characteristic: ' + characteristic.uuid);
			allData += '    characteristic: ' + characteristic.uuid;

			// Print all descriptors for characteristic.
			for (var descriptorUUID in characteristic.__descriptors)
			{
				var descriptor = characteristic.__descriptors[descriptorUUID];
				console.log('      descriptor: ' + descriptor.uuid);
				allData += '      descriptor: ' + descriptor.uuid;
			}
		}
	}
};

// Initialize the app.
app.initialize();

// Loop:
var myvar = setInterval(loop, 200);
function loop() {
	buttonToggle();
	if(slouching == 1) {
		document.getElementById('slouch').innerHTML = "Slouching!";
	} else {
		document.getElementById('slouch').innerHTML = "";
	}
	
	document.getElementById('count').innerHTML = counter.toString();
}