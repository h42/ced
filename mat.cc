#include <stdio.h> 

class matrix {
public:
    matrix(int r, int c);
    ~matrix();
private:
    int zrows, zcols;
    double *zmat;
};

matrix::~matrix() {
    delete []zmat;
}

matrix::matrix(int rows, int cols) {
    zmat=new double(rows*cols);
}

int main() {
    matrix m(4,8);
    printf("hey\n");
}
