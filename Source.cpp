#//include "header.h"
#pragma comment(lib, "bdd.lib")
#include "bdd.h"
#include <fstream>


using namespace std;

//#define K 21
#define K 17
// при изменении N, мы считаем, что поле остается квадратным
#define N 9		// число объектов
#define M 4			// число свойств
#define LOG_N 4
#define N_VAR N*M*LOG_N	// число булевых переменных
ofstream out;

// вводим  p[M][N][N], где M - номер св-ва (1..4), N1 - номер объекта, N2 - св-во
bdd p[M][N][N];
bdd task;

void fun(char* varset, int size); // функция, используемая для вывода решений

//объект со значением N1 в свойстве M1 стоит слева - сверху от объекта со значением N2 в свойстве M2
bdd up_left(int M1, int N1, int M2, int N2);

//объект со значением N1 в свойстве M1 стоит справа - сверху от объекта со значением N2 в свойстве M2
bdd up_right(int M1, int N1, int M2, int N2);


void main(void)
{
	// инициализация
	bdd_init(10000000, 1000000);
	bdd_setvarnum(N_VAR);


	// инициализация p[k][i][j]
	for (int k = 0; k < M; ++k)
	{
		int delta = 0;
		for (auto i = 0; i < N; ++i)
		{
			// цикл свойств
			for (auto j = 0; j < N; ++j)
			{
				p[k][i][j] = bddtrue;
				for (auto l = 0; l < LOG_N; ++l)
					if (j >> l & 1)
						p[k][i][j] &= bdd_ithvar(delta + LOG_N * k + l);
					else
						p[k][i][j] &= bdd_nithvar(delta + LOG_N * k + l);
			}
			delta += LOG_N * M;
		}
	}

	task = bddtrue;

	// ограничение на количество переменных почти 6
	for (int k = 0; k < M; ++k)
	{
		for (int i = 0; i < N; ++i)
		{
			bdd temp = bddfalse;
			for (int j = 0; j < N; ++j)
				temp |= p[k][i][j];
			task &= temp;
		}
	}

	// ограничение n1

	// св-во 3 у 7-го объекта равно 8
	task &= p[3][7][8];
	// св-во 3 у 1-го объекта равно 2
	task &= p[3][1][2];


	// ограничение n2

	//если у объекта i второе сво-во равно 5, то у него нулевое св-во равно 0
	for (int i = 0; i < N; ++i) task &= !(p[2][i][5] ^ p[0][i][0]);
	//если у объекта i второе сво-во равно 3, то у него третье св-во равно 5
	for (int i = 0; i < N; ++i)	task &= !(p[2][i][3] ^ p[3][i][5]);
	//если у объекта i первое сво-во равно 3, то у него второе св-во равно 8
	for (int i = 0; i < N; ++i) task &= !(p[1][i][3] ^ p[2][i][8]);
	//если у объекта i первое сво-во равно 2, то у него нулевое св-во равно 1
	for (int i = 0; i < N; ++i) task &= !(p[1][i][2] ^ p[0][i][1]);

	// ограничение n3

	//	 объект со первым свойством 1 слева-вверху от объекта с первым свойством 5
	task &= up_left(1, 1, 1, 5);

	// объект с нулевым свойством 4 справа сверху от объектом с первым свойством 7
	task &= up_right(0, 4, 1, 7);

	//	 объект со вторым свойством 4 слева-вверху от объекта с первым свойством 6
	task &= up_left(2, 4, 1, 6);

	// объект со вторым свойством 4 справа сверху от объектом с первым свойством 4
	task &= up_right(2, 4, 1, 4);

	// объект с нулевым свойством 5 справа сверху от объектом с первым свойством 8
	task &= up_right(0, 5, 1, 8);

	//task &= up_right(0, 1, 0, 2);
	//task &= up_right(3, 7, 3, 1);






	// ограничение n4

	//	объект со значением 5 в свойстве 1 стоит слева - сверху или справа - сверху от героя со значением 8 в свойстве 0
	task &= (up_left(1, 5, 0, 8) | up_right(1, 5, 0, 8));

	//	объект со значением 4 в свойстве 2 стоит слева - сверху или справа - сверху от героя со значением 2 в свойстве 0
	task &= (up_left(2, 4, 0, 2) | up_right(2, 4, 0, 2));

	//	объект со значением 5 в свойстве 1 стоит слева - сверху или справа - сверху от героя со значением 4 в свойстве 3
	task &= (up_left(1, 5, 3, 4) | up_right(1, 5, 3, 4));

	//	объект со значением 4 в свойстве 2 стоит слева - сверху или справа - сверху от героя со значением 3 в свойстве 3
	task &= (up_left(2, 4, 3, 3) | up_right(2, 4, 3, 3));

	//доп
	{
		task &= p[2][3][6];
		task &= p[2][7][0];
		task &= p[0][6][7];
		task &= p[3][0][0];

		for (int i = 0; i < N; ++i) task &= !(p[3][i][0] ^ p[2][i][5]);
		for (int i = 0; i < N; ++i)	task &= !(p[2][i][2] ^ p[3][i][6]);
		for (int i = 0; i < N; ++i)	task &= !(p[3][i][7] ^ p[0][i][3]);
		for (int i = 0; i < N; ++i)	task &= !(p[2][i][6] ^ p[3][i][1]);


		//	 объект с нулевым свойством 0 слева-вверху от объекта с третьим свойством 5
		task &= up_left(0, 0, 3, 5);

		//	 объект с первым свойством 4 слева-вверху от объекта с первым свойством 8
		task &= up_left(1, 4, 1, 8);

		// объект с третьим свойством 3 справа сверху от объектом с первым свойством 8
		task &= up_right(3, 3, 1, 8);


	}

	//ограничение по-умолчанию 5 (на повторение )
	for (unsigned j = 0; j < N; j++) {
		for (unsigned i = 0; i < N - 1; i++)
			for (unsigned k = i + 1; k < N; k++)
			{
				task &= p[0][i][j] >> !p[0][k][j];
				task &= p[1][i][j] >> !p[1][k][j];
				task &= p[2][i][j] >> !p[2][k][j];

			}
	}

	// n7

	// * - *
	// - 0 -
	// - - -

	// * * *
	// * * *
	// - - -

	// i + j + k + p > K
	// task &= !(p[0][сосед][i] & p[1][сосед][j] & p[2][сосед][k] & p[3][сосед][p])
	for (int object = 0; object < 6; object++)
		for (int i = 0; i < N; ++i)
			for (int j = 0; j < N; j++)
				for (int k = 0; k < N; k++)
					for (int pp = 0; pp < N; pp++)
						if ((i + j + k + pp) > K)
						{
							task &= !(p[0][object][i] & p[1][object][j] & p[2][object][k] & p[3][object][pp]);
						}


	// вывод результатов
	unsigned satcount = (unsigned)bdd_satcount(task);

	cout << satcount << " solutions:\n" << endl;
	if (satcount)
		bdd_allsat(task, fun);

	bdd_done(); // завершение работы библиотеки

	system("pause");
	return;
}

