#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gqueue.h"

typedef enum {
		closed, marked, open
	}
	cell_state ;

typedef struct {
		int mined ;
		int num ;
		cell_state state ; 
	}
	cell_t  ;


int M, N, K ;
int turn= 0;
cell_t board[16][16] = {{ {0, 0, 0} }} ;

void load_board(char *filename) {
    FILE *fp;
    // 파일 열기 시도
    if (!(fp = fopen(filename, "r"))) {
        fprintf(stderr, "Fail to open %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // 보드의 크기와 지뢰의 개수 읽기
    if (fscanf(fp, "%d %d %d", &M, &N, &K) != 3) {
        fprintf(stderr, "File format error\n");
        exit(EXIT_FAILURE);
    }

    // 보드 정보 읽기
    for (int i = 0; i < K; i++) {
        int row, col;
        // 파일에서 각 지뢰의 위치를 읽어와서 배열에 표시
        if (fscanf(fp, "%d %d", &row, &col) != 2) {
            fprintf(stderr, "File format error\n");
            exit(EXIT_FAILURE);
        }
        board[row][col].mined = 1; // 해당 위치에 지뢰 표시
    }

    fclose(fp);
}

int is_terminated()
{
    int result = 1; // 기본적으로 모든 셀이 닫혀있다고 가정합니다.

    for (int row = 0; row < M; row++)
    {
        for (int col = 0; col < N; col++)
        {
            if (board[row][col].state == open && board[row][col].mined)
            {
                return 1; // 열린 지뢰가 있는 경우, 게임 종료
            }
            else if (board[row][col].state == closed && !board[row][col].mined)
            {
                result = 0; // 닫힌 셀 중에 지뢰가 없는 경우
            }
        }
    }


    return result;
}

void draw_board ()
{
	int col = 0;
	int row = 0;

	printf("  ");
    for (col = 0; col < N; col++) {
        printf("%2d", col); // 열 번호 출력
    }
    printf("\n");

	for (row = 0 ; row < M ; row++) {
		printf("%2d ", row); // 행 번호 출력
		for (col = 0 ; col < N ; col++)
		{
			if (board[row][col].state == closed)
			{
				printf("+ ");
			}
			else if (board[row][col].state == marked)
			{
				printf("! ");
			}
			else if (board[row][col].state == open)
			{
				if (board[row][col].mined)
				{
					printf("* ");
				}
				else
				{
					printf("%d ", board[row][col].num) ;
				}
			}
		}
		printf("\n");
	}
}


void open_mine(int row, int col)
{
    // 범위를 벗어나거나 이미 열린 셀인 경우 종료
    if (row < 0 || row >= M || col < 0 || col >= N || board[row][col].state == open)
    {
        return;
    }

    // 셀을 열기
    board[row][col].state = open;

	turn ++;

    // 만약 해당 셀이 지뢰가 아닌 경우 주변에 지뢰가 있는지 확인하고 지뢰의 개수를 표시
    if (!board[row][col].mined)
    {
        // 인접한 8칸을 탐색하여 지뢰의 개수 확인
        int minesCount = 0;
        for (int dr = -1; dr <= 1; dr++)
        {
            for (int dc = -1; dc <= 1; dc++)
            {
                int newRow = row + dr;
                int newCol = col + dc;
                if (newRow >= 0 && newRow < M && newCol >= 0 && newCol < N && board[newRow][newCol].mined)
                {
                    minesCount++;
                }
            }
        }
        // 인접한 위치에 지뢰가 있으면 해당 셀에 지뢰의 개수 표시
        if (minesCount > 0)
        {
            board[row][col].num = minesCount;
        }
        else
        {
            // 주변에 지뢰가 없는 경우 BFS로 인접한 셀 열기
            for (int dr = -1; dr <= 1; dr++)
            {
                for (int dc = -1; dc <= 1; dc++)
                {
                    int newRow = row + dr;
                    int newCol = col + dc;
                    if (newRow >= 0 && newRow < M && newCol >= 0 && newCol < N && board[newRow][newCol].state != open)
                    {
                        open_mine(newRow, newCol);
                    }
                }
            }
        }
    }
}


void read_execute_userinput ()
{	
	printf(">");

	char command[16];
	int col, row;
	scanf("%15s %d %d", command, &row, &col);

	if (strcmp(command, "open") == 0)
	{
		open_mine(row, col);
	}
	else if (strcmp(command, "mark") == 0)
	{
		board[row][col].state = marked;
	}
	else if (strcmp(command, "unmark") == 0)
	{
		board[row][col].state = closed;
	}
	else
	{
		printf("다시 입력해 주세요.\n");
	}
}

void save_ranking(char * filename,char *nickname, int turn)
{
    FILE *fp;
    fp = fopen(filename, "a"); // 랭킹을 기록할 파일 열기 (append 모드)
    if (fp == NULL)
    {
        printf("파일 정보를 가져올 수 없습니다.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%s %d\n", nickname, turn); // 닉네임과 턴 수를 파일에 기록
    fclose(fp);
}

void show_ranking(char *filename)
{
    FILE *fp;
    fp = fopen(filename, "r"); // 랭킹 파일 열기 (읽기 모드)
    if (fp == NULL)
    {
        printf("불러올 랭킹정보가 없습니다.\n");
        return;
    }

    // 각 랭킹 항목을 읽어와서 출력
    char nickname[100];
    int turn;
    printf("Ranking:\n");
    while (fscanf(fp, "%s %d", nickname, &turn) != EOF)
    {
        printf("%s - %d turns\n", nickname, turn);
    }

    fclose(fp);
}

int main (int argc, char ** argv)
{
	if (argc != 3) {
		fprintf(stderr, "Invalid command: give a filename of board arrangement.\n") ;
		return EXIT_FAILURE ;
	}

	load_board(argv[1]) ;


    printf("1. 지뢰찾기 플레이\n");
    printf("2. 랭킹 보기\n");
    printf("1이나 2를 입력하세요: ");

    int choice;
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
        // Minesweeper 게임 플레이
		printf("open으로 지뢰가 없는 칸을 열 수 있습니다. 지뢰를 밟지 않고 모든 지뢰 위치를 찾아보세요!\nopen을 하게되면 인접해있는 지뢰의 수가 셀에 표시됩니다.\n");
		printf("mark를 이용해 지뢰가 있을 만한 곳을 미리 체크할 수 있습니다. unmark를 이용해 체크를 해제할 수도 있죠.\n");
		printf("다음은 입력 예시입니다. open 1 1, mark 3 0, unmark 3 0\n");
		printf("게임을 클리어하면 본인의 플레이를 기록해 남길 수 있습니다! 그럼 건투를 빕니다.\n");
        while (!is_terminated())
        {
            draw_board();
            read_execute_userinput();
        }
        if (is_terminated() == 0)
        {
            printf("You win!\n");
			printf("게임 플레이에 걸린 턴 수: %d",turn);
            printf("Enter your nickname: ");
            char nickname[100];
            scanf("%s", nickname);
            save_ranking(argv[2],nickname, turn); // 클리어한 경우 랭킹에 저장
        }
        else if (is_terminated() == 1)
        {
            printf("Game over\n");
            draw_board();
        }
        break;
    case 2:
        // 랭킹 표시
        show_ranking(argv[2]);
        break;
    default:
        printf("Invalid choice\n");
    }

	return EXIT_SUCCESS ;
}
