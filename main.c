/*
 * main.c
 *
 *  Created on: Mar 17 2017
 *      Author: david
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <wait.h>
#include <string.h>
#include "dsh.h"

#define MAX_PROC 250

int main(int argc, char *argv[])
{

	// DO NOT REMOVE THE BLOCK BELOW (FORK BOMB PREVENTION) //
	struct rlimit limit;
	limit.rlim_cur = MAX_PROC;
	limit.rlim_max = MAX_PROC;
	setrlimit(RLIMIT_NPROC, &limit);
	// DO NOT REMOVE THE BLOCK ABOVE THIS LINE //

	while (1)
	{
		printf("dsh> ");
		char *cmdline = (char *)malloc(MAXBUF); // stores user input from commmand line

		if (fgets(cmdline, 256, stdin) == NULL)
		{			 // the system blocks and waits for user input.
			exit(0); // exit the program if EOF is input
		}
		if (strcmp(cmdline, "\n") == 0)
		{
			continue; // ignore only the new line
		}

		int len = strlen(cmdline);
		// printf("%d \n",len);
		if (len > 0 && cmdline[len - 1] == '\n')
		{
			cmdline[len - 1] = '\0';
		}

		int terms_length = split_length(cmdline, " ");
		char **terms = split(cmdline, " ");

		if (terms[0][0] != '/')
		{
			if (strcmp(terms[0], "exit") == 0)
			{
				exit(0);
			}

			else if (strcmp(terms[0], "pwd") == 0)
			{
				char pwd[MAX_PROC];
				getcwd(pwd, sizeof(pwd));
				printf("%s\n", pwd);
			}
			else if (strcmp(terms[0], "cd") == 0)
			{
				if (terms[1] == NULL || strcmp(terms[1], "\0") == 0 || strcmp(terms[1], "~") == 0)
				{
					char *homeDir = getenv("HOME");
					chdir(homeDir);
				}
				else
				{
					if (chdir(terms[1]) == -1)
					{ // check if success
						// perror("cd"); // print error
						printf("Could not find the path %s \n", terms[1]);
						chdir(terms[1]);
					}
				}
			}

			else
			{ // check current environment
				char cwd[100];
				getcwd(cwd, MAX_PROC);
				char current_path[MAX_PROC];
				snprintf(current_path, MAX_PROC, "%s/%s", cwd, terms[0]);
				int found = 0;
				if (access(current_path, X_OK) != -1)
				{
					// int len_last_string = strlen(terms[terms_length - 1]);
					// printf("%d \n", terms[terms_length - 1][len_last_string - 1]);
					// if (terms[terms_length - 1][len_last_string - 1] == '&')
					if (strcmp(terms[terms_length - 1], "&") == 0)
					{
						terms_length--;
						found = 1;
						pid_t child_pid = fork();
						if (child_pid == 0)
						{ // Child process
							terms_length--;
							char *newargs[terms_length + 1];
							newargs[0] = current_path;
							for (int i = 0; i < terms_length; i++)
							{
								newargs[i + 1] = terms[i + 1];
							}
							newargs[terms_length] = NULL;
							// newargs[terms_length - 1][len_last_string - 1] = '\0';
							// newargs[terms_length - 1] = NULL;
							execv(current_path, newargs);
							// perror("execv");
						}
					}
					else
					{
						found = 1;
						pid_t child_pid = fork();
						if (child_pid == 0)
						{ // Child process
							char *newargs[terms_length + 1];
							newargs[0] = current_path;
							for (int i = 0; i < terms_length; i++)
							{
								newargs[i + 1] = terms[i + 1];
							}
							newargs[terms_length] = NULL;
							execv(current_path, newargs);
							// perror("execv");
						}
						else if (child_pid != 0)
						{ // Parent process
							wait(NULL);
						}
					}
				}

				else
				{
					//  and then check the Path environment
					char *path_env = getenv("PATH");
					char *path = strdup(path_env); // Make a copy of PATH
					char *split_path = strtok(path, ":");

					while (split_path != NULL)
					{
						char final_path[100];
						snprintf(final_path, MAX_PROC, "%s/%s", split_path, terms[0]);
						if (access(final_path, X_OK) != -1)
						{
							// int len_last_string = strlen(terms[terms_length - 1]);
							// if (strcmp(terms[terms_length - 1], "&") == 0)
							if (strcmp(terms[terms_length - 1], "&") == 0)
							{
															//printf("%s \n",terms[terms_length - 1]);

								terms_length--;
								pid_t child_pid = fork();
								if (child_pid == 0)
								{ // Child process
									char *newargs[terms_length + 1];
									newargs[0] = final_path;
									for (int i = 0; i < terms_length; i++)
									{
										newargs[i + 1] = terms[i + 1];
									}
									newargs[terms_length] = NULL;

									execv(final_path, newargs);
									exit(EXIT_FAILURE);
									// perror("execv");
								}
								else if (child_pid != 0)
								{
									//wait(NULL);
									found = 1;
									break;
								}
							}
							else
							{
								pid_t child_pid = fork();
								if (child_pid == 0)
								{ // Child process
									char *newargs[terms_length + 1];
									newargs[0] = final_path;
									for (int i = 0; i < terms_length; i++)
									{
										newargs[i + 1] = terms[i + 1];
									}

									newargs[terms_length] = NULL;
									// newargs[terms_length - 1][len_last_string - 1] = '\0';
									// newargs[terms_length - 1] = NULL;
									execv(final_path, newargs);
									// perror("execv");
								}
								else if (child_pid != 0)
								{
									wait(NULL);
									found = 1;
									break;
								}
							}
						}
						split_path = strtok(NULL, ":"); // Move to the next part of the PATH
					}
					free(path); // Free the copied PATH
				}
				if (!found)
				{
					printf("%s not found\n", terms[0]);
				}
				free(cmdline);
			}
		}

		else if (terms[0][0] == '/')
		{
			if (access(terms[0], X_OK) != -1)
			{
				// int len_last_string = strlen(terms[terms_length - 1]);
				//  printf("%d \n", terms[terms_length - 1][len_last_string - 1]);
				//   printf("%d \n",'&');
				//printf("%d \n", strcmp(terms[terms_length - 1], "&") == 0);
				if (strcmp(terms[terms_length - 1], "&") == 0)

				{
					// Remove the '&' from the arguments
					terms_length--;
					// printf("%d \n", strcmp(terms[terms_length - 1], "&"));
					pid_t child_pid = fork();
					if (child_pid == 0)
					{ // Child process
						char *newargs[terms_length + 1];
						newargs[0] = terms[0];
						for (int i = 0; i < terms_length; i++)
						{
							newargs[i + 1] = terms[i + 1];
						}
						newargs[terms_length] = NULL;
						// newargs[terms_length - 1][len_last_string - 1] = '\0';
						// newargs[terms_length - 1] = NULL;
						execv(terms[0], newargs);
						// perror("execv");
					}
				}
				else
				{
					pid_t child_pid = fork();
					if (child_pid == 0)
					{ // Child process
						char *newargs[terms_length + 1];
						newargs[0] = terms[0];
						for (int i = 0; i < terms_length; i++)
						{
							newargs[i + 1] = terms[i + 1];
						}
						newargs[terms_length] = NULL;
						execv(terms[0], newargs);
						// perror("execv");
					}
					else if (child_pid != 0)
					{ // Parent process
						wait(NULL);
					}
				}
			}

			else
			{
				printf("%s  not found! \n", terms[0]);
			}
		}
		// free(cmdline);
	}
	return 0;
}