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
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <inttypes.h>
#include <stdbool.h>

#define UART_DISTANCE_ADDR_OFFSET 16

typedef struct Packet {
    char header;
    uint8_t addr;
    uint8_t data;
    uint8_t cr;
    uint8_t lf;
} Packet; 

int serial_device = 0;
int fd_RS232;

/* Qeueue stuff */
typedef struct Queue {
    uint8_t data[256];
    uint8_t first;
    uint8_t last;
    uint8_t count;
} Queue;

void queue_init(Queue *q) {
    q->last = 255;
    q->first = 0;
    q->count = 0;
}

void queue_enqueue(Queue *q, uint8_t d) {
    q->last = (q->last + 1) % 256;
    q->data[q->last] = d;
    q->count++;
}

uint8_t queue_dequeue(Queue *q) {
    uint8_t d = q->data[q->first];
    q->first = (q->first + 1) % 256;
    q->count--;
    return d;
}

/* RS232 stuff */
void rs232_open(void) {
    char *name;
    int result;
    struct termios tty;

    fd_RS232 = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);  // Hardcode your serial port here, or request it as an argument at runtime

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

void packet_print(struct Packet p) {
    printf("%d %d %d %d %d\n", p.header, p.addr, p.data, p.cr, p.lf);
}

/* packet stuff */
void packet_send(Packet *p) {
    p->cr = '\r';
    p->lf = '\n';
    rs232_putchar(p->header);
    rs232_putchar(p->addr);
    rs232_putchar(p->data);
    rs232_putchar(p->cr);
    rs232_putchar(p->lf);
}

bool packet_receive(Queue *q, uint8_t *address, uint8_t *data) {
    uint8_t header, cr, lf;
    bool ret;

    header = queue_dequeue(q);
    if (header == 's') {
        printf("Success :D\n");
        *address = queue_dequeue(q);
        *data = queue_dequeue(q);
        cr = queue_dequeue(q);
        lf = queue_dequeue(q);
        ret = true;
    }
    else if (header == 'f') {
        printf("nano reported fail");
        queue_dequeue(q);
        queue_dequeue(q);
        queue_dequeue(q);
        queue_dequeue(q);
        ret = true;
    }

    else {
        printf("Expected s, got %d\n", header);
        return false;
    }

    return true;
}

int time_diff_ms(struct timespec *latest, struct timespec *first) {
    int ds = latest->tv_sec - first->tv_sec;
    int dns = latest->tv_nsec - first->tv_nsec;
    return 1000*ds + dns/1000000;
}


int main() {
    Queue rxq;
    queue_init(&rxq);
    char c;
    uint8_t addr = 0;
    uint8_t addr_rec;
    uint8_t value;
    bool can_send = true;
    Packet p_send;

    struct timespec last_received;
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    clock_gettime(CLOCK_MONOTONIC_RAW, &last_received);

    rs232_open();
    sleep(1);
    
    while(true) {
        printf("Count: %d\n", rxq.count);
        if (rxq.count > 4) {
            if (packet_receive(&rxq, &addr_rec, &value)) {
                printf("Req: %d, got %d: %d\n", addr, addr_rec, value);
                addr = ((addr - UART_DISTANCE_ADDR_OFFSET + 1) % 11) + UART_DISTANCE_ADDR_OFFSET;
                can_send = true;
                clock_gettime(CLOCK_MONOTONIC_RAW, &last_received);
            }
            else {
                printf("Something went wrong..\n");
            }
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &now);
        if (time_diff_ms(&now, &last_received) > 1000) {
            can_send = true;
            printf("Force true\n");
        }

        if (can_send) {
            p_send.header = 'r';
            p_send.addr = addr;
            p_send.data = 0; 
            packet_send(&p_send);
            can_send = false;
        }
        
        while ((c = rs232_getchar_nb()) != -1) {
            queue_enqueue(&rxq, c);
        }
    }
}
