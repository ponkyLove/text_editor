#include <unistd.h> // provides posix APIs(read, write) and constants like STDIN/STDOUT
#include <stdio.h> // provides definitions for I/O operantins like printf and scanf
#include <termios.h> // provides definitions and functions for terminal management 
#include <stdlib.h> // C library providing, process control, mem alloc and other sting/maths functions
#include <errno.h> // errorno set by library functions

/** Data **/
struct termios orgi_termios; // declaring a terminos stucture

/** Terminal configuration**/
// prints descriptive message for errno set by lib before exit
void die(const char *s){
    perror(s); // stdio function 
    exit(1);
}

//reset terminal values back to oginal
void disableRawMode(){
    // TCSAFLUSH , flushes all output to terminal and discards all input
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orgi_termios)== -1) // Setting new terminal attributes
        die("tcssetattr");
}


//Configure terminal values using flags to set it to raw mode
void enableRawMode(){
    if (tcgetattr(STDIN_FILENO, &orgi_termios) == -1) // Getting terminal attributes
        die("tcgetattr");
    atexit(disableRawMode); // Registering a call back to disableRawMode at exit
    struct termios raw = orgi_termios;
    // modifying terminal local flags
    // ECHO - do not print inputs 
    // ICANON - read every key press, and not wait till \r\n
    // ISIG - Disable input interrupt signal liek Cntrl C/Z
    // IEXTEN - Disable Cntrl V
    raw.c_lflag &= ~(ECHO| ICANON |ISIG| IEXTEN); // removing option from terminal using bitwise not and
    // modifying terminal input flags
    // IXON - Stoping suspend and resum signals, Cntrl+S and Q
    // ICRNL- To stop terminal from convering \r (cntrl + M) to \n
    raw.c_iflag &= ~(IXON| ICRNL| ISTRIP| BRKINT| INPCK);
    // modifying terminal input flags
    // OPOST - remove all output processing like converting \n to \r\n
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag &= ~(CS8);
    // Terminal control commands
    // Minumum number of bytes for read to return
    raw.c_cc[VMIN] = 0; // Retrun immediately after timeout
    // Minimum wait time  before read timesout
    raw.c_cc[VTIME] = 1; // 100ms
    // TCSAFlUSH , discards all pending input before writing 
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) // Setting new terminal attributes
        die("tcsetattr");
}

/** Init **/
int main(){
    char c;
    enableRawMode();
    while (1){
        // Handling CYGIN terminal returning -1 for read timeouts with errorno EAGAIN
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
            die("read");
        if (c != 'q'){
            printf("0\n");
        }else{
            printf("bye\n");
            return 0;
        }
    }
}

