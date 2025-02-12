// C言語を使ったすごろくゲーム

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ゴールまでのマスの数
#define MAX_SQUARE 100

// TODO: プレイヤーを構造体で作成し、4要素の1次元配列に格納する

// 操作プレイヤー数の指定（1〜4人）
int select_num_players(int *mode_debug)
{
    int num_players;
    // 1~4あるいは914の整数をプレイヤーに入力してもらう
    do
    {
        printf("プレイヤー数を選択 【1~4】: ");
        scanf("%d", &num_players);
        if (num_players < 1 || (4 < num_players && num_players < 914) || 914 < num_players)
        {
            printf("1~4の整数で入力してください\n\n");
        }
    } while (num_players < 1 || (4 < num_players && num_players < 914) || 914 < num_players);

    // 914が入力された場合、操作プレイヤーは1人としてデバッグモードに入れるようにする
    if (num_players == 914)
    {
        num_players = *mode_debug = 1;
    }

    return num_players;
}

// 何マス進むかを選択
int input_steps(int sum_dice)
{
    int steps = 0;

    // サイコロの数が0の場合（1回休み）なにもしない
    if (sum_dice)
    {
        // 0~サイコロの出目の合計の整数をプレイヤーに入力してもらう
        do
        {
            printf("最大%dマス進めます。\n\n何マス進みますか？ 【0~%d】: ", sum_dice, sum_dice);
            scanf("%d", &steps);
            if (steps < 0 || steps > sum_dice)
            {
                printf("0~%dの整数で入力してください: ", sum_dice);
            }
        } while (steps < 0 || steps > sum_dice);
    }

    return steps;
}

// 重複を防ぎつつ、効果付きのマスを指定する
void assign_unique_random(int *sq, int count, int *used) {
    int num;
    for (int i = 0; i < count; i++) {
        do {
            num = rand() % MAX_SQUARE;
        } while (used[num]); // 重複していれば再生成
        sq[i] = num;        // 重複していなければ配列に追加
        used[num] = 1;    // 使用済みとしてマーク
    }
}

// ステージのセット
void set_stage(int *stage)
{
    static int flag;
    if (flag == 0) {
        srand((unsigned int)time(NULL));
        flag = 1;
    }

    // それぞれの効果マスの数を指定
    int sqnum_move = MAX_SQUARE / 4;
    int sqnum_freeze = MAX_SQUARE / 4;
    int sqnum_dice = MAX_SQUARE / 10;

    int sq_move[MAX_SQUARE / 4] = {0};
    int sq_freeze[MAX_SQUARE / 4] = {0};
    int sq_dice[MAX_SQUARE / 10] = {0};
    int sq_restart[1] = {0};    // 振り出しにもどるマスは1つのみ
    int used[MAX_SQUARE] = {0};  // 使用済みマスチェック用の配列

    // 変更するマスの番号を選択
    // 移動マス
    assign_unique_random(sq_move, sqnum_move, used);
    // 休みマス
    assign_unique_random(sq_freeze, sqnum_freeze, used);
    // サイコロマス
    assign_unique_random(sq_dice, sqnum_dice, used);
    // 振り出しにもどるマス
    assign_unique_random(sq_restart, 1, used);

    // マスの番号を変更（1: 移動　2: 休み　3: サイコロ数変更　4: 振り出しに戻る）
    for (int i = 0; i < sqnum_move; i++)
    {
        stage[sq_move[i]] = 1;
    }
    for (int i = 0; i < sqnum_freeze; i++)
    {
        stage[sq_freeze[i]] = 2;
    }
    for (int i = 0; i < sqnum_dice; i++)
    {
        stage[sq_dice[i]] = 3;
    }
    stage[sq_restart[0]] = 4;
}

// デバッグ用：ステージのマスを全て出力
void print_stage_all(int len_stage, int *stage)
{
    for (int i = 0; i < len_stage; i++)
    {
        printf("%d ", stage[i]);
    }
    putchar('\n');
}

// ステージ全体とプレイヤーの位置を出力
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

