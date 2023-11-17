#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#define ROUND 3 /*Changing this value would result change in the number of output
bytes*/
#define bigint unsigned long long int
/*Defining the Global Variables A is a constant so wev initialize it. Modulus = 2^32
since all operations are done modulo 2^32. */
unsigned char** s;
bigint X[8], G[8], Key[8], C[8],
A[8] = { 0x4D34D34D,0xD34D34D3,0x34D34D34,0x4D34D34D,0xD34D34D3,0x34D34D34,0x4D34D34D,0xD34D34D3 }, cy = 0,
IV[4], Modulus = 4294967296;
/*show() takes an array of bigint as input and display them */
void show(bigint* elt, int I)
{
	int i, j;
	puts("");
	for (i = 0; i < I; ++i)
		printf("%x\n", elt[i]);
	puts("");
}
/*bintodec() converts a binary integer of some specified length into decimal */
long long unsigned int bintodec(unsigned char* bin, unsigned int size)
{
	int i, j, k;
	unsigned long long int temp = 0;
	for (i = 0; i < size; ++i)
	{
		temp = 2 * temp + bin[i];
	}
	return temp;
}
/*dectohex() converts a decimal int to a 8-digit hexadecimal int as a char array */
unsigned char* dectohex(unsigned long long int dec)
{
	int i = 0, j, k, hh;
	unsigned char* hex, tt;
	unsigned long long int temp = dec;
	hex = (char*)malloc(sizeof(char) * 8);
	for (i = 0; i < 8; ++i)
		hex[i] = '0';
	j = 0;
	// It uses the standard conversion routine i.e. dividing by base=16 and record the remainder
		while ((temp > 0) && (j < 8))
		{
			hh = temp % 16;
			if ((hh >= 0) && (hh <= 9))
				hex[j++] = '0' + hh;
			else if ((hh >= 10) && (hh <= 15))
				hex[j++] = 'A' + hh - 10;
			else
			{
				puts("Something wrong computing HEXCODE");
				exit(1);
			}
			temp = temp / 16;
		}
	for (i = 0; i < 4; ++i)
	{
		tt = hex[i];
		hex[i] = hex[7 - i];
		hex[7 - i] = tt;
	}
	return hex;
}
/*dectobin() is quite similar to dectohex() but much simpler. It converts a decimal
to binary form (char array) of specified bit*/
void dectobin(unsigned char* bin, unsigned long long int dec, int size)
{
	int i = 0, j, k;
	unsigned char tt;
	unsigned long long int temp = dec;
	for (i = 0; i < size; ++i)
		bin[i] = 0;
	j = 0;
	while ((temp > 0) && (j < size))
	{
		bin[j++] = (unsigned char)temp % 2;
		temp = temp / 2;
	}
	for (i = 0; i < (size / 2); ++i)
	{
		tt = bin[i];
		bin[i] = bin[size - 1 - i];
		bin[size - 1 - i] = tt;
	}
}
/*show_binary() takes big binary integer as 2-d array form and display them in
binary and coresponding hexa-decimal and decimal form*/
void show_binary(unsigned char** elt, int I, int J)
{
	int i, j;
	unsigned char* hex_temp;
	hex_temp = (unsigned char*)malloc(sizeof(unsigned char) * (J / 4));
	for (i = 0; i < I; ++i)
	{
		for (j = 0; j < J; ++j)
		{
			printf("%d", elt[i][j]);
		}
		printf(" ");
	}
	puts("");
	for (i = 0; i < I; i++)
	{
		hex_temp = dectohex(bintodec(elt[i], J));
		printf("%s\t%lld\n", hex_temp, bintodec(elt[i], J));
	}
	puts("");
}
display_bytewise(bigint B, unsigned int size)
{
	int i, j, k, l;
	unsigned char* temp_hex;
	j = size / 8;
	temp_hex = dectohex(B);
	for (k = 0; k < j; ++k)
	{
		for (l = 0; l < 2; ++l)
		{
			printf("%c", temp_hex[2 * (3 - k) + l]);
		}
		printf(" ");
	}
	puts(" ");
}
/*copy_bigint() copies a big integer to another (in array form)*/
void copy_bigint(bigint* s, bigint* t, int size)
{
	int i;
	for (i = 0; i < size; ++i)
		s[i] = t[i];
}
/*initialize() sets 0 to all bits of a large binary number (in 2-d array form)*/
void initialize(unsigned char** elt, int I, int J)
{
	int i, j;
	for (i = 0; i < I; ++i)
	{
		for (j = 0; j < J; ++j)
		{
			elt[i][j] = 0;
		}
	}
}
/*lrot_dec() rotates a 32-bit decimal number left by specified position*/
bigint lrot_dec(bigint var, unsigned int rot)
{
	int i, j, k;
	bigint t1, t2;
	unsigned char* temp, * result, * temp1;
	t1 = (var << rot) & (Modulus - 1);
	t2 = (var >> (32 - rot)) & ((Modulus - 1) >> (32 - rot));
	return (t1 + t2) % Modulus;
}
/*add2() takes two 32*8 bit big binary numbers and add them and also output the
boolean carry*/
int add2(bigint* v1, bigint* v2, unsigned long long int carry_in, bigint* result)
{
	int i, j, k;
	unsigned char temp, temp_cy = 0;
	unsigned long long int cy_dec = 0;
	unsigned long long int var_dec = 0;
	cy_dec = carry_in;
	for (i = 0; i < 8; ++i)
	{
		var_dec = v1[i] + v2[i] + cy_dec;
		if (var_dec >= Modulus)
			cy_dec = 1;
		else
			cy_dec = 0;
		var_dec = var_dec % Modulus;
		result[i] = var_dec;
	}
	return cy_dec;
}
/*compute_g() computes the g() values and update the global variable G according to
the specified equation*/
void compute_g()
{
	int i, j, k;
	unsigned long long int var1;
	unsigned long long int temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0, aux = 0;
	unsigned char* temp;
	temp = (char*)malloc(sizeof(char) * 32);
	for (i = 0; i < 8; ++i)
	{
		temp2 = X[i];
		temp1 = temp2 + C[i];
		temp1 = temp1 % Modulus;
		temp2 = temp1 * temp1;
		temp3 = temp2 >> 32;
		temp2 = temp2 % Modulus;
		temp4 = temp2 ^ temp3;
		temp4 = temp4 % Modulus;
		G[i] = temp4;
	}
}
/*NEXTSTATE() Update the gloabl state variable X and counter C according to
equations*/
void NEXTSTATE()
{
	bigint* c_prev;
	int i, j;
	unsigned long long int k = 0;
	unsigned char* temp1, * temp2;
	c_prev = (bigint*)malloc(sizeof(unsigned char*) * 8);
	temp1 = (unsigned char*)malloc(sizeof(unsigned char) * 32);
	temp2 = (unsigned char*)malloc(sizeof(unsigned char) * 32);
	copy_bigint(c_prev, C, 8);
	/*Update Counters : Add present counter value to constant A to store the
result in C*/
	k = add2(c_prev, A, cy, C);
	cy = k;/*Sets the global carry cy*/
	/*Update the variable G*/
	compute_g();
	/*Update X variable with newly calculated G and C values*/
	X[0] = G[0] + lrot_dec(G[7], 16) + lrot_dec(G[6], 16);
	X[1] = G[1] + lrot_dec(G[0], 8) + G[7];
	X[2] = G[2] + lrot_dec(G[1], 16) + lrot_dec(G[0], 16);
	X[3] = G[3] + lrot_dec(G[2], 8) + G[1];
	X[4] = G[4] + lrot_dec(G[3], 16) + lrot_dec(G[2], 16);
	X[5] = G[5] + lrot_dec(G[4], 8) + G[3];
	X[6] = G[6] + lrot_dec(G[5], 16) + lrot_dec(G[4], 16);
	X[7] = G[7] + lrot_dec(G[6], 8) + G[5];
	for (i = 0; i < 8; ++i)
	{
		X[i] = X[i] % Modulus;/*All additions are mod 2^32*/
	}
}
/*KEYSETUP() computes the initial X (global) and C (global) values with given Key
(global)*/
void KEYSETUP()
{
	unsigned char** x, ** c, ** key;/*Temporary binary representation of corresponding
	variables*/
	int i, j;
	x = (unsigned char**)malloc(sizeof(unsigned char*) * 8);
	for (i = 0; i < 8; ++i)
	{
		x[i] = (unsigned char*)malloc(sizeof(unsigned char) * 32);
	}
	initialize(x, 8, 32);
	c = (unsigned char**)malloc(sizeof(unsigned char*) * 8);
	for (i = 0; i < 8; ++i)
	{
		c[i] = (unsigned char*)malloc(sizeof(unsigned char) * 32);
	}
	key = (unsigned char**)malloc(sizeof(unsigned char*) * 8);
	for (i = 0; i < 8; ++i)
	{
		key[i] = (unsigned char*)malloc(sizeof(unsigned char) * 16);
	}
	for (i = 0; i < 8; ++i)
	{
		dectobin(key[i], Key[i], 16);/*Converting the global key (decimal) to
		local key (binary)*/
	}
	/*Initializing binary state variable x by key following specified equations*/
	for (i = 0; i < 8; ++i)
	{
		if (i % 2 == 0)
		{
			for (j = 0; j < 16; ++j)
				x[i][j] = key[(i + 1) % 8][j];
			for (j = 16; j < 32; ++j)
				x[i][j] = key[i][j - 16];
		}
		else
		{
			for (j = 0; j < 16; ++j)
				x[i][j] = key[(i + 5) % 8][j];
			for (j = 16; j < 32; ++j)
				x[i][j] = key[(i + 4) % 8][j - 16];
		}
		X[i] = bintodec(x[i], 32);/*Converting local x (binary) to global X
		(decimal)*/
	}
	/*Initializing binary counter variable c by key following specified
	equations*/
	for (i = 0; i < 8; ++i)
	{
		if (i % 2 == 0)
		{
			for (j = 0; j < 16; ++j)
				c[i][j] = key[(i + 4) % 8][j];
			for (j = 16; j < 32; ++j)
				c[i][j] = key[(i + 5) % 8][j - 16];
		}
		else
		{
			for (j = 0; j < 16; ++j)
				c[i][j] = key[i][j];
			for (j = 16; j < 32; ++j)
				c[i][j] = key[(i + 1) % 8][j - 16];
		}
		C[i] = bintodec(c[i], 32);/*Converting local c (binary) to global C
		(decimal)*/
	}
	/*Iterating the system by calling NEXTSTATE() 4 times*/
	for (i = 0; i < 4; ++i)
	{
		NEXTSTATE();
	}
	/*Updating the counter again to get rid of the possibility of recovering key
	by knowing counter*/
	for (i = 0; i < 8; ++i)
	{
		C[i] ^= X[(i + 4) % 8];
	}
}
/*KEYGEN() provides the user interface to choose between the given key. */
void KEYGEN()
{
	int i, j, k;
	unsigned long long int temp_key[8];
	/*Three different keys are hardcoded. User may change accordingly*/
	unsigned long long int key1[8] = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000 };
	unsigned long long int key2[8] = { 0xc3ac, 0xdc51, 0x62f1, 0x3bfc, 0x36fe,
	0x2e3d, 0x1329, 0x9128 };
	unsigned long long int key3[8] = { 0x0043, 0xc09b, 0xab01, 0xe9e9, 0xc733,
	0x87e0, 0x7415, 0x8395 };
	puts("Choose your key 1 2 3 :\n");
	scanf_s("%d", &k);
	if (k == 1)
	{
		for (i = 0; i < 8; ++i)
			temp_key[i] = key1[i];
	}
	else if (k == 2)
	{
		for (i = 0; i < 8; ++i)
			temp_key[i] = key2[i];
	}
	else if (k == 3)
	{
		for (i = 0; i < 8; ++i)
			temp_key[i] = key3[i];
	}
	else
	{
		puts("Press between 1,2,3");
		KEYGEN();
	}
	copy_bigint(Key, temp_key, 8);
	puts("-------------------------------");
	puts("Input Key:\n");
	for (i = 0; i < 8; ++i)
		display_bytewise(Key[i], 16);
	puts("-------------------------------");
}
/*IVSETUP() modifies the master state by modifying counter C (global) and also
serves the User interface to choose between three different IVs*/
void IVSETUP()
{
	int i, j, k;
	/*Three different IVs hard-coded. User may change them if necessary*/
	unsigned long long int IV1[4] = { 0x0000, 0x0000, 0x0000, 0x0000 };
	unsigned long long int IV2[4] = { 0x7e59, 0xc126, 0xf575, 0xc373 };
	unsigned long long int IV3[4] = { 0x1727, 0xd2f4, 0x561a, 0xa6eb };
	puts("Choose your IV 1 2 3:\n");
	scanf_s("%d", &k);
	if (k == 1)
	{
		for (i = 0; i < 4; ++i)
			IV[i] = IV1[i];
	}
	else if (k == 2)
	{
		for (i = 0; i < 4; ++i)
			IV[i] = IV2[i];
	}
	else if (k == 3)
	{
		for (i = 0; i < 4; ++i)
			IV[i] = IV3[i];
	}
	else
	{
		puts("Press between 1,2,3");
		IVSETUP();
	}
	puts("-------------------------------");
	puts("Input IV:\n");
	for (i = 0; i < 4; ++i)
		display_bytewise(IV[i], 16);
	puts("-------------------------------");
	/*Updating Counter variables by specified equations*/
	C[0] = C[0] ^ ((IV[0]) + (IV[1] << 16));
	C[4] = C[4] ^ ((IV[0]) + (IV[1] << 16));
	C[2] = C[2] ^ ((IV[2]) + (IV[3] << 16));
	C[6] = C[6] ^ ((IV[2]) + (IV[3] << 16));
	C[1] = C[1] ^ ((IV[3] << 16) + (IV[1]));
	C[3] = C[3] ^ ((IV[2] << 16) + (IV[0]));
	C[5] = C[5] ^ ((IV[3] << 16) + (IV[1]));
	C[7] = C[7] ^ ((IV[2] << 16) + (IV[0]));
	/*Iterating the system by calling NEXTSTATE() 4 times*/
	for (i = 0; i < 4; ++i)
		NEXTSTATE();
}
/*GENERATE() generates the 128-bit pseudorandom bit-stream in each iteration*/
void GENERATE()
{
	int i, j, k, l, m;
	/*Due to binary operations temporary binary variables are declared and
	allocated*/
	unsigned char** x, ** c;
	x = (unsigned char**)malloc(sizeof(unsigned char*) * 8);
	for (i = 0; i < 8; ++i)
	{
		x[i] = (unsigned char*)malloc(sizeof(unsigned char) * 32);
	}
	initialize(x, 8, 32);
	s = (unsigned char**)malloc(sizeof(unsigned char*) * 8);
	for (i = 0; i < 8; ++i)
	{
		s[i] = (unsigned char*)malloc(sizeof(unsigned char) * 32);
	}
	initialize(s, 8, 32);
	puts("-------------------------------");
	puts("Output Stream:\n");
	/*The system is iterated for 3 times to output 48-byte pseudorandom
	bit-stream */
	for (j = 0; j < ROUND; ++j)
	{
		NEXTSTATE();
		for (i = 0; i < 8; ++i)
		{
			dectobin(x[i], X[i], 32);/*Decimal X (global) is converted to
			binary x (local)*/
		}
		/*Generate output by specified equations*/
		for (i = 0; i < 16; ++i)
		{
			s[0][i + 16] = x[0][i + 16] ^ x[5][i];
			s[0][i] = x[0][i] ^ x[3][i + 16];
			s[1][i + 16] = x[2][i + 16] ^ x[7][i];
			s[1][i] = x[2][i] ^ x[5][i + 16];
			s[2][i + 16] = x[4][i + 16] ^ x[1][i];
			s[2][i] = x[4][i] ^ x[7][i + 16];
			s[3][i + 16] = x[6][i + 16] ^ x[3][i];
			s[3][i] = x[6][i] ^ x[1][i + 16];
		}
		/*Display Output byte-wise in hexadecimal form*/
		for (m = 0; m < 4; ++m)
		{
			display_bytewise(bintodec(s[m], 32), 32);
		}
	}
	puts("-------------------------------");
}
/*main() function calls the sub-routines in order*/
int main()
{
	int i = 0, j;
	char k;
	KEYGEN();
	KEYSETUP();
	puts("Do you want to use IV? (Press 1 for ’yes’ and any other key for’no’)\n");
	scanf_s("%d", &i);
	if (i == 1)
		IVSETUP();/*It may be skipped if IV is not required by the user*/
	GENERATE();
	return 0;
}