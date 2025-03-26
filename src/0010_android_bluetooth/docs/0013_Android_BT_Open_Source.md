# Android BT Open Source
SSP Disconnect 、BT Serial、BT Sample Code

# 1 参考文档

* [Disconnect a bluetooth socket in Android](https://stackoverflow.com/questions/3031796/disconnect-a-bluetooth-socket-in-android)

* [myBluetooth](https://github.com/loumeng/myBluetooth)

* [connectivity-samples](https://github.com/android/connectivity-samples)


# 2 Android BT SPP Disconnect

正确断开BT SPP的方法

# 3 说明

* Please remember to close your Input/output streams first, then close the socket.
* By closing the streams, you kick off the disconnect process. After you close the socket, the connection should be fully broken down.
* If you close the socket before the streams, you may be bypassing certain shutdown steps, such as the (proper) closing of the physical layer connection.

# BT serial
https://github.com/loumeng/myBluetooth

# BT Sample Code
https://github.com/android/connectivity-samples