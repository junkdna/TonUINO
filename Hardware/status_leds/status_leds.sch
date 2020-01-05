EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L tonuino:DFPlayer A2
U 1 1 5E0A67E8
P 4150 4050
F 0 "A2" H 3800 4600 50  0000 C CNN
F 1 "DFPlayer" H 4400 3550 50  0000 C CNN
F 2 "tonuino:DFPlayer" H 4140 4300 50  0001 C CNN
F 3 "" H 4140 4300 50  0001 C CNN
	1    4150 4050
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 5E0AA572
P 3450 3300
F 0 "R1" H 3380 3254 50  0000 R CNN
F 1 "1k" H 3380 3345 50  0000 R CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 3380 3300 50  0001 C CNN
F 3 "~" H 3450 3300 50  0001 C CNN
	1    3450 3300
	-1   0    0    1   
$EndComp
$Comp
L MCU_Module:Arduino_Nano_v2.x A1
U 1 1 5E011EDD
P 7050 3500
F 0 "A1" H 6700 4450 50  0000 C CNN
F 1 "Arduino_Nano_v2.x" H 7550 2550 50  0000 C CNN
F 2 "Module:Arduino_Nano" H 7050 3500 50  0001 C CIN
F 3 "https://www.arduino.cc/en/uploads/Main/ArduinoNanoManual23.pdf" H 7050 3500 50  0001 C CNN
	1    7050 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 3650 3450 3450
Wire Wire Line
	3450 3650 3650 3650
Wire Wire Line
	3450 3150 3450 2950
Wire Wire Line
	3450 2950 5150 2950
Wire Wire Line
	5150 2950 5150 3200
Wire Wire Line
	5150 3200 6550 3200
Wire Wire Line
	3650 3750 3350 3750
Wire Wire Line
	3350 3750 3350 2850
Wire Wire Line
	3350 2850 5250 2850
Wire Wire Line
	5250 2850 5250 3100
Wire Wire Line
	5250 3100 6550 3100
Wire Wire Line
	5150 3650 5150 3300
Wire Wire Line
	5150 3300 6550 3300
Wire Wire Line
	4150 3450 4150 3350
Wire Wire Line
	4150 3350 4200 3350
Wire Wire Line
	4250 3350 4250 3450
Wire Wire Line
	7150 4600 7150 4500
Wire Wire Line
	7050 4500 7050 4600
Wire Wire Line
	7050 4600 7100 4600
Wire Wire Line
	4650 3650 5150 3650
$Comp
L Device:R R2
U 1 1 5E0F9BC7
P 7850 1600
F 0 "R2" H 7920 1646 50  0000 L CNN
F 1 "4k7" H 7920 1555 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 7780 1600 50  0001 C CNN
F 3 "~" H 7850 1600 50  0001 C CNN
	1    7850 1600
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 5E0FA662
P 8300 1600
F 0 "R3" H 8370 1646 50  0000 L CNN
F 1 "4k7" H 8370 1555 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 8230 1600 50  0001 C CNN
F 3 "~" H 8300 1600 50  0001 C CNN
	1    8300 1600
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 5E0FAA68
P 8700 1600
F 0 "R4" H 8770 1646 50  0000 L CNN
F 1 "4k7" H 8770 1555 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 8630 1600 50  0001 C CNN
F 3 "~" H 8700 1600 50  0001 C CNN
	1    8700 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	7850 1850 7950 1850
Text Label 7950 1850 0    50   ~ 0
~PAUSE
Wire Wire Line
	8300 1850 8400 1850
Wire Wire Line
	8700 1850 8800 1850
Text Label 8400 1850 0    50   ~ 0
~UP
Text Label 8800 1850 0    50   ~ 0
~DOWN
Text Label 9500 3250 2    50   ~ 0
~PAUSE
Text Label 9500 2950 2    50   ~ 0
GND
Text Label 9500 3150 2    50   ~ 0
GND
Text Label 9500 3350 2    50   ~ 0
GND
Text Label 9500 3050 2    50   ~ 0
~UP
Text Label 9500 2850 2    50   ~ 0
~DOWN
Text Label 9500 3450 2    50   ~ 0
VCC
Text Label 9500 3650 2    50   ~ 0
LED1
Text Label 9500 3550 2    50   ~ 0
LED2
Text Label 7550 4000 0    50   ~ 0
LED0
Text Label 7550 4100 0    50   ~ 0
LED1
Text Label 7550 4200 0    50   ~ 0
LED2
Text Label 7550 3500 0    50   ~ 0
~PAUSE
Text Label 7550 3600 0    50   ~ 0
~UP
Text Label 7550 3700 0    50   ~ 0
~DOWN
Wire Wire Line
	8700 1850 8700 1750
