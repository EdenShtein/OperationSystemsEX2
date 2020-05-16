/*Made by Eden Shtein 311396592 and Or Levi 311323034*/
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>

#define MAX_LENGTH 256

//Signal handler
void SignalHandler(int sig);
//Alarm hanlder
void alarmHandler(int alarm);
//Send data to the server
void sendToServer(int argc, char *argv[]);

//Implementation of atoi function  - we can only use system calls
int myAtoi(char* str);
//Implementation to itoa function
void myItoa(int num, char* str, int base);

//Get random number from 1 to 5 for the sleep time
int getSleep();


//Implemention of str length
int my_strlen(char* s);
//STRCAT function
void my_strcat(char* str1, char* str2);
//Reverse a string
void myReverseString(char* str);




int main(int argc, char *argv[])
{
	//Check if the number of the arguments is valid
	if (argc != 5)
	{
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}
	//Check if the operator is valid ( 1 + 2 - 3 * 4 /)
	if ((myAtoi(argv[3]) < 1) || (myAtoi(argv[3]) > 4))
	{
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}
	signal(SIGUSR1, SignalHandler);
	signal(SIGALRM, alarmHandler);
	
	//Send data to the server
	sendToServer(argc, argv);

	if ((kill(myAtoi(argv[1]), SIGUSR1)) == -1)
	{
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}

	alarm(30);
	pause();
	exit(0);
}


void sendToServer(int argc, char *argv[])
{	
	int fd,i;
	pid_t cPid = getpid();
	char buffer[MAX_LENGTH] = { 0 };
	for (i = 0; i < 10; i++)
	{
		if ((fd = open("to_srv.txt", O_WRONLY | O_CREAT | O_EXCL, 0666)) < 0)
		{
			sleep(getSleep());
		}
		else { break; }
	}
	//The client could not open the to_srv.txt file after 10 tries
	if (fd < 0)
	{
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}
	myItoa(cPid, buffer, 10);
	if (write(fd, buffer, my_strlen(buffer)) < 0)
	{
		close(fd);
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}
	if (write(fd, "\n", my_strlen("\n")) < 0)
	{
		close(fd);
		printf("ERROR_FROM_EX2\n");
		exit(-1);

	}
	for (i = 2; i < argc; i++)
	{
		if (write(fd, argv[i], my_strlen(argv[i])) < 0)
		{
			close(fd);
			printf("ERROR_FROM_EX2\n");
			exit(-1);
		}
		if (write(fd, "\n", my_strlen("\n")) < 0)
		{
			close(fd);
			printf("ERROR_FROM_EX2\n");
			exit(-1);

		}
	}
	close(fd);
}

void alarmHandler(int alarm)
{
	printf("Client closed because no response was received from the server for 30 seconds\n");
}

void SignalHandler(int sig)
{
	alarm(0);
	char path[MAX_LENGTH] = "to_client_";
	char cPidtmp[MAX_LENGTH] = { 0 };
	char buffer[MAX_LENGTH] = { 0 };
	int fd;
	pid_t clientPid = getpid();

	myItoa(clientPid, cPidtmp, 10);
	my_strcat(path, cPidtmp);
	//Check if there is a problem with the existence of the file
	if (path[0] == '\0')
	{
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}
	my_strcat(path, ".txt");
	if ((fd = open(path, O_RDONLY, 0666)) < 0)
	{
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}
	if ((read(fd, buffer, MAX_LENGTH)) < 0)
	{
		printf("ERROR_FROM_EX2\n");
		exit(-1);
		close(fd);
	}
	printf("The result for the client with the process ID %d is: %s\n", clientPid, buffer);
	close(fd);
	if (unlink(path) < 0)
	{
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}
}

void myItoa(int num, char* str, int base)
{
	int i = 0;
	int isNegative = 0;

	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
	}

	if ((num < 0) && (base == 10))
	{
		isNegative = 1;
		num = -num;
	}

	while (num != 0)
	{
		int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	// If number is negative, append '-'. 
	if (isNegative == 1) { str[i++] = '-'; }

	str[i] = '\0';

	// Reverse the string 
	myReverseString(str);
}

int myAtoi(char* str)
{
	int res = 0; // Initialize result 

	// Iterate through all characters of input string and 
	// update result 
	for (int i = 0; str[i] != '\0'; ++i)
		res = res * 10 + str[i] - '0';

	// return result. 
	return res;
}
int my_strlen(char* s)
{
	int i;
	i = 0;
	while (s[i] != '\0')
	{
		i++;
	}
	return i;
}

int getSleep()
{
	int rand = 0;

	if (getrandom(&rand, sizeof(rand), 0) < 0)
	{
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}
	if (rand < 0) { rand *= -1; }
	return ((rand % 5) + 1);
}


void myReverseString(char* str)
{
	int l;
	int i;
	char* begin_ptr;
	char* end_ptr;
	char tmp;

	l = my_strlen(str);

	// Set the begin_ptr and end_ptr .
	begin_ptr = str;
	end_ptr = str;

	for (i = 0; i < l - 1; i++) { end_ptr++; }

	// Swap the char from start and end. 
	for (i = 0; i < (l / 2); i++) {

		tmp = *end_ptr;
		*end_ptr = *begin_ptr;
		*begin_ptr = tmp;

		begin_ptr++;
		end_ptr--;
	}
}
void my_strcat(char* str1, char* str2)
{
	int i = 0, j = 0;

	while (str1[i] != '\0')
	{
		i++;
	}
	while (str2[j] != '\0') {
		str1[i] = str2[j];
		i++;
		j++;
	}
	str1[i] = '\0';
}

