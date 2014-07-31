OpticalBadgeReader
==================

Optically detect and read information from common employee badges.

Build
=====

Tested on an Ubuntu 12.04 x86_64 machine and an armv7l (pcDuino v2). Follow http://docs.opencv.org/trunk/doc/tutorials/introduction/linux_install/linux_install.html.

    sudo apt-get update
    sudo apt-get install build-essential
    sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev

Algorithm
=========

    +---------------+    +---------------+    +---------------+    +---------------+
    |               |    |               |    |               |    |               |
    | Aquire Image  +--> |  Blue Filter  +--> |  Noise Filter +--> | Edge Detector |
    |               |    |               |    |               |    |               |
    +---------------+    +---------------+    +---------------+    +-------+-------+
                                                                           |       
                                                                           v       
                                              +---------------+    +---------------+
                                              |               |    |               |
                                              | Quadrilateral | <--+ Find Countours|
                                              |    Filter     |    |               |
                                              +---------------+    +---------------+
