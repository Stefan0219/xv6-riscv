#include "kernel/types.h"
#include "user/user.h"
#include "user/malloclib.h"

int main(){
    int *a = _malloc(sizeof(int));
    *a = 1;
    printf("%d\n",*a);
    return 0;
}