#ifndef UAC_DEFS_H
#define UAC_DEFS_H

/* USB Audio Class */

#include <fx2lib.h>
#include <fx2usb.h>

// TODO: this has been extracted from linux headers, but should be organised better,
//       we also need one naming convention corresponding to that in libfx2
enum { 
  /* bInterfaceProtocol values to denote the version of the standard used */
  UAC_VERSION_1 = 0x00,
  UAC_VERSION_2 = 0x20,

  USB_CLASS_AUDIO = 0x01,

  /* A.2 Audio Interface Subclass Codes */
  USB_SUBCLASS_AUDIOCONTROL = 0x01,
  USB_SUBCLASS_AUDIOSTREAMING = 0x02,
  USB_SUBCLASS_MIDISTREAMING = 0x03,

  /* A.5 Audio Class-Specific AC Interface Descriptor Subtypes */
  UAC_HEADER = 0x01,
  UAC_INPUT_TERMINAL = 0x02,
  UAC_OUTPUT_TERMINAL = 0x03,
  UAC_MIXER_UNIT = 0x04,
  UAC_SELECTOR_UNIT = 0x05,
  UAC_FEATURE_UNIT = 0x06,
  UAC1_PROCESSING_UNIT = 0x07,
  UAC1_EXTENSION_UNIT = 0x08,

  /* A.6 Audio Class-Specific AS Interface Descriptor Subtypes */
  UAC_AS_GENERAL = 0x01,
  UAC_FORMAT_TYPE = 0x02,
  UAC_FORMAT_SPECIFIC = 0x03,

  /* A.7 Processing Unit Process Types */
  UAC_PROCESS_UNDEFINED = 0x00,
  UAC_PROCESS_UP_DOWNMIX = 0x01,
  UAC_PROCESS_DOLBY_PROLOGIC = 0x02,
  UAC_PROCESS_STEREO_EXTENDER = 0x03,
  UAC_PROCESS_REVERB = 0x04,
  UAC_PROCESS_CHORUS = 0x05,
  UAC_PROCESS_DYN_RANGE_COMP = 0x06,

  /* A.8 Audio Class-Specific Endpoint Descriptor Subtypes */
  UAC_EP_GENERAL = 0x01,

  /* A.9 Audio Class-Specific Request Codes */
  UAC_SET_ = 0x00,
  UAC_GET_ = 0x80,

  UAC__CUR = 0x1,
  UAC__MIN = 0x2,
  UAC__MAX = 0x3,
  UAC__RES = 0x4,
  UAC__MEM = 0x5,

  UAC_SET_CUR = (UAC_SET_ | UAC__CUR),
  UAC_GET_CUR = (UAC_GET_ | UAC__CUR),
  UAC_SET_MIN = (UAC_SET_ | UAC__MIN),
  UAC_GET_MIN = (UAC_GET_ | UAC__MIN),
  UAC_SET_MAX = (UAC_SET_ | UAC__MAX),
  UAC_GET_MAX = (UAC_GET_ | UAC__MAX),
  UAC_SET_RES = (UAC_SET_ | UAC__RES),
  UAC_GET_RES = (UAC_GET_ | UAC__RES),
  UAC_SET_MEM = (UAC_SET_ | UAC__MEM),
  UAC_GET_MEM = (UAC_GET_ | UAC__MEM),

  UAC_GET_STAT = 0xff,

  /* A.10 Control Selector Codes */

  /* A.10.1 Terminal Control Selectors */
  UAC_TERM_COPY_PROTECT = 0x01,

  /* A.10.2 Feature Unit Control Selectors */
  UAC_FU_MUTE = 0x01,
  UAC_FU_VOLUME = 0x02,
  UAC_FU_BASS = 0x03,
  UAC_FU_MID = 0x04,
  UAC_FU_TREBLE = 0x05,
  UAC_FU_GRAPHIC_EQUALIZER = 0x06,
  UAC_FU_AUTOMATIC_GAIN = 0x07,
  UAC_FU_DELAY = 0x08,
  UAC_FU_BASS_BOOST = 0x09,
  UAC_FU_LOUDNESS = 0x0a,

