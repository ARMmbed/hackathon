# ARM mbed Hackathon

## The Cambridge University Engineering Society Hackathon
Create your own ARM based mbed wearables at the 24-25th October hackaton at the University of Cambridge Department of Engineering. You can find a live feed at our [facebook event page](https://www.facebook.com/events/1492417234391358/). The official hashtags for the event are **#CUESHackathon** and **#mbed**.

Thanks to everybody for making this event possible:
- [ARM mbed](https://www.mbed.com/en/) for sponsoring the prizes, hardware, electronic parts, food, drinks and engineering support;
- [Cambridge University Engineering Society](https://cuengineeringsociety.org.uk/events/cues-hackathon/) for setting up the event, providing food and shelter;
- [RedBearLab](http://redbearlab.com/) for giving ARM and the attendess generous discounts for their [BLEnano hardware](http://redbearlab.com/blenano/);
- [Nordic Semiconductors](https://www.mbed.com/en/partners/nordic/) for their [nRF51822 SoC](http://www.nordicsemi.com/eng/Products/Bluetooth-Smart-Bluetooth-low-energy/nRF51822) Bluetooth Low Energy chip that is the heart of [BLEnano](http://redbearlab.com/blenano/) and many other IoT wearable products.

## Contributions & Prize Jury
You are invited to add an introduction to your design and links to [our wiki pages](https://github.com/ARMmbed/hackathon/wiki) or to this page. Please sign up with your GitHub account and start editing!

How to sign up for the prizes:
- Add links to the documentation of your design to the list of [Active Projects](#active-projects) below:
  - one or more photos/screenshots/sketches of your project;
  - one paragraph describing your solution;
  - quality of the documentation and material matters -- start early!
  - submission deadline is Sunday the 25th at noon (12:00).

Please submit a pull request against this repository to have your project linked in the project list below:

### Active Projects
- ARM mbed Bluetooth Low Energy [example applications](https://github.com/ARMmbed/ble-examples/tree/hackathon); **note**: check out [these instructions](START.md) to start building the examples on the RedBearLab BLE nano device!
- [IoT-enabled Wearable SmartBin](SmartBin)

### Helpful Links
- [yotta](http://yottadocs.mbed.com/): distributed software package management for embedded devices -- publish your own modules!
- [BLE Getting Started Docs](https://docs.mbed.com/docs/ble-intros/en/latest/): documentation to get you started with mbed BLE. **Please give us feedback on these docs! Write it down, or tell Irit Arkin, Jonny Austin, Andres Amaya Garcia or some other ARM person**
- [Compiler](https://launchpad.net/gcc-arm-embedded): Embedded GNU-GCC ARM compiler toolchain (maintained by ARM);
- [BLEnano](http://redbearlab.com/blenano/): the hardware platform for this hackathon;
- [Evothings for mbed](https://evothings.com/things/mbed): connects your Bluetooth devices to the internet through your phone.


### Virtual Development Machine Environment
Download a virtual machine image of our [Fedora Linux Development Environment](http://get.meriac.com/ARMmbed.zip) with pre-installed mbed development environment and yotta based on Fedora Workstation (FC22). **You don't need this image if you plan to run [yotta](http://yottadocs.mbed.com/) and [ARM-GCC](https://launchpad.net/gcc-arm-embedded) natively**. For running this image on your machine, please download [VMware Player](https://my.vmware.com/web/vmware/free#desktop_end_user_computing/vmware_workstation_player/12_0).

You need to perform these post-installation steps when running the downloaded VMware image for the first time:
```bash
sudo ln -s /usr/bin/ninja-build /usr/bin/ninja
```

### What should you expect from the hackathon?

At this ARM sponsored event you bring your laptop and get the opportunity to use ARM mbed microcontrollers, sensors, explore the ARM mbed OS, ARM device servers and other products in the mbed eco-system. ARM engineers will teach participants how to get started with the mbed platform and be on-site for support throughout the event.

Our Theme -- Wearables:
- The aim is to create an electronic device that uses the ARM mbed IoT Device Platform to develop a wearable product;
- Teams will have ~~24~~ 25 hours (the clocks go back) to create their device;
- Prizes will be given at the end for the best designs;
- Food and drinks will also be provided.

This event is not a competition but a party. It is completely free of charge, a great place to meet new, like-minded people and all about having fun!

### Showcase at Makespace Cambridge

On Thursday 29th October, Makespace is running a Showcase event at [Makespace](http://makespace.org/) - the Cambridge Hackspace/Makerspace.

All participants from the Hackathon have a free ticket to exhibit, and this event is part of the Festival of Ideas, so there will be a wide audience from both Cambridge University and the town.

[Here's the event page on meetup](http://www.meetup.com/Makespace/events/225955983/). Please sign up to come to the event, whether you want to come and see the Makespace showcase or to show off the cool things you've made!


