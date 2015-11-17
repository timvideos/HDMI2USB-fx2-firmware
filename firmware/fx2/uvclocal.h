#ifndef UVC_LOCAL_H
#define UVC_LOCAL_H

#define FRAME_INTERVAL_60FPS 166667	// 60.0fps ==  166666.6 ns
#define FRAME_INTERVAL_50FPS 200000	// 50.0fps ==  200000.0 ns
#define FRAME_INTERVAL_30FPS 333333	// 30.0fps ==  333333.3 ns
#define FRAME_INTERVAL_15FPS 666666	// 15.0fps ==  666666.6 ns
#define FRAME_INTERVAL_7FPS 1333334	//  7.5fps == 1333333.3 ns

//        .db 0x00,0x20,0x1C,0x00          ;/* Maximum video or still frame size in bytes */ 
// 1843200
#define FRAME_SIZE_1280x720 0x1C2000

//        .db 0x00,0x00,0x18,0x00          ;/* Maximum video or still frame size in bytes */
// 1572864
#define FRAME_SIZE_1024x768 0x180000

//        .db 0x00,0x00,0x00,0x0E          ;/* Min bit rate bits/s */ 
// 234881024
#define BIT_RATE 0xE000000

enum uvc_unit_ids {
	UNIT_ID_NONE = 0,
	UNIT_ID_CAMERA,
	UNIT_ID_PROCESSING,
	UNIT_ID_EXTENSION,
	UNIT_ID_OUTPUT,
	UNIT_ID_MAX,

	// Use in the future
	UNIT_ID_SELECTOR_4_ENCODER	= 0xff,
	UNIT_ID_ENCODER			= 0xff,

/*
	UNIT_ID_INPUT_PATTERN		= 0xff,
	UNIT_ID_INPUT_HDMI_0		= 0xff,
	UNIT_ID_INPUT_HDMI_1		= 0xff,
	UNIT_ID_SELECTOR_OUTPUT_0	= 0xff,
	UNIT_ID_SELECTOR_OUTPUT_1	= 0xff,
*/
};

#endif
