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
		printf("Invalid command\n");
	}
}

int main (int argc, char ** argv)
{
	if (argc != 2) {
		fprintf(stderr, "Invalid command: give a filename of board arrangement.\n") ;
		return EXIT_FAILURE ;
	}

	load_board(argv[1]) ;

	while (!is_terminated()) {
		draw_board() ;
		read_execute_userinput() ;
	}
	if (is_terminated() == 0)
	{
		printf("You win!\n");
	}
	else if (is_terminated() == 1)
	{
		printf("Game over\n");
		draw_board();
	}
	printf("게임 플레이에 걸린 턴 수: %d",turn);

	return EXIT_SUCCESS ;
}
