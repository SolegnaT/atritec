#include<cstdio>
#include"atritec.cpp"
int main(int nargs, char** vargs)
{
    if (nargs < 2)
    {
        printf("Expected a filename.\n");
        return 1;
    }
    atritec(vargs[1]);
    return 0;
}