bdd up_left(int M1, int N1, int M2, int N2)
{

	bdd  temp2 = bddtrue;
	//	объект со значением N1 в свойстве M1 стоит слева - сверху от объекта со значением N2 в свойстве M2

	//для N = 9
	//temp2 &= (!p[M1][2][N1] & !p[M1][5][N1] & !p[M1][8][N1] & !p[M1][7][N1] & !p[M1][6][N1] & !p[M2][0][N2] & !p[M2][1][N2] & !p[M2][2][N2] & !p[M2][3][N2] & !p[M2][6][N2]);
	//temp2 &= !(p[M2][8][N2] ^ p[M1][4][N1]) & !(p[M2][4][N2] ^ p[M1][0][N1]) & !(p[M2][5][N2] ^ p[M1][1][N1]) & !(p[M2][7][N2] ^ p[M1][3][N1]);

	temp2 &= !p[M1][(int)(sqrt(N) * (sqrt(N) - 1))][N1] & !p[M2][0][N2];
	for (int i = 1; i < sqrt(N); i++)
		temp2 &= !p[M1][(int)sqrt(N) * i - 1][N1] & !p[M2][(int)sqrt(N) * i][N2] & !p[M1][(int)(sqrt(N) * (sqrt(N) - 1)) + i][N1] & !p[M2][i][N2];

	for (int i = 0; i < (sqrt(N) - 1); i++)
		for (int j = 0; j < (sqrt(N) - 1); j++)
			temp2 &= !(p[M1][(int)sqrt(N) * i + j][N1] ^ p[M2][(int)sqrt(N) * (i + 1) + j + 1][N2]);

	return temp2;
}

