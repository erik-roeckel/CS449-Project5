/*
*
*
//Erik Roeckel (EFR11)
*
*
*
*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#define true 1
#define false 0

typedef int bool; //define own boolean variable
unsigned char die1; //global variable to hold the value of first die roll
unsigned char die2; //global variable to hold the value of second die roll

//roll both dice, returns the sum of these 2 die rolls
unsigned char dice_roll(char** argv)
{
	unsigned char roll1;
	unsigned char roll2;
	unsigned char roll_total;
	FILE* file = fopen(argv[1], "r"); //opens device on command line for generating random numbers
	fread(&roll1, 1, 1, file); // reads one random num from this device
	roll1 %= 6;
	die1 = roll1 + 1;
	fread(&roll2, 1, 1, file); //reads another random num fromm this device
	roll2 %= 6;
	die2 = roll2 + 1;
	roll_total = roll1 + roll2 + 2; // sums values of both rolls
	fclose(file);
	return roll_total;
}

//reads line of input from user, zero terminates the string to handle for a newline cahracter entered by user
char* read_line(char* input, int size)
{
	fgets(input, size, stdin);
	int len = strlen(input);
	input[len-1] = '\0';
	return input;
}

//Decides whether the user won, lost, or have to keep rolling
bool run_game(unsigned char user_roll, char** argv)
{
	unsigned char point = 0;
	unsigned char new_sum = 0;
	bool result;
	
	switch(user_roll)
	{
		// user wins if they roll 7 or 11
		case 7:
		case 11:
			result = true;
			printf("You Win \n");
			break;
		
		//user loses if they roll a 2, 3, 12
		case 2:
		case 3:
		case 12:
			result = false;
			printf("HAHA, you lose \n");
			break;
		//user is prompted to roll again till they win or lose
		case 4:
		case 5:
		case 6:
		case 8:
		case 9:
		case 10:
			point = user_roll; // sets the point the user is trying to roll
			printf("Your point number: %d\n", point);
			
			//loops through while point isn't equal to the sum and the sum rolled isn't 7
			while(point != new_sum && new_sum != 7)
			{
				new_sum = dice_roll(argv);
				printf("You rolled %d + %d = %d\n", die1, die2, new_sum);
			}
			if(point == new_sum) //executes if user rolls point
			{
				printf("You rolled the point, you win!\n");
				break;
			}
			else if(new_sum == 7) // executes if user rolls 7
			{
				printf("You rolled a 7, you lose\n");
				break;
			}
			else
				break;
	}	
	return result;
}


int main(int argc, char** argv)
{
	unsigned char user_roll;
	int valid_input = 0;
	char name[30]; //buffer for user name
	char play[6]; //buffer for play option
	char play_again[5]; //buffer for play again option
	bool play_again_choice = true; // does user want to play again
	bool play_again_not_valid = true; // is user input valid
	
	//executes and exits program if user gave me no device to generate rand numbers with
	if(argv[1] == NULL || (strcmp("/dev/urandom", argv[1]) != 0 && strcmp("/dev/dice", argv[1]) != 0))
	{
		printf("You gave me an invalid or non-existent device to generate random numbers\n");
		printf("Exiting... (try using '/dev/dice' or '/dev/urandom')\n");
		exit(0);
	}

	// loop to re-run craps game until user enters valid input for play
	while(valid_input == 0)
	{
		printf("Welcome to my casino, where the only game available is craps\n");
		printf("Please enter your name: ");
		read_line(name, 30);
		printf("%s, Would you like to 'play' or 'quit'? ", name);
		read_line(play, 6);
		if(strcmp("play", play) == 0)
		{
			if(argc == 2)
			{	
				//loop to ensure the user chose the option to play again
				while(play_again_choice == true)
				{	
					play_again_choice = false;
					user_roll = dice_roll(argv);
					printf("You rolled %d + %d = %d\n", die1, die2, user_roll);
					run_game(user_roll, argv);
					play_again_not_valid = true;
					
					//loops until user enters a valid input for play again option
					while(play_again_not_valid == true)	
					{	
						printf("%s, do you wish to play again? ('yes'/'no')\n", name);
						read_line(play_again, 5);
						if(strcmp("yes", play_again) == 0)
						{
							play_again_not_valid = false;
							play_again_choice = true;
						}
						else if(strcmp("no", play_again) == 0)
						{
							play_again_not_valid = false;
							play_again_choice = false;
						}
						else
						{
							printf("Invalid answer, gonna ask you again...\n");
							play_again_not_valid = true;
						}
					
					}
				
				}
				valid_input = 1;
			}

		}
		//executes if user choses option to quit
		else if(strcmp("quit", play) == 0)
		{
			valid_input = 1;
			exit(0);
		}
		else
			printf("Invalid input... restarting program\n");
	}

}