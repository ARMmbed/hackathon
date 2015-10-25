/*
Description:

	JavaScript code for the mbed GATT example app.

Credits:

	ARM mbed [-_-]~

	http://mbed.org
*/

/**
 * Object that holds application data and functions.
 */
var app = {};


/**
 * Name of device to connect to.
 */
app.deviceName = 'Here!!'


/**
 * LED defines (inverted).
 */
app.ledOFF = 0;
app.ledON  = 1;

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
		function() { 
			evothings.scriptsLoaded(app.onDeviceReady) },
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
			app.showInfo('Status: Connected');
			app.readServices(app.device);
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
			$('#connectView').hide();
			$('#selectView').show();
			app.logAllServices(app.device);
		},
		function(error)
		{
			console.log('Error: Failed to read services: ' + error);
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
record the humpback data
**/
app.flexV = 209;

app.OnHumpback = function(){
	console.log("inhumpback");
	app.interval = setInterval(function() {app.readHumpback()}, 500);
		
};

app.readHumpback = function(){
	app.device.readCharacteristic(
		'00002a61-0000-1000-8000-00805f9b34fb',
		function(data)
		{
			console.log("in read");
			var view = new Uint8Array(data);
			var led = new Uint8Array(1);
			console.log("!:"+view[1]);
			app.flexV=view[1];
			
 			if (app.flexV<215){
 				document.getElementById("flexW").innerHTML='your spine is suffering!!';;
 				led[0] = app.ledON;
 			}
 			else{
 				led[0]=app.ledOFF;
 			}
 			app.device.writeCharacteristic(
			'0000a001-0000-1000-8000-00805f9b34fb',
			led,
			function(){console.log("LED is on,spine is suffering")},
			function(error) {console.log("LED toggled failed")});

		},
		function(error)
		{
			console.log('Error: Read characteristic failed: ' + error);
		});
}

/**
record the steps data
**/
app.stepCount = 0;
app.flag = 0;
app.isFirst = 1;
app.thres = 0;

app.OnSteps = function(){
	app.stepCount = 0;
	console.log("insteps");
	app.interval = setInterval(function() {app.readSteps()}, 300);
};

app.readSteps = function(){
	app.device.readCharacteristic(
		'00002a61-0000-1000-8000-00805f9b34fb',
		function(data)
		{
			console.log("in read");
			var view = new Uint8Array(data);
			var led = new Uint8Array(1);

			console.log("!:"+view[1]);
			app.flexV=view[1];
			if (app.isFirst == 1){
				app.thres = app.flexV-5;
				app.isFirst =0;
			}

			if (app.flag == 0)
			{
				if (app.flexV < app.thres)
					app.flag = 1;
			}
			else
			{
				if (app.flexV >= app.thres)
				{
					app.flag = 0;
					app.stepCount = app.stepCount + 1;
					console.log("stepCount:"+app.stepCount);
				}
			}
			document.getElementById("step").innerHTML='you have walked '+ app.stepCount + 'steps';
			document.getElementById("stepCalories").innerHTML='you have burned '+ app.stepCount/20 + 'calories';


		},
		function(error)
		{
			console.log('Error: Read characteristic failed: ' + error);
		});
}


/**
record push-up


app.OnPushUps = function(){
	app.PushupCount = 0;
	console.log("insteps");
	app.interval = setInterval(function() {app.readSteps()}, 300);
};

app.readSteps = function(){
	app.device.readCharacteristic(
		'00002a61-0000-1000-8000-00805f9b34fb',
		function(data)
		{
			console.log("in read");
			var view = new Uint8Array(data);
			var led = new Uint8Array(1);

			console.log("!:"+view[1]);
			app.flexV=view[1];
			if (app.isFirst == 1){
				app.thres = app.flexV-5;
				app.isFirst =0;
			}

			if (app.flag == 0)
			{
				if (app.flexV < app.thres)
					app.flag = 1;
			}
			else
			{
				if (app.flexV >= app.thres)
				{
					app.flag = 0;
					app.stepCount = app.stepCount + 1;
					console.log("stepCount:"+app.stepCount);
				}
			}
			document.getElementById("step").innerHTML='you have walked '+ app.stepCount + 'steps';
			document.getElementById("stepCalories").innerHTML='you have burned '+ app.stepCount/20 + 'calories';


		},
		function(error)
		{
			console.log('Error: Read characteristic failed: ' + error);
		});
}
**/



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
	for (var serviceUUID in device.__services)
	{
		var service = device.__services[serviceUUID];
		console.log('  service: ' + service.uuid);

		// Print all characteristics for service.
		for (var characteristicUUID in service.__characteristics)
		{
			var characteristic = service.__characteristics[characteristicUUID];
			console.log('    characteristic: ' + characteristic.uuid);

			// Print all descriptors for characteristic.
			for (var descriptorUUID in characteristic.__descriptors)
			{
				var descriptor = characteristic.__descriptors[descriptorUUID];
				console.log('      descriptor: ' + descriptor.uuid);
			}
		}
	

	
	};
};

// Initialize the app.
app.initialize();
