// Reference and Credit:  https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
// Find out more on this great tutorial site!



// C library headers
#include <stdio.h>
#include <string.h>



// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()



int main() {

  // Open the serial port. 
  // Use "dmesg | grep tty" to check the IRQ and Port # of the real USB serial device
  int serial_port = open("/dev/ttyUSB0", O_RDWR);  
  
  // Other flags can be helpful too:  O_NONBLOCK for async comm.  
  // check https://www.gnu.org/software/libc/manual/html_node/Open_002dtime-Flags.html for the reference of all the available flags for opening a serial port



  // Write to serial port  (Challenge:  What exactly would be written to the port?)
  unsigned char msg[] = { 'H', 'e', 'l', 'l', 'o', '\r' };
  write(serial_port, "Hello, world!", sizeof(msg));



  // Allocate memory for read buffer, set size according to your needs
  char read_buf [256];
  
  


  // Normally you wouldn't do this memset() call, but since we will just receive
  // ASCII data for this example, we'll set everything to 0 so we can
  // call printf() easily.
  memset(&read_buf, '\0', sizeof(read_buf));
  
  

  // Read bytes. The behaviour of read() (e.g. does it block?,
  // how long does it block for?) 
  int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));



  // n is the number of bytes read. n may be 0 if no bytes were received, and can also be -1 to signal an error.
  if (num_bytes < 0) {
      printf("Error reading: %s", strerror(errno));
      return 1;
  }
  

  // Here we assume we received ASCII data, but you might be sending raw bytes (in that case, don't try and
  // print it to the screen like this!)
  printf("Read %i bytes. Received message: %s", num_bytes, read_buf);
  
  
  //Close the serial port (very much the same as the file handle)
  close(serial_port);
  
  
  return 0; // success
}
