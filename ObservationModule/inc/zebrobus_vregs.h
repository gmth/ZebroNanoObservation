#ifndef ZEBROBUS_VREGS_H
#define ZEBROBUS_VREGS_H

#define ZEBROBUS_OBS_MASTER_READ 			0xF0
#define ZEBROBUS_OBS_MASTER_WRITE 			0x0F
#define ZEBROBUS_OBS_RESET 					0xFF

/* ZEBROBUS COMMONS */
#define ZEBROBUS_VERSION			0x00
#define ZEBROBUS_CLASS_ID			0x01
#define ZEBROBUS_PRODUCT_ID			0x02
#define ZEBROBUS_PRODUCT_VERSION	0x03
#define ZEBROBUS_SERIAL_ID			0x04
#define ZEBROBUS_SOFTWARE_VERSION	0x05
#define ZEBROBUS_ADDRESS			0x06

#define ZEBROBUS_QUICKSTATUS		0x10
#define ZEBROBUS_SYNC_CTR			0x11
#define ZEBROBUS_LOOP_CTR			0x12
#define ZEBROBUS_ERROR_CTR			0x13
#define ZEBROBUS_LAST_ERROR			0x14

/* DEVICE CLASSES */
#define ZEBROBUS_CLASS_NOTIMPLEMENTED	0x00
#define ZEBROBUS_CLASS_LEG				0x01
#define ZEBROBUS_CLASS_LOCOMOTION		0x02
#define ZEBROBUS_CLASS_OBSERVATION		0x03
#define ZEBROBUS_CLASS_LOCALISATION		0x04
#define ZEBROBUS_CLASS_COMMUNICATION	0x05

/* DEVICE IDS */
#define ZEBROBUS_DEVICE_LEG_DANIEL						0x01
#define ZEBROBUS_DEVICE_LOCOMOTION_LAURENS				0x01
#define ZEBROBUS_DEVICE_OBSERVATIONHEAD_THIJS			0x01
#define ZEBROBUS_DEVICE_OBSERVATIONINTEGRATED1_THIJS	0x02
#define ZEBROBUS_DEVICE_LOCALISATION_THIJS				0x01
#define ZEBROBUS_DEVICE_COMMUNICATION_THIJS				0x01

/* DEVICE ADDRESSES */
#define ZEBROBUS_ADDRESS_LEG			0x02
#define ZEBROBUS_ADDRESS_LOCOMOTION		0x03
#define ZEBROBUS_ADDRESS_OBSERVATION	0x04
#define ZEBROBUS_ADDRESS_LOCALISATION	0x05
#define ZEBROBUS_ADDRESS_COMMUNICATION	0x06

#define ZEBROBUS_QUEUESIZE				256

#define VREGS_OBS_VERSION			ZEBROBUS_VERSION			
#define VREGS_OBS_CLASS_ID			ZEBROBUS_CLASS_ID			
#define VREGS_OBS_PRODUCT_ID		ZEBROBUS_PRODUCT_ID			
#define VREGS_OBS_PRODUCT_VERSION	ZEBROBUS_PRODUCT_VERSION	
#define VREGS_OBS_SERIAL_ID			ZEBROBUS_SERIAL_ID			
#define VREGS_OBS_SOFTWARE_VERSION	ZEBROBUS_SOFTWARE_VERSION	
#define VREGS_OBS_ADDRESS			ZEBROBUS_ADDRESS			

#define VREGS_OBS_QUICKSTATUS		ZEBROBUS_QUICKSTATUS		
#define VREGS_OBS_SYNC_CTR			ZEBROBUS_SYNC_CTR			
#define VREGS_OBS_LOOP_CTR			ZEBROBUS_LOOP_CTR			
#define VREGS_OBS_ERROR_CTR			ZEBROBUS_ERROR_CTR			
#define VREGS_OBS_LAST_ERROR		ZEBROBUS_LAST_ERROR			


/* Communication settings */
#define VREGS_OBS_SETTINGS			0x20
#define VREGS_OBS_SYSCTRL			0x21

// distance measurements, assuming the 11 long distance array
// for compatibility between observation modules
#define VREGS_OBS_DISTANCE0 		0x30
#define VREGS_OBS_DISTANCE1 		0x31
#define VREGS_OBS_DISTANCE2 		0x32
#define VREGS_OBS_DISTANCE3 		0x33
#define VREGS_OBS_DISTANCE4 		0x34
#define VREGS_OBS_DISTANCE5 		0x35
#define VREGS_OBS_DISTANCE6 		0x36
#define VREGS_OBS_DISTANCE7 		0x37
#define VREGS_OBS_DISTANCE8 		0x38
#define VREGS_OBS_DISTANCE9 		0x39
#define VREGS_OBS_DISTANCE10 		0x3A
#define VREGS_OBS_INFRARED_L		0x3B
#define VREGS_OBS_INFRARED_H 		0x3C

#define VREGS_OBS_SERVO_NOSWEEP	0x40
#define VREGS_OBS_SERVO_SET0		0x41
#define VREGS_OBS_SERVO_SET1		0x42
#define VREGS_OBS_SERVO_SET2		0x43
#define VREGS_OBS_SERVO_SET3		0x44
#define VREGS_OBS_SERVO_SET4		0x45

// module specific raw measurments
#define VREGS_OBS_INFRAREDRAW_L 	0x50  
#define VREGS_OBS_INFRAREDRAW_H 	0x51  
#define VREGS_OBS_DISTANCERAW0		0x52  
#define VREGS_OBS_DISTANCERAW1		0x53  
#define VREGS_OBS_DISTANCERAW2		0x54  
#define VREGS_OBS_DISTANCERAW3		0x55  
#define VREGS_OBS_DISTANCERAW4		0x56  
#define VREGS_OBS_DISTANCERAW5		0x57  
#define VREGS_OBS_DISTANCERAW6		0x58  
#define VREGS_OBS_DISTANCERAW7		0x59  
#define VREGS_OBS_DISTANCERAW8		0x5A
#define VREGS_OBS_DISTANCERAW9		0x5B  
#define VREGS_OBS_DISTANCERAW10		0x5C  
#define VREGS_OBS_DISTANCERAW11		0x5D  
#define VREGS_OBS_DISTANCERAW12		0x5E  
#define VREGS_OBS_DISTANCERAW13		0x5F  

#define VREGS_OBS_ZEBROBUS_RESET	0xFF  

#endif
