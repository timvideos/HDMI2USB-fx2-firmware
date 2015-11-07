
2.4.2.2 Status Interrupt Endpoint

A USB VideoControl interface can support an optional interrupt endpoint to
inform the Host about the status of the different addressable entities
(Terminals, Units, interfaces and endpoints) inside the video function. The
interrupt endpoint, if present, is used by the entire Video Interface
Collection to convey status information to the Host. It is considered part of
the VideoControl interface because this is the anchor interface for the
Collection.

This interrupt endpoint is mandatory if:
 * The device implements any AutoUpdate controls (controls supporting device
   initiated changes).

 * The device implements any Asynchronous controls (see section 2.4.4, "Control
   Transfer and Request Processing").

When a dynamic format change event occurs, the following steps take place:
 * Device detects dynamic format change (while streaming is occurring).

 * Device begins sending empty data payloads to the host with the Error bit set
   in the video stream payload header.

 * Device sets the Stream Error Code Control to "Format Change" (see section
   4.3.1.7 "Stream Error Code Control").

 * The host queries the new stream state through a VS_PROBE_CONTROL request
   with the GET_CUR attribute (see 4.3.1.1, “Video Probe and Commit Controls”).

 * If the new format is acceptable by the host, it issues a VS_COMMIT_CONTROL
   request with the SET_CUR attribute and, if necessary, reallocates the USB
   bandwidth through an alternate interface selection standard request. If the
   new format is not acceptable, the host will negotiate a new format with the
   stream PROBE/COMMIT controls.

------------------

Use the following bits to control the HDMI2USB

3 x Camera Units (Input Terminals)

 - HDMI Input 1
 - HDMI Input 2
 - Test Pattern

3 x Selector Units

 - Selector for Encoder
 - Selector for HDMI Output 1
 - Selector for HDMI Output 2

 * Each selector has 3 input pins


3 x Output Terminals

 - Encoder Output
   - Has USB end points - videostream.header.bTerminalLink

 - HDMI Output 1
 - HDMI Output 2
   - Don't have USB end points..
   - OTT_DISPLAY  0x0301

1 x Processing Unit

1 x Encoding Unit


?? x Extension Units?

----

PIP Mode using this ->

4.2.2.1.19 Digital Window Control
The windowing API is based on “pixel” coordinates where each row and column of pixels on the
sensor can be referred to by integers between zero - (height-1) and zero- (width-1). The point at
0,0 is the top, left of the coordinate system and (height-1), (width-1) is the bottom, right of the
coordinates system.

----

HDMI Input using this ->

4.2.2.2 Selector Unit Control Requests
These requests are used to set or read an attribute of a Selector Control inside a Selector Unit of
the video function.

A Selector Unit represents a video stream source selector. The valid range for the CUR, MIN,
and MAX attributes is from one up to the number of Input Pins of the Selector Unit. This value
can be found in the bNrInPins field of the Selector Unit descriptor. The RES attribute can only
have a value of one.


				// FIXME: Add a "Selector Unit" which allows
				// selecting between HDMI inputs.
				// The Selector Unit (SU) selects from n input
				// data streams and routes them unaltered to
				// the single output stream.

----

Frequency control using this ->

4.2.2.3.6 Power Line Frequency Control
This control allows the host software to specify the local power line frequency, in order for the
device to properly implement anti-flicker processing, if supported. The default is
implementation-specific. This control must accept the GET_DEF request and return its default
value.

----

Report HDMI signal available on input using this -->

4.2.2.3.19 Analog Video Lock Status Control
This is used to report whether the video decoder has achieved horizontal lock of the analog input
signal. If the decoder is locked, it is assumed that a valid video stream is being generated. This
control is to be supported only for analog video decoder functionality.

----

				// FIXME: Add "Encoding Unit" for MJPEG control.
				// The Encoding Unit controls attributes of the
				// encoder that encodes the video being
				// streamed through it.

4.2.2.4 Encoding Units

 -- Quantization parameter
 -- 4.2.2.4.3 Video Resolution Control