  /* A.10.3.1 Up/Down-mix Processing Unit Controls Selectors */
  UAC_UD_ENABLE = 0x01,
  UAC_UD_MODE_SELECT = 0x02,

  /* A.10.3.2 Dolby Prologic (tm) Processing Unit Controls Selectors */
  UAC_DP_ENABLE = 0x01,
  UAC_DP_MODE_SELECT = 0x02,

  /* A.10.3.3 3D Stereo Extender Processing Unit Control Selectors */
  UAC_3D_ENABLE = 0x01,
  UAC_3D_SPACE = 0x02,

  /* A.10.3.4 Reverberation Processing Unit Control Selectors */
  UAC_REVERB_ENABLE = 0x01,
  UAC_REVERB_LEVEL = 0x02,
  UAC_REVERB_TIME = 0x03,
  UAC_REVERB_FEEDBACK = 0x04,

  /* A.10.3.5 Chorus Processing Unit Control Selectors */
  UAC_CHORUS_ENABLE = 0x01,
  UAC_CHORUS_LEVEL = 0x02,
  UAC_CHORUS_RATE = 0x03,
  UAC_CHORUS_DEPTH = 0x04,

  /* A.10.3.6 Dynamic Range Compressor Unit Control Selectors */
  UAC_DCR_ENABLE = 0x01,
  UAC_DCR_RATE = 0x02,
  UAC_DCR_MAXAMPL = 0x03,
  UAC_DCR_THRESHOLD = 0x04,
  UAC_DCR_ATTACK_TIME = 0x05,
  UAC_DCR_RELEASE_TIME = 0x06,

  /* A.10.4 Extension Unit Control Selectors */
  UAC_XU_ENABLE = 0x01,

  /* MIDI - A.1 MS Class-Specific Interface Descriptor Subtypes */
  UAC_MS_HEADER = 0x01,
  UAC_MIDI_IN_JACK = 0x02,
  UAC_MIDI_OUT_JACK = 0x03,

  /* MIDI - A.1 MS Class-Specific Endpoint Descriptor Subtypes */
  UAC_MS_GENERAL = 0x01,

  /* Terminals - 2.1 USB Terminal Types */
  UAC_TERMINAL_UNDEFINED = 0x100,
  UAC_TERMINAL_STREAMING = 0x101,
  UAC_TERMINAL_VENDOR_SPEC = 0x1FF,


  UAC_BCD_V10 = 0x100,
  UAC_CHANNEL_LEFT = 0x1,
  UAC_CHANNEL_RIGHT = 0x2,
  UAC_OUTPUT_TERMINAL_STREAMING = 0x101,

  USB_DT_CS_DEVICE = (USB_TYPE_CLASS | USB_DESC_DEVICE),
  USB_DT_CS_CONFIG = (USB_TYPE_CLASS | USB_DESC_CONFIGURATION),
  USB_DT_CS_STRING = (USB_TYPE_CLASS | USB_DESC_STRING),
  USB_DT_CS_INTERFACE = (USB_TYPE_CLASS | USB_DESC_INTERFACE),
  USB_DT_CS_ENDPOINT = (USB_TYPE_CLASS | USB_DESC_ENDPOINT),

  /* Terminals - 2.2 Input Terminal Types */
  UAC_INPUT_TERMINAL_UNDEFINED = 0x200,
  UAC_INPUT_TERMINAL_MICROPHONE = 0x201,
  UAC_INPUT_TERMINAL_DESKTOP_MICROPHONE = 0x202,
  UAC_INPUT_TERMINAL_PERSONAL_MICROPHONE = 0x203,
  UAC_INPUT_TERMINAL_OMNI_DIR_MICROPHONE = 0x204,
  UAC_INPUT_TERMINAL_MICROPHONE_ARRAY = 0x205,
  UAC_INPUT_TERMINAL_PROC_MICROPHONE_ARRAY = 0x206,

