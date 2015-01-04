#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

// VirtualWire
#define VW_MAX_MESSAGE_LEN 30
#define VW_MAX_PAYLOAD VW_MAX_MESSAGE_LEN-3

// RF configuration
#define RF_ADDRESS 1

// The PiWeather board i2c address
#define ADDRESS 0x04

// The I2C bus: This is for V2 pi's. For V1 Model B you need i2c-0
static const char *devName = "/dev/i2c-1";

unsigned int power2(int power)
{
  unsigned int result = 1;
  int i;
  for (i = 0; i < power; i++)
    result *= 2;
  return result;
}

int itobb(int integer, unsigned char *buf)
{
  int i, length = 0;

  while (integer > (255 * power2(8 * length)))
      length++;
  length++;
  for (i = (length - 1); i >= 0; i--)
    buf[i] = (integer >> (i * 8)) & 0xFF;
  return length;
}

int i2cWrite(int fd, unsigned char *buf, int len)
{
  int i;
  if (write(fd, buf, len) == len)
  {
    printf("Sending ");
    for (i = 0; i < len; i++)
      printf("%u ", buf[i]);
    printf("\n");
  }
  else
    printf("i2cWrite: error\n");
}

int i2cRead(int fd, unsigned char *buf)
{
  int len = 0, i = 0;
  printf("Receiving length: ");
  if (read(fd, &buf[0], sizeof(unsigned char)) != 1)
    printf("error\n");
  else
  {
    printf("%u\n", buf[0]);
    if (read(fd, &buf[0], buf[0]) == buf[0])
    {
      printf("Received %u bytes: ", buf[0]);
      len = (int) buf[0];
      for (i = 0; i < len; i++)
        printf("%u ", buf[i]);
      printf("\n");
    }
    else
      printf("i2cRead: error\n");
  }
  return len;
}

int sendSignal(int fd, char *receiver, char *type, char *data)
{
  unsigned char signal[VW_MAX_MESSAGE_LEN + 1]; // 1 for the communication byte
  int signalLen = 0;
  unsigned int dataInt;

  if (atoi(receiver) > 255)
  {
    printf("Receiver address cannot be superior to 255\n");
    exit(1);
  }
  if (atoi(type) > 255)
  {
    printf("Type cannot be superior to 255\n");
    exit(1);
  }
  if (strlen(data) > (VW_MAX_PAYLOAD - 3)) // 3 because : receiver + emitter + type
  {
    printf("Data length cannot be superior to %i\n", (VW_MAX_PAYLOAD - 3));
    exit(1);
  }
  signal[0] = 0;
  signal[1] = (unsigned char) atoi(receiver);
  signal[2] = (unsigned char) RF_ADDRESS;
  signal[3] = (unsigned char) atoi(type);
  dataInt = (unsigned int) atoi(data);
  signalLen = 4 + itobb(dataInt, &signal[4]);
  i2cWrite(fd, &signal[0], signalLen);
}

int sendHomeEasySignal(int fd, char *receiver, char *state)
{
  unsigned char signal[3];

  if (atoi(receiver) > 3)
  {
    printf("Receiver address cannot be superior to 3\n");
    exit(1);
  }
  if (atoi(state) > 1)
  {
    printf("Type cannot be superior to 1\n");
    exit(1);
  }
  signal[0] = 4;
  signal[1] = (unsigned char) atoi(receiver);
  signal[2] = (unsigned char) atoi(state);
  i2cWrite(fd, &signal[0], 3);
}

int getResponse(int fd)
{
  unsigned char signal[1];
  unsigned char response[VW_MAX_MESSAGE_LEN + 1];
  int dataLen, i;

  signal[0] = 1;
  i2cWrite(fd, &signal[0], 1);
  usleep(10000);
  dataLen = i2cRead(fd, response);
  printf("Response: ");
  for (i = 1; i < dataLen; i++)
    printf("%u ", response[i]);
  printf("\n");
}

int setLedMode(int fd, char *mode)
{
  unsigned char signal[2];

  if (atoi(mode) > 12)
  {
    printf("Led mode cannot be superior to 12\n");
    exit(1);
  }
  signal[0] = 2;
  signal[1] = (unsigned char) atoi(mode);
  i2cWrite(fd, &signal[0], 2);
}

int switchLedDim(int fd)
{
  unsigned char signal[1];

  signal[0] = 3;
  i2cWrite(fd, &signal[0], 1);
}

int main(int argc, char** argv)
{
  int file, arg;
  unsigned int val, ret;
  char buf[4];
  if (argc < 2)
  {
    printf("Supply one or more commands to send to the Arduino\n");
    exit(1);
  }
  printf("I2C: Connecting\n");
  if ((file = open(devName, O_RDWR)) < 0)
  {
    fprintf(stderr, "I2C: Failed to access %s (%i)\n", devName, file);
    exit(1);
  }
  printf("I2C: acquiring buss to 0x%x\n", ADDRESS);
  if (ioctl(file, I2C_SLAVE, ADDRESS) < 0)
  {
    fprintf(stderr, "I2C: Failed to acquire bus access/talk to slave 0x%x\n", ADDRESS);
    exit(1);
  }
  if (strcmp(argv[1], "sendSignal") == 0)
  {
    if (argc != 5)
    {
      printf("sendSignal needs 5 arguments, %d provided\n", argc);
      exit(1);
    }
    sendSignal(file, argv[2], argv[3], argv[4]);
  }
  else if (strcmp(argv[1], "sendHomeEasySignal") == 0)
  {
    if (argc != 4)
    {
      printf("sendHomeEasySignal needs 4 arguments, %d provided\n", argc);
      exit(1);
    }
    sendHomeEasySignal(file, argv[2], argv[3]);
  }
  else if (strcmp(argv[1], "getResponse") == 0)
  {
    if (argc != 2)
    {
      printf("getResponse needs 2 arguments, %d provided\n", argc);
      exit(1);
    }
    getResponse(file);
  }
  else if (strcmp(argv[1], "ledMode") == 0)
  {
    if (argc != 3)
    {
      printf("ledMode needs 3 arguments, %d provided\n", argc);
      exit(1);
    }
    setLedMode(file, argv[2]);
  }
  else if (strcmp(argv[1], "ledDim") == 0)
  {
    if (argc != 2)
    {
      printf("ledDim needs 2 arguments, %d provided\n", argc);
      exit(1);
    }
    switchLedDim(file);
  }
  close(file);
  usleep(100000);
  return (EXIT_SUCCESS);
}


/*  for (arg = 1; arg < argc; arg++)
  {
    if (0 == sscanf(argv[arg], "%d", &val))
    {
      fprintf(stderr, "Invalid parameter %d \"%s\"\n", arg, argv[arg]);
      exit(1);
    }
    printf("Sending %u\n", val);
    if (write(file, &val, sizeof val) == sizeof val)
    {
      usleep(10000);
      if (read(file, buf, 4) == 4)
      {
	printf("Received %d %d %d %d\n", buf[0], buf[1], buf[2], buf[3]);
	ret = buf[0] + buf[1] * 256 + buf[2] * 65536 + buf[3] * 16777216;
	printf("Received %u\n", ret);
      }
    }
    usleep(50000);
  }
*/
