#ifndef _MACROS_
#define _MACROS_

#define BAUDRATE B38400

#define FLAG                0x7E
#define TRANSMITTER_A       0x03    //Command sent by Transmitter
#define RECEIVER_A          0X01    //Command sent by Receiver
#define SET                 0x3 
#define UA                  0x7
#define DISC 0xB
#define C_RR0 0x05
#define C_RR1 0x85
#define C_REJ0 0x01
#define C_REJ1 0x81
#define C10 0x00
#define C11 0x40
#define BCC(A, C) (A ^ C)
#define C_S(n) ((n % 2) << 6)

#define FALSE 0
#define TRUE 1

typedef enum
{
    START,
    FLAG_RCV,
    A_RCV,

    C_RCV,
    BCC_OK,
    S_MSG_RCV,

    I_RCV,
    DATA_RCV,
    INFO_MSG,

    ERROR,
    STOP
} state_info;


#endif // _MACROS_