Wire Wire Line
	8300 1850 8300 1750
Wire Wire Line
	7850 1850 7850 1750
Text Label 7850 1450 0    50   ~ 0
VCC
Text Label 8300 1450 0    50   ~ 0
VCC
Text Label 8700 1450 0    50   ~ 0
VCC
Text Label 9500 2650 2    50   ~ 0
VCC
Text Label 9500 3850 2    50   ~ 0
GND
Text Label 4200 3250 1    50   ~ 0
GND
Text Label 4050 3250 1    50   ~ 0
VCC
Text Label 7100 4700 2    50   ~ 0
GND
Text Label 7150 2500 1    50   ~ 0
3V3
Text Label 7250 2500 1    50   ~ 0
VCC
Text Label 6550 4200 2    50   ~ 0
SCK
Text Label 6550 3800 2    50   ~ 0
RST
Text Label 6550 3900 2    50   ~ 0
SS
Wire Wire Line
	7100 4600 7100 4700
Connection ~ 7100 4600
Wire Wire Line
	7100 4600 7150 4600
Wire Wire Line
	4200 3350 4200 3250
Connection ~ 4200 3350
Wire Wire Line
	4200 3350 4250 3350
Wire Wire Line
	4050 3250 4050 3450
Text Label 9500 2750 2    50   ~ 0
GND
Text Label 9500 3950 2    50   ~ 0
~HEADP
Text Label 6550 3500 2    50   ~ 0
~HEADP
Wire Wire Line
	6550 3400 5250 3400
Wire Wire Line
	5250 3750 4650 3750
Wire Wire Line
	5250 3400 5250 3750
Text Label 6800 5350 1    50   ~ 0
3V3
Text Label 7000 5350 1    50   ~ 0
GND
Text Label 7500 5350 1    50   ~ 0
SS
Text Label 7400 5350 1    50   ~ 0
SCK
Text Label 6900 5350 1    50   ~ 0
RST
Text Label 7300 5350 1    50   ~ 0
MOSI
Text Label 7200 5350 1    50   ~ 0
MISO
Text Label 6550 4100 2    50   ~ 0
MISO
Text Label 6550 4000 2    50   ~ 0
MOSI
$Comp
L Connector:Conn_01x08_Female J3
U 1 1 5E0B651E
P 7100 5550
F 0 "J3" V 7173 5480 50  0000 C CNN
F 1 "Conn_01x08_Female" V 7264 5480 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x08_P2.54mm_Horizontal" H 7100 5550 50  0001 C CNN
F 3 "~" H 7100 5550 50  0001 C CNN
	1    7100 5550
	0    -1   1    0   
$EndComp
Text Label 4650 4250 0    50   ~ 0
HEADP_L
Text Label 4650 4350 0    50   ~ 0
HEADP_R
Text Label 4650 4150 0    50   ~ 0
SPK+
Text Label 4650 4050 0    50   ~ 0
SPK-
Text Label 9500 4150 2    50   ~ 0
HEADP_L
Text Label 9500 4050 2    50   ~ 0
HEADP_R
Text Label 9500 4350 2    50   ~ 0
SPK-
Text Label 9500 4250 2    50   ~ 0
SPK+
$Comp
L Connector:Conn_01x18_Female J1
U 1 1 5E170684
P 9700 3450
F 0 "J1" H 9728 3426 50  0000 L CNN
F 1 "Conn_01x18_Female" H 9728 3335 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x18_P2.54mm_Vertical" H 9700 3450 50  0001 C CNN
F 3 "~" H 9700 3450 50  0001 C CNN
	1    9700 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R5
U 1 1 5E17F92A
P 9150 1600
F 0 "R5" H 9220 1646 50  0000 L CNN
F 1 "4k7" H 9220 1555 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 9080 1600 50  0001 C CNN
F 3 "~" H 9150 1600 50  0001 C CNN
	1    9150 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 1850 9250 1850
