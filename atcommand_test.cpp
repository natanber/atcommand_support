#include <stdio.h>
#include <string> 
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <semaphore.h> 

//using namespace std;
timer_t m_timerid;
int fd;
sem_t mutex;
char buf[1000]={"\0"}; 
int open_port(void)
{
  int fd; /* File descriptor for the port */

  fd = open("/dev/ttyAT", O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1)
  {
    perror("open_port: Unable to open /dev/ttyAT - ");
  }
  else
    fcntl(fd, F_SETFL, FNDELAY);

  printf ( "In Open port fd = %i\n", fd); 
  return (fd);
}
static void son_timer(union sigval val)
{
   printf("son_timer time out\n"); 
   printf("use\n"); 
   printf("cat /dev/ttyAT\n");
   fflush(stdout);
   close( fd );
   exit(0);
};
void start_timer(int delay_ms,bool periodic = false)
{
	struct sigevent sev;

   sev.sigev_notify = SIGEV_THREAD;
   sev.sigev_notify_attributes = nullptr;
   sev.sigev_value.sival_ptr = nullptr;
   sev.sigev_notify_function = son_timer;
   int res = timer_create(CLOCK_BOOTTIME, &sev, &m_timerid);
   printf ("start_timer delay_ms = %d res = %d\n",delay_ms,res); 
   if (res == 0)
   {
      struct itimerspec tspec;
      timespec *delay = &tspec.it_value;
      timespec *interval = &tspec.it_interval;

      delay->tv_sec = delay_ms / 1000;
      delay->tv_nsec = (delay_ms % 1000) * 1000000;

      if (periodic)
         *interval = *delay;
      else {
         interval->tv_nsec = 0;
         interval->tv_sec = 0;
      }
      printf ("start_timer delay_ms = %d\n",delay_ms); 
      timer_settime(m_timerid, 0, &tspec, nullptr);     
   }
}
void *get_answer_at_command(void *threadid)
{
   printf ("get_answer_at_command fd = %d\n",fd);
   while (1) 
   {
      int n = read( fd, buf, sizeof(buf) );
      if(n>0)
      {   
         printf("%s", buf);    
         fflush(stdout);
         close( fd );
         sem_post(&mutex); 
      }
   }  
   sem_post(&mutex); 
}
int main(int argc, char* argv[])
{
   int n,i;   
   //char com[]={"at!gstatus?\r"}; 
   
	if (argc != 2)
   {
		printf("check argumensts\n");
      printf("example - ./atcommand_test at!gstatus?\n");
		exit(0);
	}
   sem_init(&mutex, 0, 0);
   printf("cat /dev/ttyAT\n");
   std::string com;
   com = argv[1];
   com.append("\n\r");
   start_timer(5000,false);
   fd = open_port();
   pthread_t thread;
   int rc = pthread_create(&thread, NULL, get_answer_at_command, nullptr);

   // Read the configureation of the port

   struct termios options;
   tcgetattr( fd, &options );

   /* SEt Baud Rate */

   cfsetispeed( &options, B115200 );
   cfsetospeed( &options, B115200 );

   //I don't know what this is exactly

   options.c_cflag |= ( CLOCAL | CREAD );

   // Set the Charactor size

   options.c_cflag &= ~CSIZE; /* Mask the character size bits */
   options.c_cflag |= CS8;    /* Select 8 data bits */

   // Set parity - No Parity (8N1)

   options.c_cflag &= ~PARENB;
   options.c_cflag &= ~CSTOPB;
   options.c_cflag &= ~CSIZE;
   options.c_cflag |= CS8;

   options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

   // Disable Software Flow control

   options.c_iflag &= ~(IXON | IXOFF | IXANY);
   options.c_oflag &= ~OPOST;

   if ( tcsetattr( fd, TCSANOW, &options ) == -1 )
      printf ("1Error with tcsetattr = %s\n", strerror ( errno ) );
   else
      printf ( "%s\n", "tcsetattr succeed" );

   fcntl(fd, F_SETFL, FNDELAY);


   printf ("\n com  = %s \n len = %d\n", com.c_str(), com.size());
   n = write(fd, com.c_str(), com.size());
   if (n < 0)
      fputs("write() of 4 bytes failed!\n", stderr);
   else
      printf ("Write succeed n  = %i\n", n );

   n=0;
   i=0;
#if 0   
   while (1) 
   {
      sleep(1);
      n = read( fd, buf, sizeof(buf) );
     
      if(n>0)
      {   
         printf("%s", buf);    
         fflush(stdout);
         close( fd );
         return 0;
      }
   }
#endif
   printf ("before sem_wait\n");
   sem_wait(&mutex); 
   printf ("after sem_wait\n");
   close( fd );
   return 0;
}