  /* Terminals - 2.3 Output Terminal Types */
  UAC_OUTPUT_TERMINAL_UNDEFINED = 0x300,
  UAC_OUTPUT_TERMINAL_SPEAKER = 0x301,
  UAC_OUTPUT_TERMINAL_HEADPHONES = 0x302,
  UAC_OUTPUT_TERMINAL_HEAD_MOUNTED_DISPLAY_AUDIO = 0x303,
  UAC_OUTPUT_TERMINAL_DESKTOP_SPEAKER = 0x304,
  UAC_OUTPUT_TERMINAL_ROOM_SPEAKER = 0x305,
  UAC_OUTPUT_TERMINAL_COMMUNICATION_SPEAKER = 0x306,
  UAC_OUTPUT_TERMINAL_LOW_FREQ_EFFECTS_SPEAKER = 0x307,

  /* Formats - A.1.1 Audio Data Format Type I Codes */
  UAC_FORMAT_TYPE_I_UNDEFINED = 0x0,
  UAC_FORMAT_TYPE_I_PCM = 0x1,
  UAC_FORMAT_TYPE_I_PCM8 = 0x2,
  UAC_FORMAT_TYPE_I_IEEE_FLOAT = 0x3,
  UAC_FORMAT_TYPE_I_ALAW = 0x4,
  UAC_FORMAT_TYPE_I_MULAW = 0x5,

  /* Formats - A.2 Format Type Codes */
  UAC_FORMAT_TYPE_UNDEFINED = 0x0,
  UAC_FORMAT_TYPE_I = 0x1,
  UAC_FORMAT_TYPE_II = 0x2,
  UAC_FORMAT_TYPE_III = 0x3,
  UAC_EXT_FORMAT_TYPE_I = 0x81,
  UAC_EXT_FORMAT_TYPE_II = 0x82,
  UAC_EXT_FORMAT_TYPE_III = 0x83,
};


// Macro to easily define typedefs for descriptor structures
#define USB_DESC_CONST_CODE_TYPEDEF(desc) \
    typedef __code const struct desc \
    desc ## _c;

/* 4.3.2  Class-Specific AC Interface Descriptor */
struct usb_desc_uac1_ac_header {
  uint8_t bLength;            /* 8 + n */
  uint8_t bDescriptorType;    /* USB_DT_CS_INTERFACE */
  uint8_t bDescriptorSubtype; /* UAC_MS_HEADER */
  uint16_t bcdADC;            /* 0x0100 */
  uint16_t wTotalLength;      /* includes Unit and Terminal desc. */
  uint8_t bInCollection;      /* n */
  uint8_t baInterfaceNr[];    /* [n] */
};

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uac1_ac_header)

/* 4.5.2 Class-Specific AS Interface Descriptor */
struct usb_desc_uac1_as_header {
  uint8_t bLength;            /* in bytes: 7 */
  uint8_t bDescriptorType;    /* USB_DT_CS_INTERFACE */
  uint8_t bDescriptorSubtype; /* AS_GENERAL */
  uint8_t bTerminalLink;      /* Terminal ID of connected Terminal */
  uint8_t bDelay;             /* Delay introduced by the data path */
  uint16_t wFormatTag;        /* The Audio Data Format */
};

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uac1_as_header)

struct usb_desc_uac1_output_terminal {
  uint8_t bLength;            /* in bytes: 9 */
  uint8_t bDescriptorType;    /* CS_INTERFACE descriptor type */
  uint8_t bDescriptorSubtype; /* OUTPUT_TERMINAL descriptor subtype */
  uint8_t bTerminalID;        /* Constant uniquely terminal ID */
  uint16_t wTerminalType;     /* USB Audio Terminal Types */
  uint8_t bAssocTerminal;     /* ID of the Input Terminal associated */
  uint8_t bSourceID;          /* ID of the connected Unit or Terminal*/
  uint8_t iTerminal;
};

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uac1_output_terminal)