Wire Wire Line
	9150 1850 9150 1750
Text Label 9150 1450 0    50   ~ 0
VCC
Text Label 9250 1850 0    50   ~ 0
~HEADP
Text Label 9500 3750 2    50   ~ 0
LED0
$Comp
L Device:R R6
U 1 1 5E19BC34
P 2100 6600
F 0 "R6" H 2030 6554 50  0000 R CNN
F 1 "150R" H 2030 6645 50  0000 R CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 2030 6600 50  0001 C CNN
F 3 "~" H 2100 6600 50  0001 C CNN
	1    2100 6600
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3400 5900 3450 5900
Wire Wire Line
	3400 5450 3400 5900
Wire Wire Line
	3000 5450 3400 5450
Wire Wire Line
	2100 6750 3250 6750
Wire Wire Line
	3250 6200 3450 6200
Wire Wire Line
	3250 6750 3250 6200
Wire Wire Line
	2100 6250 3150 6250
Wire Wire Line
	3150 6100 3450 6100
Wire Wire Line
	3150 6250 3150 6100
Wire Wire Line
	2100 5750 3300 5750
Wire Wire Line
	3300 6000 3450 6000
Wire Wire Line
	3300 5750 3300 6000
Connection ~ 3000 5450
Wire Wire Line
	2650 5450 3000 5450
Wire Wire Line
	3000 5950 3000 5450
Wire Wire Line
	3000 6450 3000 5950
Connection ~ 3000 5950
Wire Wire Line
	2650 5950 3000 5950
Wire Wire Line
	2650 6450 3000 6450
$Comp
L Connector:Conn_01x04_Female J2
U 1 1 5E1A5A96
P 3650 6100
F 0 "J2" H 3678 6076 50  0000 L CNN
F 1 "Conn_01x04_Female" H 3678 5985 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x02_P2.54mm_Vertical" H 3650 6100 50  0001 C CNN
F 3 "~" H 3650 6100 50  0001 C CNN
	1    3650 6100
	1    0    0    1   
$EndComp
Wire Wire Line
	2100 6450 2350 6450
Wire Wire Line
	2350 5950 2100 5950
Wire Wire Line
	2350 5450 2100 5450
$Comp
L Device:R R8
U 1 1 5E19E525
P 2100 5600
F 0 "R8" H 2030 5554 50  0000 R CNN
F 1 "150R" H 2030 5645 50  0000 R CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 2030 5600 50  0001 C CNN
F 3 "~" H 2100 5600 50  0001 C CNN
	1    2100 5600
	-1   0    0    -1  
$EndComp
$Comp
L Device:R R7
U 1 1 5E19DD89
P 2100 6100
F 0 "R7" H 2030 6054 50  0000 R CNN
F 1 "150R" H 2030 6145 50  0000 R CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 2030 6100 50  0001 C CNN
F 3 "~" H 2100 6100 50  0001 C CNN
	1    2100 6100
	-1   0    0    -1  
$EndComp
$Comp
L Device:LED D3
U 1 1 5E19ABD7
P 2500 5450
F 0 "D3" H 2493 5666 50  0000 C CNN
F 1 "LED" H 2493 5575 50  0000 C CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 2500 5450 50  0001 C CNN
F 3 "~" H 2500 5450 50  0001 C CNN
	1    2500 5450
	1    0    0    1   
$EndComp
$Comp
L Device:LED D2
U 1 1 5E19A473
P 2500 5950
F 0 "D2" H 2493 6166 50  0000 C CNN
F 1 "LED" H 2493 6075 50  0000 C CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 2500 5950 50  0001 C CNN
F 3 "~" H 2500 5950 50  0001 C CNN
	1    2500 5950
	1    0    0    1   
$EndComp
$Comp
L Device:LED D1
U 1 1 5E199C58
P 2500 6450
F 0 "D1" H 2493 6666 50  0000 C CNN
F 1 "LED" H 2493 6575 50  0000 C CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 2500 6450 50  0001 C CNN
F 3 "~" H 2500 6450 50  0001 C CNN
	1    2500 6450
	1    0    0    1   
$EndComp
$EndSCHEMATC
