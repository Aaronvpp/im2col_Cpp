// g++ matmul.cpp -o matmul -std=c++17 -O3 -Wall && ./matmul

#include <sys/time.h>
#include <iostream>
#include <cassert>
#include <cstring>

double get_time() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  return tv.tv_sec + 1e-6 * tv.tv_usec;
}

constexpr int n = 512;
int A[n][n];
int B[n][n];
int BT[n][n];
int AT[n][n];
int C[n][n];
int C_groundtruth[n][n];

void init() {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      A[i][j] = rand(); 
      B[i][j] = rand(); 
    } 
  }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      for (int k = 0; k < n; k++) {
        C_groundtruth[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}

void test() {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      assert(C[i][j] == C_groundtruth[i][j]);
    }
  }
}

void matmul() {
  memset(C, 0, sizeof(C));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      for (int k = 0; k < n; k++) {
        C[i][j] += A[i][k] * B[k][j];    
      }   
    }
  }
}

void matmul_ikj() {
  memset(C, 0, sizeof(C));
  for (int i = 0; i < n; i++) {
    for (int k = 0; k < n; k++) {
      for (int j = 0; j < n; j++) {
        C[i][j] += A[i][k] * B[k][j];    
      }   
    }
  }
}

void matmul_AT() {
  memset(C, 0, sizeof(C));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      AT[i][j] = A[j][i];
    }
  }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      for (int k = 0; k < n; k++) {
        C[i][j] += AT[k][i] * B[k][j];    
      }   
    }
  }
}

void matmul_BT() {
  memset(C, 0, sizeof(C));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      BT[i][j] = B[j][i];
    }
  }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      for (int k = 0; k < n; k++) {
        C[i][j] += A[i][k] * BT[j][k];    
      }   
    }
  }
}

/***void im2col1() {
    //generate a 56*56 Matrix 
    for (int i = 0; i < height_feature; i++) {
        for (int j = 0; j < width_feature; j++) {
        C[i][j] = i+j; 
        } 
    }
    //convert it into a 9 * 2916 Matrix
    for (int i = 0; i < 2916; i++) {
        for (int j = 0; j < 9; j++) {
        D[i][j] = i+j; 
        } 
    }
}***/

void im2col(const float* data_im, 
    const int im_c, 
    const int im_w, 
    const int im_h, 
    const int kw, 
    const int kh, 
    const int ph, 
    const int pw, 
    const int sh,
    const int sw,
    float* data_col, 
    const int col_w, 
    const int col_h) {

        // win_w and win_h are the stop times of the kernel in the image.
        int win_w = (im_w + 2 * pw - kw + 1) / sw;
        int win_h = (im_h + 2 * ph - kh + 1) / sh;

        
        for (int i = 0; i< col_h; i++) {
            float x,y;

            x = i % win_w;
            y = i / win_w;
            for(int j = 0; j < col_w; j++) {
                int c = j / (kw * kh);
                int kj = j % kw;
                int ki = j / kw;

                int row = y * sh + ki;
                int col = x * sw + kj;

                data_col[i * col_w + j] = get_data(data_im, c, im_w, im_h, row, col, ph, pw);
            }
        }

}
float get_data(float* data_im, int c, int im_w, int im_h, int row, int col, int ph, int pw) {

  row = row - ph;
  col = col - pw;
  if(row < 0 || row >= im_h || col < 0 || col >= im_w) {
    return 0;
  }

  return data_im[c * im_w * im_h + row * im_w + col];

}
int main() {
  init();
  int E = [2916][9];
  int D = [56][56];
  int height_feature = 56;
  int width_feature = 56;
  for (int i = 0; i < height_feature; i++) {
        for (int j = 0; j < width_feature; j++) {
        D[i][j] = i+j; 
        } 
  }
  im2col(C,3,56,56,3,3,0,0,1,1,E,9,2916);
  printf(E);

  return 0;
}