/* Formats - A.1.1 Audio Data Format Type I Codes */
struct usb_desc_uac_format_type_i_continuous {
  uint8_t bLength;            /* in bytes: 8 + (ns * 3) */
  uint8_t bDescriptorType;    /* USB_DT_CS_INTERFACE */
  uint8_t bDescriptorSubtype; /* FORMAT_TYPE */
  uint8_t bFormatType;        /* FORMAT_TYPE_1 */
  uint8_t bNrChannels;        /* physical channels in the stream */
  uint8_t bSubframeSize;      /* */
  uint8_t bBitResolution;
  uint8_t bSamFreqType;
  uint8_t tLowerSamFreq[3];
  uint8_t tUpperSamFreq[3];
};
struct usb_desc_uac_format_type_i_discrete {
  uint8_t bLength;            /* in bytes: 8 + (ns * 3) */
  uint8_t bDescriptorType;    /* USB_DT_CS_INTERFACE */
  uint8_t bDescriptorSubtype; /* FORMAT_TYPE */
  uint8_t bFormatType;        /* FORMAT_TYPE_1 */
  uint8_t bNrChannels;        /* physical channels in the stream */
  uint8_t bSubframeSize;
  uint8_t bBitResolution;
  uint8_t bSamFreqType;
  uint8_t tSamFreq[][3];
};

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uac_format_type_i_continuous)
USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uac_format_type_i_discrete)

/* 4.3.2.1 Input Terminal Descriptor */
struct usb_desc_uac_input_terminal {
  uint8_t bLength;            /* in bytes: 12 */
  uint8_t bDescriptorType;    /* CS_INTERFACE descriptor type */
  uint8_t bDescriptorSubtype; /* INPUT_TERMINAL descriptor subtype */
  uint8_t bTerminalID;        /* Constant uniquely terminal ID */
  uint16_t wTerminalType;     /* USB Audio Terminal Types */
  uint8_t bAssocTerminal;     /* ID of the Output Terminal associated */
  uint8_t bNrChannels;        /* Number of logical output channels */
  uint16_t wChannelConfig;
  uint8_t iChannelNames;
  uint8_t iTerminal;
};

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uac_input_terminal)

/* Formats - A.2 Format Type Codes */
struct usb_desc_uac_iso_endpoint {
  uint8_t bLength;            /* in bytes: 7 */
  uint8_t bDescriptorType;    /* USB_DT_CS_ENDPOINT */
  uint8_t bDescriptorSubtype; /* EP_GENERAL */
  uint8_t bmAttributes;
  uint8_t bLockDelayUnits;
  uint16_t wLockDelay;
};

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uac_iso_endpoint)

/* USB_DT_ENDPOINT: Audio Endpoint descriptor */
struct usb_desc_audio_endpoint {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bEndpointAddress;
  uint8_t bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t bInterval;
  /* NOTE:  these two are _only_ in audio endpoints. */
  uint8_t bRefresh;
  uint8_t bSynchAddress;
};

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_audio_endpoint)

/* USB_DT_DEVICE_QUALIFIER: Device Qualifier descriptor */
struct usb_desc_dev_qualifier {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t bcdUSB;
  uint8_t bDeviceClass;
  uint8_t bDeviceSubClass;
  uint8_t bDeviceProtocol;
  uint8_t bMaxPacketSize0;
  uint8_t bNumConfigurations;
  uint8_t bRESERVED;
};

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_dev_qualifier)

#undef USB_DESC_CONST_CODE_TYPEDEF

#endif /* UAC_DEFS_H */
