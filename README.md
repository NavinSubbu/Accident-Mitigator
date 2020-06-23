# Accident-Mitigator
This is an arduino based device for detecting accident of a car and send a distress signal in a form of text messages along with the GPS coordinates of the location and call using GSM808 Module.

The device has an accelerometer and few flex sensors connected to an arduino nano board.
the accelerometer detects the orientation of the car and its sudden decceralation. If the car is found to be toppled due to accident the accelerometer wil detect it and trigger a send request. The arduino will then send a Text message to a designated Phone number with the current location.

if the accident occurs and the car is in normal orientation the flex sensors will detect the impact and if the threshold os exceeded then the messsgae send request is activated.