bdd up_right(int M1, int N1, int M2, int N2)
{
	bdd  temp1 = bddtrue;
	//	объект со значением N1 в свойстве M1 стоит справа - сверху от объекта со значением N2 в свойстве M2

	//для N = 9
	//temp1 &= (!p[M1][0][N1] & !p[M1][3][N1] & !p[M1][6][N1] & !p[M1][7][N1] & !p[M1][8][N1] & !p[M2][0][N2] & !p[M2][1][N2] & !p[M2][2][N2] & !p[M2][5][N2] & !p[M2][8][N2]);
	//temp1 &= !(p[M2][3][N2] ^ p[M1][1][N1]) & !(p[M2][4][N2] ^ p[M1][2][N1]) & !(p[M2][6][N2] ^ p[M1][4][N1]) & !(p[M2][7][N2] ^ p[M1][5][N1]);

	temp1 &= !p[M1][(int)(sqrt(N) * (sqrt(N) - 1))][N1] & !p[M2][0][N2];
	for (int i = 1; i < sqrt(N); i++)
		temp1 &= !p[M1][(int)sqrt(N) * i][N1] & !p[M2][(int)sqrt(N) * i - 1][N2] & !p[M1][(int)(sqrt(N) * (sqrt(N) - 1)) + i][N1] & !p[M2][i][N2];

	for (int i = 0; i < (sqrt(N) - 1); i++)
		for (int j = 1; j < sqrt(N); j++)
			temp1 &= !(p[M1][(int)sqrt(N) * i + j][N1] ^ p[M2][(int)sqrt(N) * (i + 1) + j - 1][N2]);


	return temp1;
}


char var[N_VAR];

void print(void)
{
	for (unsigned i = 0; i < N; i++)
	{
		cout << i << ": ";
		for (unsigned j = 0; j < M; j++)
		{
			int J = i * M * LOG_N + j * LOG_N;
			int num = 0;
			for (unsigned k = 0; k < LOG_N; k++) num += (unsigned)(var[J + k] << k);
			cout << num << ' ';
		}
		cout << endl;
	}
	cout << endl;
}

void build(char* varset, unsigned n, unsigned I)
{
	if (I == n - 1)
	{
		if (varset[I] >= 0)
		{
			var[I] = varset[I];
			print();
			return;
		}
		var[I] = 0;
		print();
		var[I] = 1;
		print();
		return;
	}
	if (varset[I] >= 0)
	{
		var[I] = varset[I];
		build(varset, n, I + 1);
		return;
	}
	var[I] = 0;
	build(varset, n, I + 1);
	var[I] = 1;
	build(varset, n, I + 1);
}

void fun(char* varset, int size)
{
	build(varset, size, 0);
	/*for (int i = 0; i < size; i++) cout << (varset[i] < 0 ? 'X' : (char)('0' + varset[i]));
	cout << endl;*/
}
