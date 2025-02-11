#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SQUARE 128


// 1〜4人の整数でプレイヤー数の指定をおこなう
int select_num_players()
{
    int num_players;

    do
    {
        printf("プレイヤー数を選択 【1~4】: ");
        scanf("%d", &num_players);
        if (num_players < 1 || num_players > 4)
        {
            printf("1~4の整数で入力してください\n\n");
        }
    } while (num_players < 1 || num_players > 4);

    return num_players;
}

// 進むマスを選択
int input_steps(int sum_dice)
{
    int steps;

    do
    {
        printf("最大%dマス進めます。\n\n何マス進みますか？ 【0~%d】: ", sum_dice, sum_dice);
        scanf("%d", &steps);
        if (steps < 0 || steps > sum_dice)
        {
            printf("0~%dの整数で入力してください: ", sum_dice);
        }
    } while (steps < 0 || steps > sum_dice);

    return steps;
}

void set_stage(int *stage)
{
    ;
}

void print_stage_all(int len_stage, int *stage)
{
    for (int i = 0; i < len_stage; i++)
    {
        printf("%d ", stage[i]);
    }
    putchar('\n');
}

void print_stage(int *pos, int max_square)
{
    putchar('\n');
    for (int i = 0; i < max_square; i++)
    {
        if (pos[0] == i || pos[1] == i || pos[2] == i || pos[3] == i)
        {
            putchar('!');
        }
        else
        {
            putchar('_');
        }
    }
    printf("|GOAL|\n\n");
}

void print_stage_forward(int pos, int *stage)
{
    printf("先の4マス（□は何もなし　■はなにかあるかも）\n");
    for (int i = pos; i < pos + 4; i++)
    {
        if (i >= MAX_SQUARE)
        {
            printf("|GOAL|");
            break;
        }

        if (stage[i] == 0)
        {
            printf("□ ");
        }
        else
        {
            printf("■ ");
        }
    }
    putchar('\n');
}

int roll_dice(int num_dice)
{
    static int flag;
    int dice;
    int sum_dice = 0;

    if (flag == 0) {
        srand((unsigned int)time(NULL));
        flag = 1;
    }

    // 1から6までの乱数を num_dice 回だけ生成
    for(int i = 0; i < num_dice; i++)
    {
        dice = rand() % 6 + 1;
        printf("サイコロ%d: %d    ", i+1, dice);
        sum_dice += dice;
    }

    return sum_dice;
}


int main(void)
{
    int mode_debug = 1;

    int num_players;
    int steps;
    int pos[4] = {0};
    int stage[MAX_SQUARE] = {0};
    int turn = 0;

    int num_dice = 2;       // 一度に振れるサイコロの数

    printf("=== すごろく ===\n");
    num_players = select_num_players();

    set_stage(stage);

    if(mode_debug)
        print_stage_all(MAX_SQUARE, stage);

    // pos[i] が MAX_SQUARE 以上になるまでループ
    while (pos[0] < MAX_SQUARE && pos[1] < MAX_SQUARE && pos[2] < MAX_SQUARE && pos[3] < MAX_SQUARE)
    {
        turn++;
        printf("\n【%dターン】\n", turn);
        print_stage(pos, MAX_SQUARE);
        for (int i = 0; i < num_players; i++)
        {
            printf("\nプレイヤー%dの番です\n", i + 1);
            print_stage_forward(pos[i], stage);
            steps = input_steps(roll_dice(num_dice));
            printf("%dマス進みます\n", steps);
            pos[i] += steps;
        }
        for (int i = 0; i < 4 - num_players; i++)
        {
            printf("\nCOM%dの番です\n", i + 1);
            steps = roll_dice(num_dice);
            printf("%dマス進みます\n", steps);
            pos[i + num_players] += steps;
        }
    }

    printf("\n\n----------------------------------------\n最終結果");
    print_stage(pos, MAX_SQUARE);
    for (int i = 0; i < num_players; i++)
    {
        if (pos[i] >= MAX_SQUARE)
            printf("プレイヤー%dはゴールしました！\n", i + 1);
        else
            printf("プレイヤー%dはゴールまであと%dマスでした\n", i + 1, MAX_SQUARE - pos[i]);
    }
    for (int i = 0; i < 4 - num_players; i++)
    {
        if (pos[i + num_players] >= MAX_SQUARE)
            printf("COM%dはゴールしました！\n", i + 1);
        else
            printf("COM%dはゴールまであと%dマスでした\n", i + 1, MAX_SQUARE - pos[i + num_players]);
    }

    return 0;
}