// プレイヤーの現在地からサイコロの目の8割先までのマスの内容を出力
void print_stage_forward(int pos, int sum_dice, int *stage)
{
    int num_block = sum_dice * 4 / 5;
    printf("先の%dマス（□: 何もなし　■: なにかが起こる）\n", num_block);
    for (int i = pos + 1; i < pos + 1 + num_block; i++)
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

// サイコロを振る
int roll_dice(int num_dice)
{
    int dice;
    int sum_dice = 0;

    if (num_dice)
    {
        // 1から6までの乱数を num_dice 回だけ生成
        for(int i = 0; i < num_dice; i++)
        {
            dice = rand() % 6 + 1;
            printf("サイコロ%d: %d    ", i+1, dice);
            sum_dice += dice;
        }
    }
    else
    {
        printf("お休みのターンです zzz……\n");
    }

    return sum_dice;
}

// 現在のマスの評価
void evaluate_square(int player, int *dice, int *pos, int *freeze, int *stage)
{
    printf("止まったマスの効果: ");
    if (pos[player] >= MAX_SQUARE)
        printf("ゴール！\n");
    else if (stage[pos[player]] == 1)   // 移動マスに止まった場合
    {
        int move_step = rand() % 6 + 1;
        if (move_step > 3)
        {
            move_step -= 7;
            printf("%dマスもどる\n", move_step * -1);
        }
        else
        {
            printf("%dマス進む\n", move_step);
        }
        pos[player] = pos[player] + move_step < 0 ? 0 : pos[player] + move_step;
    }
    else if (stage[pos[player]] == 2)   // 休みマスに止まった場合
    {
        freeze[player] ^= 1;
        if (freeze[player])
        {
            dice[player] = 0;
            printf("1回休み\n");
        }
        else
        {
            printf("お休み中です zzz……\n");
        }
    }
    else if (stage[pos[player]] == 3)   // サイコロマスに止まった場合
    {
        int dice_change = rand() % 2;
        if (dice_change)
        {
            dice[player]++;
            printf("次のターンサイコロが3つに\n");
        }
        else
        {
            dice[player]--;
            printf("次のターンサイコロが1つに\n");
        }
    }
    else if (stage[pos[player]] == 4)   // 振り出しにもどるマスに止まった場合
    {
        pos[player] = 0;
        printf("\n\n                 振り出しにもどる……！！\n\n");
    }
    else
    {
        printf("なし\n");
    }
}

void comfirm_res(void)
{
    int flag = 0;
    do
    {
        printf("\n結果が確認できたら 1 を入力してください: ");
        scanf("%d", &flag);
    } while (flag == 0);
}


int main(void)
{
    srand((unsigned int)time(NULL));

    int mode_debug = 0;

    int num_players;
    int sum_dice;
    int steps;

    int dice[4] = {2, 2, 2, 2};
    int pos[4] = {0};
    int freeze[4] = {0};

    int stage[MAX_SQUARE] = {0};
    int turn = 0;

    printf("=== すごろく ===\n");
    num_players = select_num_players(&mode_debug);

    set_stage(stage);

    if(mode_debug)
    {
        printf("デバッグモードで起動します\n");
        print_stage_all(MAX_SQUARE, stage);
        comfirm_res();
    }

    // pos[i] が MAX_SQUARE 以上になるまでループ
    while (pos[0] < MAX_SQUARE && pos[1] < MAX_SQUARE && pos[2] < MAX_SQUARE && pos[3] < MAX_SQUARE)
    {
        turn++;
        printf("\n【%dターン】\n", turn);
        print_stage(pos, MAX_SQUARE);
        for (int i = 0; i < num_players; i++)
        {
            printf("\nPLAYER%dの番です ( %d / %d )\n", i + 1, pos[i], MAX_SQUARE);
            sum_dice = roll_dice(dice[i]);
            print_stage_forward(pos[i], sum_dice, stage);
            steps = input_steps(sum_dice);
            pos[i] += steps;
            printf("%dマス進みます\n\n", steps);
            dice[i] = 2;
            evaluate_square(i, dice, pos, freeze, stage);
            comfirm_res();
        }
        printf("\n*******************************************\n");
        for (int i = num_players; i < 4; i++)
        {
            printf("\nCOM%d ( %d / %d )\n", i - num_players + 1, pos[i], MAX_SQUARE);
            steps = roll_dice(dice[i]);
            printf("%dマス進みました\n", steps);
            pos[i] += steps;
            dice[i] = 2;
            evaluate_square(i, dice, pos, freeze, stage);
        }
    }

    printf("\n\n*******************************************\n最終結果\n");
    print_stage(pos, MAX_SQUARE);
    for (int i = 0; i < num_players; i++)
    {
        if (pos[i] >= MAX_SQUARE)
            printf("PLAYER%d : ゴールしました！！\n", i + 1);
        else
            printf("PLAYER%d : あと %3d マスでした\n", i + 1, MAX_SQUARE - pos[i]);
    }
    for (int i = 0; i < 4 - num_players; i++)
    {
        if (pos[i + num_players] >= MAX_SQUARE)
            printf("COM%d    : ゴールしました！！\n", i + 1);
        else
            printf("COM%d    : あと %3d マスでした\n", i + 1, MAX_SQUARE - pos[i + num_players]);
    }
    printf("\n*******************************************\nThank you!\n\n");

    return 0;
}