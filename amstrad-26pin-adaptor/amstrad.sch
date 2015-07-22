EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:amstrad-cache
EELAYER 25 0
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
L CONN_02X17 P1
U 1 1 55AF972A
P 4250 3650
F 0 "P1" H 4250 4550 50  0000 C CNN
F 1 "FLOPPY_34" V 4250 3650 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x17" H 4250 2550 60  0001 C CNN
F 3 "" H 4250 2550 60  0000 C CNN
	1    4250 3650
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X13 P2
U 1 1 55AF97E3
P 6550 3750
F 0 "P2" H 6550 4450 50  0000 C CNN
F 1 "FLOPPY_26" V 6550 3750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x13" H 6550 2600 60  0001 C CNN
F 3 "" H 6550 2600 60  0000 C CNN
	1    6550 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 2850 3400 2850
Wire Wire Line
	4000 2950 3400 2950
Wire Wire Line
	4000 3050 3400 3050
Wire Wire Line
	4000 3150 3400 3150
Wire Wire Line
	4000 3250 3400 3250
Wire Wire Line
	4000 3350 3400 3350
Wire Wire Line
	4000 3450 3400 3450
Wire Wire Line
	4000 3550 3400 3550
Wire Wire Line
	4000 3650 3400 3650
Wire Wire Line
	4000 3750 3400 3750
Wire Wire Line
	4000 3850 3400 3850
Wire Wire Line
	4000 3950 3400 3950
Wire Wire Line
	4000 4050 3400 4050
Wire Wire Line
	4000 4150 3400 4150
Wire Wire Line
	4000 4250 3400 4250
Wire Wire Line
	4000 4350 3400 4350
Wire Wire Line
	4000 4450 3400 4450
Wire Wire Line
	4500 3150 5100 3150
Wire Wire Line
	4500 3250 5100 3250
Wire Wire Line
	4500 3350 5100 3350
Wire Wire Line
	4500 3550 5100 3550
Wire Wire Line
	4500 3650 5100 3650
Wire Wire Line
	4500 3750 5100 3750
Wire Wire Line
	4500 3850 5100 3850
Wire Wire Line
	4500 3950 5100 3950
Wire Wire Line
	4500 4050 5100 4050
Wire Wire Line
	4500 4150 5100 4150
Wire Wire Line
	4500 4250 5100 4250
Wire Wire Line
	4500 4350 5100 4350
Wire Wire Line
	4500 4450 5100 4450
Wire Wire Line
	6300 3150 5800 3150
Wire Wire Line
	6300 3250 5800 3250
Wire Wire Line
	6300 3350 5800 3350
Wire Wire Line
	6300 3450 5800 3450
Wire Wire Line
	6300 3550 5800 3550
Wire Wire Line
	6300 3650 5800 3650
Wire Wire Line
	6300 3750 5800 3750
Wire Wire Line
	6300 3850 5800 3850
Wire Wire Line
	6300 3950 5800 3950
Wire Wire Line
	6300 4050 5800 4050
Wire Wire Line
	6300 4150 5800 4150
Wire Wire Line
	6300 4250 5800 4250
Wire Wire Line
	6300 4350 5800 4350
Wire Wire Line
	6800 3150 7300 3150
Wire Wire Line
	6800 3250 7300 3250
Wire Wire Line
	6800 3350 7300 3350
Wire Wire Line
	6800 3450 7300 3450
Wire Wire Line
	6800 3550 7300 3550
Wire Wire Line
	6800 3650 7300 3650
Wire Wire Line
	6800 3750 7300 3750
Wire Wire Line
	6800 3850 7300 3850
Wire Wire Line
	6800 3950 7300 3950
Wire Wire Line
	6800 4050 7300 4050
Wire Wire Line
	6800 4150 7300 4150
Wire Wire Line
	6800 4250 7300 4250
Wire Wire Line
	6800 4350 7300 4350
Wire Wire Line
	3400 4450 3400 2850
Connection ~ 3400 2950
Connection ~ 3400 3050
Connection ~ 3400 3150
Connection ~ 3400 3250
Connection ~ 3400 3350
Connection ~ 3400 3450
Connection ~ 3400 3550
Connection ~ 3400 3650
Connection ~ 3400 3750
Connection ~ 3400 3850
Connection ~ 3400 3950
Connection ~ 3400 4050
Connection ~ 3400 4150
Connection ~ 3400 4250
Connection ~ 3400 4350
Wire Wire Line
	5800 4350 5800 3150
Connection ~ 5800 3250
Connection ~ 5800 3350
Connection ~ 5800 3450
Connection ~ 5800 3550
Connection ~ 5800 3650
Connection ~ 5800 3750
Connection ~ 5800 3850
Connection ~ 5800 3950
Connection ~ 5800 4050
Connection ~ 5800 4150
Connection ~ 5800 4250
Text Label 5850 3150 0    60   ~ 0
GND
Text Label 3450 2850 0    60   ~ 0
GND
Text Label 4600 3150 0    60   ~ 0
IDX
Text Label 4600 3250 0    60   ~ 0
SEL0
Text Label 4600 3350 0    60   ~ 0
SEL1
Text Label 4600 3550 0    60   ~ 0
MTR
Text Label 4600 4050 0    60   ~ 0
TRK0
Text Label 4600 3650 0    60   ~ 0
DIR
Text Label 4600 3750 0    60   ~ 0
STEP
Text Label 4600 3950 0    60   ~ 0
WGATE
Text Label 4600 3850 0    60   ~ 0
WDATA
Text Label 4600 4150 0    60   ~ 0
WRPROT
Text Label 4600 4250 0    60   ~ 0
RDATA
Text Label 4600 4350 0    60   ~ 0
SIDE
Text Label 4600 4450 0    60   ~ 0
RDY
NoConn ~ 4500 2850
NoConn ~ 4500 2950
NoConn ~ 4500 3050
Text Label 6850 3150 0    60   ~ 0
IDX
Text Label 6850 3250 0    60   ~ 0
SEL0
Text Label 6850 3350 0    60   ~ 0
SEL1
Text Label 6850 3450 0    60   ~ 0
MTR
Text Label 6850 3550 0    60   ~ 0
DIR
Text Label 6850 3650 0    60   ~ 0
STEP
Text Label 6850 3750 0    60   ~ 0
WDATA
Text Label 6850 3850 0    60   ~ 0
WGATE
Text Label 6850 3950 0    60   ~ 0
TRK0
Text Label 6850 4050 0    60   ~ 0
WRPROT
Text Label 6850 4150 0    60   ~ 0
RDATA
Text Label 6850 4250 0    60   ~ 0
SIDE
Text Label 6850 4350 0    60   ~ 0
RDY
NoConn ~ 4500 3450
$EndSCHEMATC
