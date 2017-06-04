#include <stdio.h>
#include <stdlib.h>
#define BUF_SIZE 255
#define MAX_RESOURCE 10
#define MAX_PROCESS 10
#define INFI 999999999
void Bankers(int total_resource[], int available_resource[],
	int process_state[][MAX_RESOURCE], int process_need[][MAX_RESOURCE]);
int resource_cnt = 0;
int process_cnt = 0;
int main(int argc, char *argv[])
{
	FILE *fp;
	if (argc != 2)
		return 0;
	fopen_s(&fp, argv[1], "r");
	char buf[BUF_SIZE];

	int total_resource[MAX_RESOURCE];
	int available_resource[MAX_RESOURCE];
	int process_state[MAX_PROCESS][MAX_RESOURCE];
	int process_need[MAX_PROCESS][MAX_RESOURCE];
	char* temp_str;

	int buf_index = 0;
	int resource_index = 0;
	int process_index = 0;

	if (fp == NULL)
	{
		printf("READ ERROR\n");
		return 0;
	}
	while (fgets(buf, BUF_SIZE, fp) != NULL)
	{
		if (buf[0] == 'T')//Total
		{
			fgets(buf, BUF_SIZE, fp);

			temp_str = strtok(buf, " ");
			while (temp_str != NULL)
			{
				total_resource[resource_index++] = atoi(temp_str);
				temp_str = strtok(NULL, " ");
			}
			resource_cnt = resource_index;
		}
		else if (buf[0] == 'A')//Available
		{
			fgets(buf, BUF_SIZE, fp);
			resource_index = 0;

			temp_str = strtok(buf, " ");
			while (temp_str != NULL)
			{
				available_resource[resource_index++] = atoi(temp_str);
				temp_str = strtok(NULL, " ");
			}
		}
		else if (buf[0] == 'P')//Process
		{
			fgets(buf, BUF_SIZE, fp);
			process_index = 0;
			resource_index = 0;
			while (buf[0] != '/')
			{
				temp_str = strtok(buf, " ");
				while (temp_str != NULL)
				{
					process_state[process_index][resource_index++] = atoi(temp_str);
					temp_str = strtok(NULL, " ");
				}
				process_index++;
				resource_index = 0;
				fgets(buf, BUF_SIZE, fp);
			}
			process_cnt = process_index;
		}
		else if (buf[0] == 'N')//Needed
		{
			fgets(buf, BUF_SIZE, fp);
			process_index = 0;
			resource_index = 0;
			while (buf[0] != '/')
			{
				temp_str = strtok(buf, " ");
				while (temp_str != NULL)
				{
					process_need[process_index][resource_index++] = atoi(temp_str);
					temp_str = strtok(NULL, " ");
				}
				process_index++;
				resource_index = 0;
				fgets(buf, BUF_SIZE, fp);
			}
		}
	}
	Bankers(total_resource, available_resource, process_state, process_need);
	return 0;
}
void Bankers(int total_resource[], int available_resource[],
	int process_state[][MAX_RESOURCE], int process_need[][MAX_RESOURCE])
{
	int* temp_check;
	temp_check = (int*)malloc(sizeof(int) * process_cnt);
	for (int i = 0; i < process_cnt; i++)temp_check[i] = 0;

	int* processed_check;
	processed_check = (int*)malloc(sizeof(int) * process_cnt);
	for (int i = 0; i < process_cnt; i++)processed_check[i] = 0;

	int* processed_order;
	processed_order = (int*)malloc(sizeof(int) * process_cnt);
	for (int i = 0; i < process_cnt; i++)processed_order[i] = -1;

	int processed_num = 0;
	int unprocessed_flag = 0;
	int exit_flag = 1;
	int temp = 0;
	int not_print_index = 0;
	int deadlock_occur = 0;

	while (process_cnt != processed_num) {
		temp = -1;
		exit_flag = 1;
		for (int i = 0; i < process_cnt; i++)
		{
			unprocessed_flag = 0;
			for (int j = 0; j < resource_cnt; j++)
			{
				if (process_need[i][j] <= available_resource[j]) {
					unprocessed_flag++;
				}
				if (unprocessed_flag == 3) {
					if (temp == -1) temp = i;
					temp_check[i] = 1;
				}
			}
		}

		if (temp != -1)
		{
			//Step Print
			printf("[Step %d]\n", processed_num + 1);
			//Total Print
			if ((processed_num + 1) == 1)
			{
				printf("Total : [%d", total_resource[0]);
				for (int j = 0; j < resource_cnt; j++)
				{
					if (j > 0)
					{
						printf(",%d", total_resource[j]);
					}
				}
				printf("]\n");
			}
			//Available Print
			printf("Available : [%d", available_resource[0]);
			for (int j = 0; j < resource_cnt; j++)
			{
				if (j > 0)
				{
					printf(",%d", available_resource[j]);
				}
			}
			printf("]\n");
			//P_-> Print
			for (int i = 0; i < process_cnt; i++)
			{
				if (processed_check[i] == 0) {
					printf("P%d -> A :", i);
					printf("[%d", process_state[i][0]);
					for (int j = 1; j < resource_cnt; j++)
					{
						printf(",%d", process_state[i][j]);
					}
					printf("], ");
					printf("N: [%d", process_need[i][0]);
					for (int j = 1; j < resource_cnt; j++)
					{
						printf(",%d", process_need[i][j]);
					}
					printf("]\n");
				}
			}
			//Can be Processed Print
			for (int i = 0; i < process_cnt; i++)
			{
				if (i == temp)
					printf("P%d", i);
				else if (temp_check[i] == 1)
					printf(",P%d", i);
			}
			printf(" can be processed.\n");
			//Selected to be processed Print
			printf("P%d is selected to be processed.\n", temp);
			//Allocate Print
			printf("(i) Allocate [%d", process_need[temp][0]);
			for (int j = 0; j < resource_cnt; j++)
			{
				available_resource[j] += process_state[temp][j];
				if (j > 0)
				{
					printf(",%d", process_need[temp][j]);
				}
				process_need[temp][j] = INFI;
			}
			printf("] to P%d\n", temp);
			printf("(ii) P%d completes its work and return resources\n", temp);
			processed_order[processed_num] = temp;
			processed_num++;
			processed_check[temp] = 1;
			for (int i = 0; i < process_cnt; i++)temp_check[i] = 0;
			not_print_index = 0;
			exit_flag = 0;
			printf("\n");
		}
		else
		{
			printf("DEADLOCK OCCURED\n");
			deadlock_occur = 1;
			break;
		}
		if (exit_flag == 1)
			break;
	}
	if (deadlock_occur != 1)
	{
		printf("[Final]\n");
		printf("Processing order : ");
		printf("P%d", processed_order[0]);
		for (int i = 1; i < process_cnt; i++)
		{
			printf(",P%d", processed_order[i]);
		}
		printf("\n");
	}
}