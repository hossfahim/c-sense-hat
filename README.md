# Raspberry Pi Sense-HAT add-on board

## C language demonstration code

### Tested with:  
- Sense HAT v1.0 / Raspberry Pi 3 B+ / Raspbian GNU/Linux 10 (buster)
- Sense HAT v1.0 / Raspberry Pi 4  / Raspbian GNU/Linux 11 (Bullseye) in 2024


### TO USE

Clone:
```
git clone --recurse-submodules  https://github.com/TheWiner1/c-sense-hat
```

Each time you make a modification and you want to run the project:
```
make clean
```
then
```
make 
```
finally
```
./sock
```

pressure.c : Get pressure and temperature readings from the LPS25H pressure sensor.

humidity.c : Get relative humidity and temperature readings from the HTS221 humidity sensor.

> This is a more complex procedure than the pressure sensor, above. Each HTS221 sensor is
individually calibrated and the results stored in designated registers. These calibration
registers are then read and the straight line equations for calibration (y = mx + c) determined.
The measured ambient values are then converted to give the output humidity and temperature,
by reference to these calibration linear equations.

led_matrix.c : Demonstrate control of the LED matrix using the mmap method to map the led device into memory.
