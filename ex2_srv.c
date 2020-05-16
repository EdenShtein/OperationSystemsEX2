/*Made by Eden Shtein 311396592 and Or Levi 311323034*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_LENGTH 256


//Signal handler
void SignalHandler(int sig);
//Alarm hanlder
void alarmHandler(int alarm);

//Read data from the server
void readFromServer(char* clientPath, pid_t* pid, int* num1, int* operation, int* num2);
//Write to the client
void writeToClient(char* toClient, int* result);

//Read from the file
void readFromFile(int fd, char* buffer);

//Implementation of atoi function  - we can only use system calls
int myAtoi(char* str);
//Implementation to itoa function
void myItoa(int num, char* str, int base);

//Implemention of str length
int my_strlen(char* s);
//STRCAT function
void my_strcat(char* str1, char* str2);
//Reverse a string
void myReverseString(char* str);
//Clear the buffer
void clear_Array(char * buffer);


int main(int argc, char *argv[])
{
	char serverFile[MAX_LENGTH] = "to_srv.txt";
	int fd;

	printf("The server is running and waiting for clients signals.....\n");	printf("Server process ID is: %d\n", getpid());

	//The first thing that the server does is to delete the to_srv.txt (if it exists)
	if ((fd = open(serverFile, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0)
	{
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}
	close(fd);
	if (unlink(serverFile) < 0)
	{
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}
	

	//Sets the signals
	signal(SIGUSR1, SignalHandler);
	signal(SIGALRM, alarmHandler);
	alarm(60);
	while (1)
	{
		pause();
	}
	exit(0);
}

void SignalHandler(int sig)
{
	//Reset to the timer
	alarm(60);

	signal(SIGUSR1, SignalHandler);
	pid_t pid;
	if ((pid = fork())< 0)
	{
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}
	//This is the child process
	if (pid == 0)
	{
		char toClient[MAX_LENGTH] = "to_client_";
		pid_t cPid = 0;
		int num1, operation, num2, result;

		readFromServer(toClient, &cPid, &num1, &operation, &num2);
		switch (operation)
		{
		case 1:
			result = num1 + num2;
			break;
		case 2:
			result = num1 - num2;
			break;
		case 3:
			result = num1 * num2;
			break;
		case 4:
			if (num2 == 0)
			{
				printf("ERROR_FROM_EX2\n");
				exit(-1);
			}
			result = num1 / num2;
			break;

		}
		writeToClient(toClient, &result);
		if ((kill(cPid, SIGUSR1)) == -1)
		{
			printf("ERROR_FROM_EX2\n");
			exit(-1);
		}
		exit(0);
	}
	//To prevent a zombie
	if (pid > 0)
	{
		wait(NULL);
		return;
	}
	
}

void alarmHandler(int alarm)
{
	printf("The server was closed because no service request was received for the last 60 seconds\n");
	exit(0);
}


void readFromServer(char* clientPath, pid_t* pid, int* num1, int* operation, int* num2)
{
	char buffer[MAX_LENGTH] = { 0 };
	int fd;
	if ((fd = open("to_srv.txt", O_RDONLY, 0666)) < 0)
	{
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}
	readFromFile(fd, buffer);
	*pid = myAtoi(buffer);

	my_strcat(clientPath, buffer); //Create the to_clientXXXX file
	my_strcat(clientPath, ".txt");

	clear_Array(buffer);
	readFromFile(fd, buffer);
	*num1 = myAtoi(buffer);

	clear_Array(buffer);
	readFromFile(fd, buffer);
	*operation = myAtoi(buffer);

	clear_Array(buffer);
	readFromFile(fd, buffer);
	*num2 = myAtoi(buffer);
	if (unlink("to_srv.txt") < 0)
	{
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}
	close(fd);

}

void writeToClient(char* toClient, int* result)
{
	int fd;
	char buffer[MAX_LENGTH];
	if ((fd = open(toClient, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0)
	{
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}
	myItoa(*result, buffer, 10);
	if (write(fd, buffer, my_strlen(buffer)) < 0)
	{
		close(fd);
		printf("ERROR_FROM_EX2\n");
		exit(-1);
	}
	close(fd);
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

void readFromFile(int fd, char* buffer)
{
	char ch;
	int i = 0;
	while (1)
	{
		if ((read(fd, &ch, sizeof(ch))) < 0)
		{
			close(fd);
			printf("ERROR_FROM_EX2\n");
			exit(-1);
		}
		if (ch == '\n')
		{
			buffer[i] == '\0';
			break;
		}

		buffer[i++] = ch;
	}
}
void clear_Array(char * buffer){	int i;	for (i = 0; i < MAX_LENGTH; i++)	{		buffer[i] = 0;	}}

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


