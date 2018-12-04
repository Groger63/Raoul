
int i = 0;
char* NumberToRoman(int number)
{
    int* i = 0;;
    char romanval[1000];

    if (number <= 0)
    {
      return 0;
    }
    while (number != 0)
    {
        if (number >= 1000)
        {
            postdigit('M', number / 1000,romanval, i);
            number = number - (number / 1000) * 1000;
        }
        else if (number >= 500)
        {
            if (number < (500 + 4 * 100))
            {
                postdigit('D', number / 500,romanval, i);
                number = number - (number / 500) * 500;
            }
            else
            {
                predigit('C','M',romanval, i);
                number = number - (1000-100);
            }
        }
        else if (number >= 100)
        {
            if (number < (100 + 3 * 100)) 
            {
                postdigit('C', number / 100,romanval, i);
                number = number - (number / 100) * 100;
            }
            else
            {
                predigit('L', 'D',romanval, i);
                number = number - (500 - 100);
            }
        }
        else if (number >= 50 )
        {
            if (number < (50 + 4 * 10))
            {
                postdigit('L', number / 50,romanval, i);
                number = number - (number / 50) * 50;
            }
            else
            {
                predigit('X','C',romanval, i);
                number = number - (100-10);
            }
        }
        else if (number >= 10)
        {
            if (number < (10 + 3 * 10))
            {
                postdigit('X', number / 10,romanval, i);
                number = number - (number / 10) * 10;
            }
            else
            {
                predigit('X','L',romanval, i);
                number = number - (50 - 10);
            }
        }
        else if (number >= 5)
        {
            if (number < (5 + 4 * 1))
            {
                postdigit('V', number / 5,romanval, i);
                number = number - (number / 5) * 5;
            }
            else
            {
                predigit('I', 'X',romanval, i);
                number = number - (10 - 1);
            }
        }
        else if (number >= 1)
        {
            if (number < 4)
            {
                postdigit('I', number / 1,romanval, i);
                number = number - (number / 1) * 1;
            }
            else
            {
                predigit('I', 'V',romanval, i);
                number = number - (5 - 1);
            }
        }
    }

    return romanval;
}
 
void predigit(char num1, char num2, char* romanval, int* i)
{
    romanval[*i++] = num1;
    romanval[*i++] = num2;
}
 
void postdigit(char c, int n, char* romanval, int* i)
{
    int j;
    for (j = 0; j < n; j++)
        romanval[*i++] = c;
}