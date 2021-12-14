#ifndef _DEFINES_
#define _DEFINES_

#define FLAG 0x7E
#define TRANSMITTER_A 0x03 //Command sent by Transmitter
#define RECEIVER_A 0X01    //Command sent by Receiver
#define CONTROL 0x1        //Data package control
#define BCC(A, C) (A ^ C) 
#define SET 0x3 
#define DISC 0xB
#define UA 0x7
#define C10 0x00
#define C11 0x40
#define ESC 0x7D
#define C_RR0 0x05
#define C_RR1 0x85
#define C_REJ0 0x01
#define C_REJ1 0x81
#define REC_REJ 0x03
// #define RR(n) (())
#define C_S(n) ((n % 2) << 6)

#define C_START 0x2
#define C_END 0x3

#define BIT(n) (0x01<<(n))
#define RR(r) 0x05 | (((r) % 2) ? BIT(7) : 0x00)
#define REJ(r) 0x01 | (((r) % 2) ? BIT(7) : 0x00)

#define T1 0X0 //File size
#define T2 0x1 //File name
#define V1 0X3
#define V2 0x10
#define MAX_FILE_SIZE 146431
#define MAX_FILE_NAME_SIZE 255

#define FALSE 0
#define TRUE 1

#define TIMEOUT 3
#define MAX_TRIES 5

#define FRAME_SIZE(packet_size) (2*(packet_size) + 6)
#define STUFF 0x20
#define STUFF_FLAG 0X5E
#define STUFF_ESC 0X5D
#define RCV_DISC 0X00

#define MAX_PACKET_SIZE 1024
#define STUFFED_MAX_SIZE MAX_PACKET_SIZE * 2

typedef enum
{
    START,
    FLAG_RCV,
    A_RCV,

    C_RCV,
    BCC_OK,
    MSG_RCV,

    INFO_RCV,
    DATA_RCV,

    ERROR,
    INFO_MSG,

    STOP
} information_state;

typedef struct
{
  information_state state;
  char addr;
  char ctrl;
  int index;
  int unstuffed_size;
  char unstuffed_msg[1024 + 1];
  char message[(1024 * 2) + 2];
} state_machine;


typedef enum{
  OPENING_CONNECTION,
  SENDING_DATA,
  CLOSING_CONNECTION
} program_state;


#endif // _DEFINES_