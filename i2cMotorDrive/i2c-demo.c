// I2C test program.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <getopt.h>

// I2C Linux device handle
int g_i2cFile;

// open the Linux device
void i2cOpen()
{
	g_i2cFile = open("/dev/i2c-0", O_RDWR);
	if (g_i2cFile < 0) {
		perror("i2cOpen");
		exit(1);
	}
}

// close the Linux device
void i2cClose()
{
	close(g_i2cFile);
}

// set the I2C slave address for all subsequent I2C device transfers
void i2cSetAddress(int address)
{
	if (ioctl(g_i2cFile, I2C_SLAVE, address) < 0) {
		perror("i2cSetAddress");
		exit(1);
	}
}

void writeRegister(uint8_t reg, uint8_t value)
{
	uint8_t data[3];
	data[0] = reg;
	data[1] = value & 0xff;
	data[2] = 0; 
	if (write(g_i2cFile, data, 3) != 3) {
		perror("SetRegister");
	}
}

int main(int argc, char** argv)
{
	int this_option_optind = optind ? optind : 1;
	
	int option_index = 0;	
	
	float duration;
	
	static struct option long_options[] = {
		{"ldirection",  required_argument, 0, 0},
		{"lspeed",      required_argument, 0, 1},
		{"rdirection",  required_argument, 0, 2},
		{"rspeed",      required_argument, 0, 3},
		{"duration",    required_argument, 0, 4},
		{0,             0,                 0, 0},
	};
		
	// open Linux I2C device
	i2cOpen();

	// set address of the PCA9555
	i2cSetAddress(0x21);
		
	int opt;	
	while ((opt = getopt_long(argc, argv,"0:1:2:d:e:",
			long_options, &option_index)) != -1) {
		usleep(5);		
		switch (opt) {
			case 0 :
				if (atoi(optarg) == 0) writeRegister(0x04, 0x01);
				if (atoi(optarg) == 1) writeRegister(0x04, 0x02);
				break;
			case 1 :
				writeRegister(0x15, atoi(optarg));
				break;
			case 2 :
				if (atoi(optarg) == 0) writeRegister(0x05, 0x01);
				if (atoi(optarg) == 1) writeRegister(0x05, 0x02);
				break;
			case 3 :
				writeRegister(0x16, atoi(optarg));
				break;
			case 4 :
				duration = atof(optarg);
				break;
			default:
				abort();
		}
	}
	// Enable the outputs
	writeRegister(0x01, 0x01);
	writeRegister(0x02, 0x01);
	sleep(duration);
	// Disable the outputs
	writeRegister(0x01, 0x00);
	usleep(1);
	writeRegister(0x02, 0x00);
	// Disable the differential controls
	writeRegister(0x04, 0x00);
	writeRegister(0x05, 0x00);

	// Close the i2c bus when finished
	i2cClose();

	return 0;
}
