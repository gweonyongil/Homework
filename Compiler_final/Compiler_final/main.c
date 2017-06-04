#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_NON_TERMINAL_SYMBOL 30
#define MAX_TERMINAL_SYMBOL 20
#define MAX_STATE 50
#define BUF_SIZE 255
#define MAX_PRODUCTION 100
#define MAX_FIRST_SYMBOL 20
#define MAX_FOLLOW_SYMBOL 20
#define EPSILON '#'

char** FIRST_OP(char *arr[], char* first_result[]);
char** FOLLOW_OP(char *arr[], char *first_result[], char *follow_result[]);
char** GOTO_OP(char *arr[]);
//GOTO_OP_FUNC
char* insert_dot(char arr[]);
char* move_dot(char arr[]);
char* find_goto_set(char arr[], int arr_length);
//
char nont_node[MAX_NON_TERMINAL_SYMBOL];
char *prod_arr[MAX_PRODUCTION];
int follow_result_node_index[MAX_NON_TERMINAL_SYMBOL];
int nont_node_length[MAX_NON_TERMINAL_SYMBOL];
int prod_num = 0;
int non_t_count = 0;
int main(int argc, char* argv[])
{
	int non_t_index = 0;
	char **goto_result;
	char **first_result;
	char **follow_result;
	char buf[BUF_SIZE];
	FILE *fp;
	if (argc != 2)
		return 0;
	fopen_s(&fp, argv[1], "r");

	if (fp == NULL)
	{
		printf("READ ERROR\n");
		return 0;
	}
	while (fgets(buf, BUF_SIZE, fp) != NULL)
	{
		prod_arr[prod_num] = (char*)malloc(sizeof(char) * BUF_SIZE);
		strcpy_s(prod_arr[prod_num], sizeof(buf), buf);
		if (prod_num != 0 && prod_num % 2 != 0)
		{
			if (non_t_count == 0 || nont_node[non_t_index] != prod_arr[prod_num][0])
			{
				non_t_index = non_t_count;
				nont_node[non_t_count++] = prod_arr[prod_num][0];
			}
		}
		prod_num++;
	}
	for (int i = 0; i < MAX_NON_TERMINAL_SYMBOL; i++)
		follow_result_node_index[i] = 0;
	for (int i = 0; i < MAX_NON_TERMINAL_SYMBOL; i++)
		nont_node_length[i] = 0;
	goto_result = GOTO_OP(prod_arr);
	return 0;
}
char** FIRST_OP(char *arr[], char *first_result[])
{
	char nont_temp1 = ' ';
	char nont_temp2 = ' ';
	int nont_index = 0;
	int first_result_index = 0;
	int prev_check = 0;
	int post_check = 1;
	int origin_index = -1;
	int add_index = -1;
	int same_check = 0;

	for (int i = 0; i < non_t_count; i++)
	{
		first_result[i] = (char*)malloc(sizeof(char) * BUF_SIZE);
	}
	for (int j = 0; j < MAX_FIRST_SYMBOL; j++)
		first_result[nont_index][j] = '\0';

	for (int i = 0; i < prod_num; i++)
	{
		if (i % 2 != 0) nont_temp2 = arr[i][0];
		if (i != 0 && nont_temp1 != nont_temp2 && nont_temp1 != ' ')
		{
			nont_node_length[nont_index] = first_result_index;
			nont_index++;
			for (int j = 0; j < MAX_FIRST_SYMBOL; j++)
				first_result[nont_index][j] = '\0';
			first_result_index = 0;
		}
		if ((i % 2 != 0) && !(arr[i][2] >= 'A' && arr[i][2] <= 'Z'))
		{
			first_result[nont_index][first_result_index++] = arr[i][2];
			nont_node_length[nont_index]++;
		}
		if (i % 2 != 0) nont_temp1 = arr[i][0];
	}
	while (prev_check != post_check)
	{
		prev_check = post_check;
		for (int i = 1; i < prod_num; i += 2)
		{
			for (int j = 0; j <= nont_index; j++)
			{
				if (arr[i][0] == nont_node[j])
				{
					origin_index = j;
					break;
				}
			}
			if (arr[i][2] >= 'A' && arr[i][2] <= 'Z')//>다음부터
			{
				for (int j = 0; j <= nont_index; j++)
				{
					if (arr[i][2] == nont_node[j])
					{
						add_index = j;
						break;
					}
				}
				if (nont_node_length[add_index] != 0)
				{
					same_check = 0;
					int origin_index2 = (nont_node_length[origin_index] - 1);
					for (int i = (nont_node_length[add_index] - 1); i >= 0; i--)//비교하는 과정
					{
						if (first_result[origin_index][origin_index2--] != first_result[add_index][i]) {
							break;
						}
						else {
							if (i == 0)
								same_check = 1;
						}
					}
					if (same_check != 1)//넣는다.
					{
						int nont_node_length_temp = nont_node_length[add_index];
						for (int j = 0; j < nont_node_length_temp; j++)
						{
							int same = 0;
							for (int i = 0; i < nont_node_length[origin_index]; i++)
							{
								if (first_result[origin_index][i]
									== first_result[add_index][j])
								{
									same = 1;
									break;
								}
							}
							if (same == 0) {
								first_result[origin_index][nont_node_length[origin_index]++] = first_result[add_index][j];
								post_check++;
							}
						}
					}
				}
			}
		}
	}
	return first_result;
}
char** FOLLOW_OP(char *arr[], char *first_result[], char *follow_result[])
{
	int follow_result_index = 0;
	int nont_node_index = 0;
	int search_index = 0;
	int same_check = 0;

	for (int i = 0; i < non_t_count; i++)
	{
		follow_result[i] = (char*)malloc(sizeof(char) * MAX_FOLLOW_SYMBOL);
	}
	//Starting Node $추가
	follow_result[nont_node_index][follow_result_node_index[nont_node_index]++] = '$';
	for (int i = 1; i < prod_num; i += 2)
	{
		int j = 2;
		while (arr[i][j] != '\n')
		{
			if (!(arr[i][j] >= 'A' && arr[i][j] <= 'Z'))
			{
				if (arr[i][j - 1] >= 'A' && arr[i][j - 1] <= 'Z')
				{
					for (int k = 0; k < non_t_count; k++)
					{
						if (arr[i][j - 1] == nont_node[k]) {
							nont_node_index = k;
							break;
						}
					}
					if (arr[i][j] != EPSILON) {
						follow_result[nont_node_index][follow_result_node_index[nont_node_index]++] = arr[i][j];
					}
				}
			}
			j++;
		}
	}
	for (int i = 1; i < prod_num; i += 2)
	{
		search_index = 0;
		while (arr[i][search_index] != '\n') search_index++;
		search_index--;
		if (arr[i][search_index] >= 'A' && arr[i][search_index] <= 'Z')
		{
			int nont_node_index1 = 0;//E>E+T에서 1번째 E역할
			int nont_node_index2 = 0;//E>E+T에서 T역할x
			for (int j = 0; j < non_t_count; j++)
			{
				if (arr[i][0] == nont_node[j])
				{
					nont_node_index1 = j;
					break;
				}
			}
			for (int j = 0; j < non_t_count; j++)
			{
				if (arr[i][search_index] == nont_node[j])
				{
					nont_node_index2 = j;
					break;
				}
			}
			//printf("%c %c", nont_node[nont_node_index1], nont_node[nont_node_index2]);
			for (int j = 0; j < follow_result_node_index[nont_node_index1]; j++)
			{
				same_check = 0;
				for (int k = 0; k < follow_result_node_index[nont_node_index2]; k++)
				{
					if (follow_result[nont_node_index2][k] == follow_result[nont_node_index1][j]) {
						same_check = 1;
						break;
					}
				}
				if (same_check != 1)
					follow_result[nont_node_index2][follow_result_node_index[nont_node_index2]++] =
					follow_result[nont_node_index1][j];
			}
		}
	}
	for (int i = 0; i < non_t_count; i++)
	{
		follow_result[i][follow_result_node_index[i]] = '\0';
	}
	return follow_result;
}
char** GOTO_OP(char *arr[])
{
	int index = 0;
	int row_index = 0;
	int length = 0;
	int result_arr_length[MAX_STATE];
	char nont_check[MAX_NON_TERMINAL_SYMBOL];
	int nont_check_index = 0;
	int result_index = 0;
	char nont = ' ';
	char** result_arr;
	result_arr = (char**)malloc(sizeof(char*) * MAX_STATE);
	char* temp = (char*)malloc(sizeof(char) * BUF_SIZE);
	char str[BUF_SIZE] = "S>";
	str[2] = prod_arr[1][0];
	strcpy_s(temp, sizeof(temp), str);
	temp = insert_dot(str);
	result_arr[0] = (char*)malloc(sizeof(char) * BUF_SIZE);
	for (int i = 0; i < 4; i++, index++)
	{
		result_arr[row_index][index + 2] = temp[index];
	}
	result_arr[0][0] = '_';
	result_arr[0][1] = '_';
	result_arr[0][6] = ',';
	result_arr_length[row_index] = 6;
	result_arr[0] = find_goto_set(result_arr[0], result_arr_length[row_index]);//S->.E
																			   //PROD_REDUCE_CHECK 기존에 ITEM이 있는지 검사
	int* prod_reduce_check;
	prod_reduce_check = (int*)malloc(sizeof(int) * (prod_num / 2) + 1);//PROD 1부터 시작 NOT 0
	for (int i = 1; i <= (prod_num / 2); i++)
		prod_reduce_check[i] = 0;
	int item_state_index = 0;
	int item_state_array_index = 2;
	int next_row_index = row_index + 1;
	int search_row_index = 0;
	row_index = 0;
	int all_prod_check_count = 0;
	int start_count = 2;
	int final_count = 0;
	int test = 0;
	char search_word = '_';
	char new_word = '_';
	char prev_nont = '_';
	char now_nont = '_';
	int flag1 = 0;
	int item_case = 0;
	int exit = 0;
	while (1)
	{
		int index = 2;
		search_word = '_';
		new_word = '_';
		item_state_array_index = 2;
		//ITEM_STATE 검사 후에 기존에 있다면 메모리를 할당하지 않아야 한다.
		while (result_arr[search_row_index][index++] != '\0')
		{
			if (result_arr[search_row_index][index] == '.' &&
				(result_arr[search_row_index][index + 1] != '\0' && result_arr[search_row_index][index + 1] != ','))
			{
				if (search_word == '_' && new_word == '_')
				{
					search_word = result_arr[search_row_index][index + 1];
					new_word = result_arr[search_row_index][index + 1];
					result_arr[++item_state_index] = (char*)malloc(sizeof(char) * BUF_SIZE);
					result_arr[item_state_index][0] = search_row_index + 48;
					result_arr[item_state_index][1] = search_word;
				}
				else
					new_word = result_arr[search_row_index][index + 1];
				//새로운 WORD를 읽을 때
				if (search_word != new_word)
				{
					search_word = new_word;
					result_arr[item_state_index][item_state_array_index - 1] = ',';
					result_arr_length[item_state_index] = item_state_array_index - 1;
					result_arr[item_state_index] = find_goto_set(result_arr[item_state_index], result_arr_length[item_state_index]);
					//ITEM_STATE 검사 후에 기존에 있다면 메모리를 할당하지 않아야 한다.
					for (int i = 0; i < item_state_index; i++)
					{
						int index = 2;
						while (result_arr[i][index] == result_arr[item_state_index][index])
						{
							index++;
							if (result_arr[i][index] == '\0')
							{
								free(result_arr[item_state_index]);
								item_state_index--;
								i = search_row_index;
								break;
							}
						}
					}
					result_arr[++item_state_index] = (char*)malloc(sizeof(char) * BUF_SIZE);
					result_arr[item_state_index][0] = search_row_index + 48;
					result_arr[item_state_index][1] = search_word;
					item_state_array_index = 2;
				}
				while (result_arr[search_row_index][index--] != '>');
				while (result_arr[search_row_index][index] != ',' && result_arr[search_row_index][index] != '\0')//데이터 입력
				{
					if (result_arr[search_row_index][index] == '.' &&
						(result_arr[search_row_index][index + 1] != ',' &&
							result_arr[search_row_index][index + 1] != '\0'))
					{
						result_arr[item_state_index][item_state_array_index] = result_arr[search_row_index][index + 1];
						result_arr[item_state_index][item_state_array_index + 1] = '.';
						item_state_array_index += 2;
						index += 2;
					}
					else
						result_arr[item_state_index][item_state_array_index++] = result_arr[search_row_index][index++];
					if (result_arr[search_row_index][index] == ',')
						result_arr[item_state_index][item_state_array_index++] = ',';
					//printf("%d %c %c\n", item_state_index, result_arr[item_state_index][item_state_array_index - 1], result_arr[search_row_index][index - 1]);
				}
				if (result_arr[search_row_index][index] == '\0')
				{
					result_arr[item_state_index][item_state_array_index] = ',';
					result_arr_length[item_state_index] = item_state_array_index;
					result_arr[item_state_index] = find_goto_set(result_arr[item_state_index], result_arr_length[item_state_index]);
					//ITEM_STATE 검사 후에 기존에 있다면 메모리를 할당하지 않아야 한다.
					for (int i = 0; i < item_state_index; i++)
					{
						int index = 2;
						while (result_arr[i][index] == result_arr[item_state_index][index])
						{
							index++;
							if (result_arr[i][index] == '\0')
							{
								free(result_arr[item_state_index]);
								item_state_index--;
								i = search_row_index;
								break;
							}
						}
					}
					//result_arr[item_state_index][item_state_array_index] = '\0';
				}
			}
		}
		search_row_index++;
		if (item_state_index < search_row_index)
			break;
	}
	next_row_index = item_state_index + 1;
	//ITEM STATE COMPLETE

	//ITEM_NUMBERING
	int total_item_cnt = next_row_index;
	int* item_number;
	int real_item_cnt = 0;
	item_number = (int*)malloc(sizeof(int) * next_row_index);
	for (int i = 0; i < next_row_index; i++)
	{
		if (result_arr[i][1] != '\0')
		{
			item_number[i] = real_item_cnt++;
		}
		else
		{
			item_number[i] = -1;
		}
	}
	//SLR PART GOTO TABLE
	int** slr_goto_table;
	slr_goto_table = (int**)malloc(sizeof(int*) * (real_item_cnt));
	for (int i = 0; i < real_item_cnt; i++)
	{
		slr_goto_table[i] = (int*)malloc(sizeof(int) * non_t_count);
	}
	for (int i = 0; i < real_item_cnt; i++)//12
	{
		for (int j = 0; j < non_t_count; j++)
		{
			slr_goto_table[i][j] = -1;
		}
	}
	int nont_index = 0;
	row_index = 0;
	index = 0;
	while (1)
	{
		int check = 0;
		int test = 0;
		int temp2_index = 2;
		while (row_index != total_item_cnt)
		{
			char temp2[100];
			check = 0;
			if (result_arr[row_index][index - 1] == '.'
				&& result_arr[row_index][index] == nont_node[nont_index])
			{
				int temp_index = index;
				while (result_arr[row_index][temp_index--] != ',')
				{
					if (temp_index == 2)
					{
						break;
					}
				}
				if (temp_index == 2)
					start_count = temp_index;
				else
					start_count = temp_index + 1;
				check = 1;
			}
			if (check == 1)
			{
				int temp = index;
				while (result_arr[row_index][index++] != ',') final_count = index - 1;
				index = temp;
				for (int i = start_count; i <= final_count; i++, temp2_index++)
				{
					if (result_arr[row_index][i] == '.')
					{
						temp2[temp2_index] = result_arr[row_index][i + 1];
						temp2[temp2_index + 1] = '.';
						temp2_index++;
						i++;
					}
					else
						temp2[temp2_index] = result_arr[row_index][i];
					//예외처리
					if (temp2[2] == ',') {
						temp2_index--;
					}

					if (i == final_count)
						start_count = final_count + 1;;
				}
			}
			index++;
			if (result_arr[row_index][index] == '\0')//GOTO TABLE 생성
			{
				temp2[temp2_index] = '\0';
				index = 2;
				start_count = 2;
				int same = 0;
				int same_item_num;
				for (int i = 0; i < next_row_index; i++)
				{
					for (int j = index; result_arr[i][j] != '\0'; j++)
					{
						if (temp2[j] != result_arr[i][j])
						{
							same = 0;
							break;
						}
						else
							same = 1;
					}
					if (same == 1)
					{
						same_item_num = i;
						break;
					}
				}
				if (same == 1) {
					//printf("\nGOTO(%d, %c) = %d", row_index, nont_node[nont_index], item_number[same_item_num]);
					for (int i = 0; i < non_t_count; i++)
					{
						if (nont_node[i] == nont_node[nont_index])
						{
							slr_goto_table[row_index][i] = item_number[same_item_num];
						}
					}
				}
				temp2_index = 2;
				nont_index++;
				if (nont_index == non_t_count)//non_t_count(NONTERMINAL REAL COUNT)
				{
					row_index++;
					nont_index = 0;
				}
				test++;
			}
		}
		break;
	}
	//SLR_PART_GOTO_TABLE 생성
	//ACTION TABLE
	//TEMINAL ARRAY CREATE
	char terminal_array[MAX_TERMINAL_SYMBOL];
	int terminal_num = 0;
	for (int i = 1; i < prod_num; i += 2)
	{
		int index = 1;
		int same = 0;
		while (prod_arr[i][index++] != '\n')
		{
			if (!(prod_arr[i][index] >= 'A' && prod_arr[i][index] <= 'Z'))
			{
				same = 0;
				for (int j = 0; j < terminal_num; j++)
				{
					if (terminal_array[j] == prod_arr[i][index]) {
						same = 1;
						break;
					}
				}
				if (same == 0 && prod_arr[i][index] != '\n')
					terminal_array[terminal_num++] = prod_arr[i][index];
			}
		}
	}
	terminal_array[terminal_num++] = '$';
	//SLR_PART_ACTION_TABLE
	char** slr_action_table;
	char** first_result;
	first_result = (char**)malloc(sizeof(char) * MAX_FIRST_SYMBOL);
	for (int i = 0; i < non_t_count; i++)
		first_result[i] = (char*)malloc(sizeof(char) * BUF_SIZE);
	first_result = FIRST_OP(prod_arr, first_result);
	//
	char** follow_result;
	int** action_item_num;
	follow_result = (char**)malloc(sizeof(char*) * MAX_FOLLOW_SYMBOL);
	for (int i = 0; i < MAX_FOLLOW_SYMBOL; i++)
		follow_result[i] = (char*)malloc(sizeof(char) * BUF_SIZE);
	follow_result = FOLLOW_OP(prod_arr, first_result, follow_result);
	//
	slr_action_table = (char**)malloc(sizeof(char*) * real_item_cnt);
	for (int i = 0; i < real_item_cnt; i++)
	{
		slr_action_table[i] = (char*)malloc(sizeof(char) * terminal_num * 2);
	}

	action_item_num = (int**)malloc(sizeof(int*) * real_item_cnt);
	for (int i = 0; i < real_item_cnt; i++)
	{
		action_item_num[i] = (int*)malloc(sizeof(int) * terminal_num);
	}
	//DATA 초기화 slr_action_table
	for (int i = 0; i < real_item_cnt; i++)
	{
		for (int j = 0; j < terminal_num * 2; j++)
			slr_action_table[i][j] = '_';
	}
	//DATA 초기화 action_item_num
	for (int i = 0; i < real_item_cnt; i++)
	{
		for (int j = 0; j < terminal_num; j++)
			action_item_num[i][j] = -1;
	}
	//TEMINAL NODE 추출
	//printf("\n--TEMINAL GOTO--\n");
	char t_arr[MAX_NON_TERMINAL_SYMBOL];
	row_index = 0;
	int t_arr_index = 0;
	int t_arr_num = 0;
	row_index = 0;
	index = 0;
	while (1)
	{
		int check = 0;
		int test = 0;
		int temp2_index = 2;
		while (row_index != next_row_index)
		{
			char temp2[100];
			check = 0;
			if (t_arr_index == 0 && index == 0)
			{
				while (result_arr[row_index][index++] != '\0')//terminal node 추출
				{
					if (result_arr[row_index][index] == '.'
						&& !(result_arr[row_index][index + 1] >= 'A' && result_arr[row_index][index + 1] <= 'Z')
						&& result_arr[row_index][index + 1] != ',')
					{
						t_arr[t_arr_index++] = result_arr[row_index][index + 1];
					}
					if (result_arr[row_index][index + 1] == '\0')
					{
						t_arr[t_arr_index] = '\0';
						t_arr_num = t_arr_index;
						t_arr_index = 0;
						break;
					}
				}
				index = 0;
			}
			if (result_arr[row_index][index - 1] == '.'
				&& result_arr[row_index][index] == t_arr[t_arr_index])
			{
				int temp_index = index;
				while (result_arr[row_index][temp_index--] != ',')
				{
					if (temp_index == 2)
					{
						break;
					}
				}
				if (temp_index == 2)
					start_count = temp_index;
				else
					start_count = temp_index + 1;
				check = 1;
			}
			if (check == 1)
			{
				int temp = index;
				while (result_arr[row_index][index] != ',' && result_arr[row_index][index])
				{
					final_count = index - 1;
					index++;
				}
				index = temp;
				for (int i = start_count; i <= final_count; i++, temp2_index++)
				{
					if (result_arr[row_index][i] == '.')
					{
						temp2[temp2_index] = result_arr[row_index][i + 1];
						temp2[temp2_index + 1] = '.';
						temp2_index++;
						i++;
					}
					else
						temp2[temp2_index] = result_arr[row_index][i];
					//예외처리
					if (temp2[2] == ',') {
						temp2_index--;
					}

					if (i == final_count) {
						start_count = final_count + 1;;
					}
				}
			}
			index++;
			if (result_arr[row_index][index] == '\0')//다 읽고 난 뒤에, GOTO TABLE 생성
			{
				temp2[temp2_index] = '\0';
				index = 2;
				start_count = 2;
				int same = 0;
				int same_item_num;
				for (int i = 0; i < next_row_index; i++)
				{
					for (int j = index; temp2[j] != '\0'; j++)
					{
						if (temp2[j] != result_arr[i][j])
						{
							same = 0;
							break;
						}
						else
							same = 1;
					}
					if (same == 1)
					{
						same_item_num = i;
						break;
					}
				}
				index = 0;
				if (same == 0) {
					while (result_arr[row_index][index++] != '\0')
					{
						if (result_arr[row_index][index] == '.' &&
							(result_arr[row_index][index + 1] == ',' || result_arr[row_index][index + 1] == '\0'))
						{
							while (result_arr[row_index][index--] != '>');
							if (result_arr[row_index][index] == 'S')
							{
								//ACCEPT 표시
								slr_action_table[item_number[row_index]][2 * (terminal_num - 1)] = 'a';
								slr_action_table[item_number[row_index]][2 * (terminal_num - 1) + 1] = 'a';
								action_item_num[item_number[row_index]][terminal_num - 1] = 0;
								break;
							}
							int result_arr_index = 0;
							int prod_arr_index = 0;
							while (result_arr[row_index][result_arr_index++] != '\0')
							{
								if (result_arr[row_index][result_arr_index] == '.'
									&& (result_arr[row_index][result_arr_index + 1] == '\0' ||
										result_arr[row_index][result_arr_index + 1] == ','))
								{
									while (result_arr[row_index][result_arr_index--] != '>');
									int result_temp_index = result_arr_index;
									for (int i = 1; i < prod_num; i += 2)
									{
										int prod_arr_index2 = 0;
										result_arr_index = result_temp_index;
										while (prod_arr[i][prod_arr_index2++] == result_arr[row_index][result_arr_index++])
										{
											if (result_arr[row_index][result_arr_index] == '.')
												prod_arr_index = (i + 1) / 2;
										}
									}
									break;
								}
							}
							for (int i = 0; i < non_t_count; i++)
							{
								if (nont_node[i] == result_arr[row_index][index])
								{
									for (int j = 0; j < follow_result_node_index[i]; j++)
									{
										for (int k = 0; k < terminal_num; k++)
										{
											if (terminal_array[k] == follow_result[i][j])
											{
												slr_action_table[item_number[row_index]][2 * k] = 'r';
												slr_action_table[item_number[row_index]][(2 * k) + 1] = 48 + prod_arr_index;
												action_item_num[item_number[row_index]][k] = prod_arr_index;
											}
										}
									}
								}
							}
							break;
						}
					}
				}
				if (same == 1)
				{
					//printf("GOTO(%d, %c) = %d\n", item_number[row_index], t_arr[t_arr_index], item_number[same_item_num]);
					int index = 0;
					while (result_arr[row_index][index++] != '\0')
					{
						if (result_arr[row_index][index] == '.' &&
							(result_arr[row_index][index + 1] == ',' || result_arr[row_index][index + 1] == '\0'))
						{
							while (result_arr[row_index][index--] != '>');
							if (result_arr[row_index][index] == 'S')
							{
								//ACCEPT 표시
								slr_action_table[item_number[row_index]][2 * (terminal_num - 1)] = 'a';
								slr_action_table[item_number[row_index]][2 * (terminal_num - 1) + 1] = 'a';
								action_item_num[item_number[row_index]][terminal_num - 1] = 0;
								break;
							}
							int result_arr_index = 0;
							int prod_arr_index = 0;
							while (result_arr[row_index][result_arr_index++] != '\0')
							{
								if (result_arr[row_index][result_arr_index] == '.'
									&& (result_arr[row_index][result_arr_index + 1] == '\0' ||
										result_arr[row_index][result_arr_index + 1] == ','))
								{
									while (result_arr[row_index][result_arr_index--] != '>');
									int result_temp_index = result_arr_index;
									for (int i = 1; i < prod_num; i += 2)
									{
										int prod_arr_index2 = 0;
										result_arr_index = result_temp_index;
										while (prod_arr[i][prod_arr_index2++] == result_arr[row_index][result_arr_index++])
										{
											if (result_arr[row_index][result_arr_index] == '.')
												prod_arr_index = (i + 1) / 2;
										}
									}
									break;
								}
							}
							for (int i = 0; i < non_t_count; i++)
							{
								if (nont_node[i] == result_arr[row_index][index])
								{
									for (int j = 0; j < follow_result_node_index[i]; j++)
									{
										for (int k = 0; k < terminal_num; k++)
										{
											if (terminal_array[k] == follow_result[i][j])
											{
												slr_action_table[item_number[row_index]][2 * k] = 'r';
												slr_action_table[item_number[row_index]][(2 * k) + 1] = 48 + prod_arr_index;
												action_item_num[item_number[row_index]][k] = prod_arr_index;
											}
										}
									}
								}
							}
							break;
						}
					}
					for (int i = 0; i < non_t_count; i++)
					{
						for (int k = 0; k < terminal_num; k++)
						{
							if (terminal_array[k] == t_arr[t_arr_index])
							{
								slr_action_table[item_number[row_index]][2 * k] = 's';
								slr_action_table[item_number[row_index]][(2 * k) + 1] = item_number[same_item_num] + 48;
								action_item_num[item_number[row_index]][k] = item_number[same_item_num];
							}
						}
					}
				}
				temp2_index = 2;
				t_arr_index++;
				if (t_arr_index == (t_arr_num))//non_t_count(NONTERMINAL REAL COUNT)
				{
					row_index++;
					t_arr_index = 0;
					index = 0;
				}
			}
		}
		break;
	}
	//PRINT ACTION TABLE
	//SLR PARSING TABLE CONSTRUCTION
	while (1)
	{
		char input[BUF_SIZE];
		printf(">>");
		scanf_s("%s", input, BUF_SIZE);
		if (strcmp(input, "TABLE") == 0)
		{
			for (int i = 0; i < real_item_cnt; i++)
			{
				printf("%d :", i);
				for (int j = 0; j < terminal_num; j++)
				{
					if (action_item_num[i][j] != -1)
						printf("%c[%c%d] ", terminal_array[j], slr_action_table[i][j * 2], action_item_num[i][j]);
				}
				for (int j = 0; j < non_t_count; j++)
				{
					if (slr_goto_table[i][j] != -1)
						printf("%c[%d] ", nont_node[j], slr_goto_table[i][j]);
				}
				printf("\n");
			}
		}
		else if (strcmp(input, "FIRST") == 0)
		{
			for (int i = 0; i < non_t_count; i++)
			{
				printf("%c : %s\n", nont_node[i], first_result[i]);
			}
		}
		else if (strcmp(input, "FOLLOW") == 0)
		{
			for (int i = 0; i < non_t_count; i++)
			{
				follow_result[i][follow_result_node_index[i]] = '\0';
				printf("%c : %s\n", nont_node[i], follow_result[i]);
			}
		}
		else if (strcmp(input, "ACTION") == 0)
		{
			for (int i = 0; i < real_item_cnt; i++)
			{
				printf("%d :", i);
				for (int j = 0; j < terminal_num; j++)
				{
					if (action_item_num[i][j] != -1)
						printf("%c[%c%d]", terminal_array[j], slr_action_table[i][j * 2], action_item_num[i][j]);
				}
				printf("\n");
			}
		}
		else if (strcmp(input, "GOTO") == 0)
		{
			for (int i = 0; i < real_item_cnt; i++)//12
			{
				printf("%d :", i);
				for (int j = 0; j < non_t_count; j++)
				{
					if (slr_goto_table[i][j] != -1)
						printf("%c[%d] ", nont_node[j], slr_goto_table[i][j]);
				}
				printf("\n");
			}
		}
		else if (strcmp(input, "exit") == 0)
		{
			break;
		}
		else if (input[0] == 'I')//수정해야 한다.
		{
			char* num;
			int item_number;
			int index = 1;
			num = (char*)malloc(sizeof(char) * 10);
			for (int i = 1; i < 10; i++)
			{
				num[i - 1] = input[i];
				if (input[i] == '\0')
				{
					break;
				}
			}
			item_number = atoi(num);
			if (item_number >= real_item_cnt)
				printf("값을 다시 확인하세요.");
			else
			{
				while (result_arr[item_number][index++] != '\0')
				{
					if (result_arr[item_number][index] == ',')
					{
						printf("\n");
						continue;
					}
					printf("%c", result_arr[item_number][index]);
				}
				printf("\n");
			}
		}
		else
		{
			char* check_string = (char*)malloc(sizeof(char) * BUF_SIZE);
			int input_index = 0;
			while (input[input_index] != '\0') {
				check_string[input_index] = input[input_index];
				input_index++;
			}
			check_string[input_index++] = '$';
			check_string[input_index++] = '\0';
			int* prod_remove_cnt;
			prod_remove_cnt = (int*)malloc(sizeof(int) * prod_num / 2);
			for (int i = 0; i < prod_num / 2; i++)//prod_remove_cnt DATA
			{
				int index = 2;//> 이후 SEARCH
				while (prod_arr[(i * 2) + 1][++index] != '\n');
				prod_remove_cnt[i] = (index - 2) * 2;
			}
			int stack_num[BUF_SIZE];
			char stack_word[BUF_SIZE];
			int stack_num_index = 0;
			int stack_word_index = 1;
			int top_index = 0;
			int slr_action_col_index = -1;
			stack_num[stack_num_index] = 0;
			stack_num_index += 2;
			int check_str_index = 0;
			while (check_string[check_str_index] != '\0')
			{
				for (int i = 0; i < terminal_num; i++)//ACTION TABLE COL 찾기
				{
					if (check_string[check_str_index] == terminal_array[i]) {
						slr_action_col_index = i;
						break;
					}
				}
				if (slr_action_table[stack_num[top_index]][slr_action_col_index * 2] == 's')
				{
					stack_word[stack_word_index] = check_string[check_str_index];
					stack_word_index += 2;
					stack_num[stack_num_index] = action_item_num[stack_num[top_index]][slr_action_col_index];
					top_index = stack_num_index;
					stack_num_index += 2;
					check_str_index++;
					continue;
				}
				if (slr_action_table[stack_num[top_index]][slr_action_col_index * 2] == 'r')
				{
					int prod_arr_index = prod_remove_cnt[(action_item_num[stack_num[top_index]][slr_action_col_index]) - 1];
					stack_word_index -= prod_arr_index;
					stack_num_index -= prod_arr_index;
					for (int i = 0; i < non_t_count; i++)
					{
						if (nont_node[i] == prod_arr[(action_item_num[stack_num[top_index]][slr_action_col_index] * 2) - 1][0])
						{
							stack_word[stack_word_index] = nont_node[i];
							stack_word_index += 2;
							stack_num[stack_num_index] = slr_goto_table[stack_num[stack_num_index - 2]][i];
							top_index = stack_num_index;
							stack_num_index += 2;
							break;
						}
					}
					continue;
				}
				if (slr_action_table[stack_num[top_index]][slr_action_col_index * 2] == 'a')
				{
					printf("ACCEPT\n");
					break;
				}
				if (slr_action_table[stack_num[top_index]][slr_action_col_index * 2] == '_'
					|| slr_action_col_index == -1)
				{
					printf("ERROR\n");
					break;
				}
			}
		}
	}
	return arr;
}
char* insert_dot(char arr[])//Production내에서 .이 없을 때 처음 삽입
{
	char* result_arr;
	result_arr = (char*)malloc(sizeof(char) * BUF_SIZE);
	int result_index = 3;
	int arr_index = 0;
	arr_index = 2;
	result_arr[0] = arr[0];
	result_arr[1] = arr[1];
	while (1)
	{
		if (arr[arr_index] == '\n' || arr[arr_index] == '\0')
			break;
		result_arr[result_index++] = arr[arr_index++];
	}
	result_arr[2] = '.';
	return result_arr;
}
char* move_dot(char arr[])//Production내에서 .이 있을 때 이동
{
	char temp = ' ';
	int arr_index = 0;
	while (arr[arr_index] != '.') arr_index++;
	if (arr[arr_index + 1] == '\0' || arr[arr_index + 1] == '\n')
		return arr;
	else
	{
		temp = arr[arr_index];
		arr[arr_index] = arr[arr_index + 1];
		arr[arr_index + 1] = temp;
		return arr;
	}
}
char* find_goto_set(char arr[], int arr_length)//goto_set에서 .A A와 관련된 모든 Prod 생성
{
	int count = 0;
	int change = 1;
	int index = 0;
	int row_index = 0;
	char nont_check[MAX_NON_TERMINAL_SYMBOL];
	int nont_check_index = 0;

	char nont = ' ';
	while (change == 1)
	{
		change = 0;
		while (1)
		{
			if (index == arr_length) {
				break;
			}
			if (arr[index] == '.' && (arr[index + 1] >= 'A' &&
				arr[index + 1] <= 'Z')) {
				nont = arr[index + 1];
				if (nont_check_index == 0)
				{
					change = 1;
					break;
				}
				for (int i = 0; i < nont_check_index; i++)
				{
					if (nont != nont_check[i])
					{
						change = 1;
						break;
					}
				}
			}
			index++;
		}
		if (change == 0)
		{
			arr[arr_length] = '\0';
			break;
		}
		for (int i = 1; i < prod_num; i += 2)
		{
			if (nont == prod_arr[i][0])
			{
				int prod_arr_index = 0;
				count = 0;
				while (1)
				{

					if (prod_arr[i][prod_arr_index] == '\0' || prod_arr[i][prod_arr_index] == '\n')
						break;
					if (count == 2) arr[++arr_length] = '.';
					else arr[++arr_length] = prod_arr[i][prod_arr_index++];
					count++;
				}
				arr[++arr_length] = ',';
			}
			if (i == 1)
			{
				nont_check[nont_check_index++] = nont;
			}
		}
	}
	return arr;
}