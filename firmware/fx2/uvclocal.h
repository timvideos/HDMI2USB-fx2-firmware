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

#define CAMERA_UNIT_ID 1
#define PROCESSING_UNIT_ID 2
#define EXTENSION_UNIT_ID 3
#define OUTPUT_UNIT_ID 4

#endif
