// address
#define VIDEOCORE_MBOX      (volatile unsigned int*) 0x3F00B880
#define MBOX_READ           (volatile unsigned int*) 0x3F00B880
#define MBOX_POLL           (volatile unsigned int*) 0x3F00B890
#define MBOX_SENDER         (volatile unsigned int*) 0x3F00B894
#define MBOX_STATUS         (volatile unsigned int*) 0x3F00B898
#define MBOX_CONFIG         (volatile unsigned int*) 0x3F00B89C
#define MBOX_WRITE          (volatile unsigned int*) 0x3F00B8A0
#define MBOX_RESPONSE       0x80000000
#define MBOX_FULL           0x80000000
#define MBOX_EMPTY          0x40000000

#define MBOX_REQUEST        0

typedef enum
{
    MBOX_CH_POWER = 0,
    MBOX_CH_FRAMEBUFFER,
    MBOX_CH_VIRTUALUART,
    MBOX_CH_VCHIQ,
    MBOX_CH_LEDS,
    MBOX_CH_BUTTONS,
    MBOX_CH_TOUCHSCREEN,
    MBOX_CH_PROP_ARM2VC = 8,
    MBOX_CH_PROP_VC2ARM = 9,
} mailbox_channel_t;

typedef enum
{
    // VideoCore
    MBOX_TAG_GET_FirmwareRevision = 0x00001,
    // Hardware
    MBOX_TAG_GET_BoardModel = 0x10001,
    MBOX_TAG_GET_BoardRevision = 0x10002,
    MBOX_TAG_GET_BoardMAC = 0x10003,
    MBOX_TAG_GET_BoardSerial = 0x10004,
    MBOX_TAG_GET_ARMMemory = 0x10005,
    MBOX_TAG_GET_VCMemory = 0x10006,
    MBOX_TAG_GET_Clocks = 0x10007,
    // Config
    MBOX_TAG_GET_CommandLine = 0x50001,
    // Shared resource management
    MBOX_TAG_GET_DMAChannels = 0x60001,
    // Power
    MBOX_TAG_GET_PowerState = 0x20001,
    MBOX_TAG_GET_Timing = 0x20002,
    MBOX_TAG_SET_PowerState = 0x28001,
    // Clocks
    MBOX_TAG_GET_ClockState = 0x30001,
    MBOX_TAG_SET_ClockState = 0x38002,
    MBOX_TAG_GET_ClockRate = 0x30002,
    MBOX_TAG_SET_ClockRate = 0x38002,
    MBOX_TAG_GET_MaxClockRate = 0x30004,
    MBOX_TAG_GET_MinClockRate = 0x30007,
    MBOX_TAG_GET_Turbo = 0x30009,
    MBOX_TAG_SET_Turbo = 0x38009,
    // Voltage
    MBOX_TAG_GET_Voltage = 0x30003,
    MBOX_TAG_SET_Voltage = 0x38003,
    MBOX_TAG_GET_MaxVoltage = 0x30005,
    MBOX_TAG_GET_MinVoltage = 0x30008,
    MBOX_TAG_GET_Temperature = 0x30006,
    MBOX_TAG_GET_MaxTemperature = 0x3000A,
    MBOX_TAG_AllocateMemory = 0x3000C,
    MBOX_TAG_LockMemory = 0x3000D,
    MBOX_TAG_UnlockMemory = 0x3000E,
    MBOX_TAG_ReleaseMemory = 0x3000F,
    MBOX_TAG_ExecuteCode = 0x30010,
    MBOX_TAG_Get_Dispmanx_Resource_mem_handle = 0x30014,
    MBOX_TAG_Get_EDIDBlock = 0x30020,
    // Frame Buffer
    MBOX_TAG_AllocateBuffer = 0x40001,
    MBOX_TAG_ReleaseBuffer = 0x48001,
    MBOX_TAG_BlankBuffer = 0x40002,
    MBOX_TAG_Get_PhysicalWH = 0x40003,
    MBOX_TAG_Test_PhysicalWH = 0x44003,
    MBOX_TAG_Set_PhysicalWH = 0x48003,
    MBOX_TAG_Get_VirtualWH = 0x40004,
    MBOX_TAG_Test_VirtualWH = 0x44004,
    MBOX_TAG_Set_VirtualWH = 0x48004,
    MBOX_TAG_Get_Depth = 0x40005,
    MBOX_TAG_Test_Depth = 0x44005,
    MBOX_TAG_Set_Depth = 0x48005,
    MBOX_TAG_Get_PixelOrder = 0x40006,
    MBOX_TAG_Test_PixelOrder = 0x44006,
    MBOX_TAG_Set_PixelOrder = 0x48006,
    MBOX_TAG_Get_AlphaMode = 0x40007,
    MBOX_TAG_Test_AlphaMode = 0x44007,
    MBOX_TAG_Set_AlphaMode = 0x48007,
    MBOX_TAG_Get_Pitch = 0x40008,
    MBOX_TAG_Get_VirtualOffset = 0x40009,
    MBOX_TAG_Test_VirtualOffset = 0x44009,
    MBOX_TAG_Set_VirtualOffset = 0x48009,
    MBOX_TAG_Get_Overscan = 0x4000A,
    MBOX_TAG_Test_Overscan = 0x4400A,
    MBOX_TAG_Set_Overscan = 0x4800A,
    MBOX_TAG_Get_Palette = 0x4000B,
    MBOX_TAG_Test_Palette = 0x4400B,
    MBOX_TAG_Set_Palette = 0x4800B,
    MBOX_TAG_Set_CursorInfo = 0x08010,
    MBOX_TAG_Set_CursorState = 0x08011,

    MBOX_TAG_LAST = 0,
} mailbox_tag_t;

// mailbox message buffer
extern volatile unsigned int mailbuffer[256];

int _mailbox_call(mailbox_channel_t channel);