#include <stdio.h>
#include <tensorflow/c/c_api.h>

int main() {
    printf("Hello from Tensorflow C library version %s\n", TF_Version());
    return 0;
}