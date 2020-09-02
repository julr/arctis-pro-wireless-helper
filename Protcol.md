# IMPORTANT
All information in this document was collected by sniffing the communication between the device and the SteelSeries Engine application.
It is not a complete protocol documentation and the present information might not be correct.

# Protocol
## Device interface
```
VID: 0x1038
PID: 0x1290
Interface: 0
```

## Master to Device
* 32 Byte report length, report ID 0
* first byte seems to be the command
* second byte always 0xAA
* rest of the data are optional parameters

## Responses from Device
* 32 bytes report length, only available after a request was sent
* command byte 0x40, no parameters -> returns headset battery level at byte 0 [rest of the message always 0]
* command byte 0x41, no parameters -> returns headset connection status. Byte 0 is 0x02 when it is not connected, 0x04 when connected, second byte is always 2 [rest of the message always 0]
* command byte 0x42, no parameters -> returns base battery charge at byte 0 [rest of the message always 0]

### Battery level indication
* 0x00 = no (or empty?) battery
* 0x01 to 0x04 = number of bars on display
* 0xFF = unknown or invalid [sometimes sent right after connecting the headset]

# SeelSeries Engine behavior
When no headset is connected or the connection is stable (connection sequence finished) the following commands are looped:
1. send 0x40 command
2. send 0x41 command
3. send 0x42 command

