/*------------------------------------------------------------
 * Serial I/O
 * 8 bits, 1 stopbit, no parity,
 * 38,400 baud
 *------------------------------------------------------------
 */
#include <termios.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <inttypes.h>
#include <stdbool.h>

struct Packet {
    uint8_t header;
    uint8_t addr;
    uint8_t data;
    uint8_t cr;
    uint8_t lf;
}; 
    

int serial_device = 0;
int fd_RS232;
struct Packet packet_in;
struct Packet packet_out;

void rs232_open(void) {
    char *name;
    int result;
    struct termios tty;

    fd_RS232 = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY);  // Hardcode your serial port here, or request it as an argument at runtime

    assert(fd_RS232>=0);

    result = isatty(fd_RS232);
    assert(result == 1);

    name = ttyname(fd_RS232);
    assert(name != 0);

    result = tcgetattr(fd_RS232, &tty);
    assert(result == 0);

    tty.c_iflag = IGNBRK; /* ignore break condition */
    tty.c_oflag = 0;
    tty.c_lflag = 0;

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; /* 8 bits-per-character */
    tty.c_cflag |= CLOCAL | CREAD; /* Ignore model status + read input */

    cfsetospeed(&tty, B38400);
    cfsetispeed(&tty, B38400);

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 1; // added timeout

    tty.c_iflag &= ~(IXON|IXOFF|IXANY);

    result = tcsetattr (fd_RS232, TCSANOW, &tty); /* non-canonical */

    tcflush(fd_RS232, TCIOFLUSH); /* flush I/O buffer */
}


void rs232_close(void) {
    int result;

    result = close(fd_RS232);
    assert (result==0);
}


int rs232_getchar_nb() {
    int result;
    unsigned char c;

    result = read(fd_RS232, &c, 1);

    if (result == 0) {
        return -1;
    }

    else {
        assert(result == 1);
        return (int) c;
    }
}


int rs232_getchar() {
    int c;

    while ((c = rs232_getchar_nb()) == -1) {
        ;
    }
    return c;
}


int rs232_putchar(char c) {
    int result;

    do {
        result = (int) write(fd_RS232, &c, 1);
    } while (result == 0);

    assert(result == 1);
    return result;
}


void packet_send() {
    packet_out.cr = '\r';
    packet_out.lf = '\n';
    rs232_putchar(packet_out.header);
    rs232_putchar(packet_out.addr);
    rs232_putchar(packet_out.data);
    rs232_putchar(packet_out.cr);
    rs232_putchar(packet_out.lf);
}

bool packet_receive() {
    packet_in.header = rs232_getchar_nb();
    bool ret = false;

    if (packet_in.header == 's') {
        packet_in.addr = rs232_getchar();
        packet_in.data = rs232_getchar();
        packet_in.cr = rs232_getchar();
        packet_in.lf = rs232_getchar();
        ret = true;
    }
    
    else if (packet_in.header == 'f') {
        packet_in.addr = rs232_getchar();
        packet_in.data = rs232_getchar();
        packet_in.cr = rs232_getchar();
        packet_in.lf = rs232_getchar();
        ret = true;
    }

    return ret;
}

int main() {
    uint8_t c;

    rs232_open();
    sleep(3);

    packet_out.header = 'r';
    packet_out.addr = 20;
    packet_out.data = 0;
    
//!     while(true) {
//!         c = rs232_getchar();
//!         printf("%d ", c);
//!         if (c == '0') {
//!             c = rs232_getchar();
//!             printf("%d ", c);
//!             if (c == '\r') {
//!                 c = rs232_getchar();
//!                 printf("%d ", c);
//!                 if (c == '\n') {
//!                     packet_send();
//!                 }
//!             }
//!         }
//!     }
    
    while(true) {
        packet_send();
        c = rs232_getchar();
        printf("char: %d \n", c);
        c = rs232_getchar();
        printf("char: %d \n", c);
        c = rs232_getchar();
        printf("char: %d \n", c);
        c = rs232_getchar();
        printf("char: %d \n", c);
        c = rs232_getchar();
        printf("char: %d \n", c);
        printf("\n");
        sleep(1);
    }

    rs232_close();
}
