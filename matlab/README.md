# teensy_pd/matlab

- This is an example MATLAB Simulink project to receive and display telemetry.
- It captures and classifies data received over the UDP packets relayed by the [subproject executable](teensy_pd/rasppi).
- Data received by MATLAB simulink is not syncronized to real-time, packets may drop. 
- However, each packet is associated with a time index and measurement time.