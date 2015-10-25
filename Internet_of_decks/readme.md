Internet Of Decks: Clothing containing LED Strips which beat in time to music
=============================================================================

Overview of Project
-------------------

We developed clothing containing an embedded microcontroller and LED strips which beat in time to music.
The LED Strips switch between many colours and flash in one of three patterns.
A phone app is used to control when the device is on and which of the three patterns is used.

Block Diagram of the Project
----------------------------
A block diagram of the project is found in the accompanyig powerpoint file.

How it works
------------

A DJ plays songs using a mixing software called Mixxx. 
The songs are streamed to a local IP address on an Icecast2 Media Streaming Server.
A python script queries the metadata produced by the Icecast2 server to work out the title and artist.
The song data is fetched from the SQLite database used by Mixxx to find the BPM of the song.
The python script then records a 1-second sample of the song, and performs a wave-correlation algorithm to find the time offset of the start of the beats
This data is fed into a BLE Nano which acts as a beacon and transmits the time offset together with the BPM as a BLE iBeacon.
The clients use this data to synchronise to the beats of the music.
A client app on an Smartphone allows the user to choose their preferred pattern of LED flashes, or to disable the LEDs.

Link to Youtube video is https://www.youtube.com/watch?v=1LZBshc2_f4&feature=youtu.be