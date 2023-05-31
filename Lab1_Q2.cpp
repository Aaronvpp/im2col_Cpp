//readme: please directly run this program which will run for about 4 minutes.
#include<iostream>

using namespace std;

bool is_a_ge_zero_and_a_lt_b(int a,int b)
{
    if(a>=0 && a <b) return true;
    return false;
}

void im2col_cpu(const float* data_im, const int channels,
                const int height, const int width, const int kernel_h, const int kernel_w,
                const int pad_h, const int pad_w,
                const int stride_h, const int stride_w,
                const int dilation_h, const int dilation_w,
                float* data_col) {
    const int output_h = (height + 2 * pad_h -
                          (dilation_h * (kernel_h - 1) + 1)) / stride_h + 1;
    const int output_w = (width + 2 * pad_w -
                          (dilation_w * (kernel_w - 1) + 1)) / stride_w + 1;
    const int channel_size = height * width;
    for (int channel = channels; channel--; data_im += channel_size) {
        for (int kernel_row = 0; kernel_row < kernel_h; kernel_row++) {
            for (int kernel_col = 0; kernel_col < kernel_w; kernel_col++) {
                int input_row = -pad_h + kernel_row * dilation_h;
                for (int output_rows = output_h; output_rows; output_rows--) {
                    if (!is_a_ge_zero_and_a_lt_b(input_row, height)) {
                        for (int output_cols = output_w; output_cols; output_cols--) {
                            *(data_col++) = 0;
                        }
                    } else {
                        int input_col = -pad_w + kernel_col * dilation_w;
                        for (int output_col = output_w; output_col; output_col--) {
                            if (is_a_ge_zero_and_a_lt_b(input_col, width)) {
                                *(data_col++) = data_im[input_row * width + input_col];
                            } else {
                                *(data_col++) = 0;
                            }
                            input_col += stride_w;
                        }
                    }
                    input_row += stride_h;
                }
            }
        }
    }
}


int main()
{
    float* data_im;
    int height=56;
    int width=56;
    int kernel_h=3;
    int kernel_w=3;
    int pad_h=0;
    int pad_w=0;
    int stride_h=1;
    int stride_w=1;
    int dilation_h=1;
    int dilation_w=1;
    float* data_col;
    int channels =3;
    const int output_h = (height + 2 * pad_h -
                          (dilation_h * (kernel_h - 1) + 1)) / stride_h + 1;
    const int output_w = (width + 2 * pad_w -
                          (dilation_w * (kernel_w - 1) + 1)) / stride_w + 1;
    data_im = new float[channels*height*width];
    data_col = new float[channels*output_h*output_w*kernel_h*kernel_w];

    //init input image data
    for(int m=0;m<channels;++m)
    {
        for(int i=0;i<height;++i)
        {
            for(int j=0;j<width;++j)
            {
                data_im[m*width*height+i*width+j] = m*width*height+ i*width +j;
                cout <<data_im[m*width*height+i*width+j] <<' ';
            }
            cout <<endl;
        }
    }
    cout <<"feature map with 3 input channels "<<endl;
    im2col_cpu(data_im, channels,
               height,width, kernel_h, kernel_w,
               pad_h, pad_w,
               stride_h, stride_w,
               dilation_h, dilation_w,
               data_col);
    cout <<channels<<endl;
    cout <<output_h<<endl;
    cout <<output_w<<endl;
    cout <<kernel_h<<endl;
    cout <<kernel_w<<endl;
    // cout <<"error"<<endl;
    int im2col_feature_output [kernel_w*kernel_h*channels][output_w*output_h];

    for(int i=0;i<kernel_w*kernel_h*channels;++i)
    {
        for(int j=0;j<output_w*output_h;++j)
        {

            cout <<data_col[i*output_w*output_h+j]<<' ';
            im2col_feature_output [i][j]= data_col[i*output_w*output_h+j];
        }

        cout <<endl;

    }
    cout <<"feature map after im2col operation "<<endl;
    /***int im2col_feature_output_c = [3][9][2916];
    for (int c = 0; c < 3; c++) {
        for (int row = 0; row < 9; row++) {
            for (int col = 0; col < 2916; col++) {
                im2col_feature_output_c [c][row][col] = im2col_feature_output[(9*c)+row][col];
            }
        }
    }***/
    cout << im2col_feature_output[5][3];
    cout <<endl;
    // To build kernel.

    int kernel_channels [64][3][9][1];
    for (int k = 0; k < 64; k++) {
        for (int c = 0; c < 3; c++) {
            kernel_channels[k][c][0][0] = 1;
            kernel_channels[k][c][1][0] = 0;
            kernel_channels[k][c][2][0] = 0;
            kernel_channels[k][c][3][0] = 1;
            kernel_channels[k][c][4][0] = -1;
            kernel_channels[k][c][5][0] = 1;
            kernel_channels[k][c][6][0]= 1;
            kernel_channels[k][c][7][0] = 0;
            kernel_channels[k][c][8][0] = -1;
        }
    }
    cout << kernel_channels[61][0][26];
    cout <<endl;

    cout <<"Start matrix multi, please be patient, it will run for a few minutes."<<endl;
    cout <<sizeof(im2col_feature_output)/ sizeof(im2col_feature_output[0])<<endl;//row
    cout <<sizeof(im2col_feature_output[0])/ sizeof(im2col_feature_output[0][0])<<endl;//col

    float OA[64][54][54];
    for(int k=0;k<64;k++) {
        for (int i = 0; i < 54; i++) {
            for (int j = 0; j < 54; j++) {
                for (int ki = 0; ki < 8748; ki++) {
                    for (int c = 0; c < 3; c++) {
                        for (int col = 0; col < 9; col++) {
                            OA[k][i][j] += im2col_feature_output[col][ki] * kernel_channels[k][c][col][0];
                        }
                    }
                }
            }
        }
    }

    /***int h,w,k,p,q,r,s;
    for (k=0; k<64; k++) {
        for (p = 0; p < 54; p++) {
            for (q = 0; q < 54; q++) {
                for (r = 0; r < 1; r++) {
                    for (s = 0; s < 27; s++) {

                        h = p + r;
                        w = q + s;
                        OA[k][p][q] += im2col_feature_output[h][w] * kernel_channels[k][r][s];
                    }
                }
            }
        }
    }***/
    cout <<"convolution result "<<endl;
    cout <<endl;
    int x,y,z;
    for(x=0; x<64; x++)
    {
        for(y=0; y<54; y++)
        {
            for(z=0; z<54; z++)
                cout<<OA[x][y][z]<<"  ";
            cout<<endl;
        }
        cout<<endl;
    }
    cout <<"Final convolution result using im2col method generating a 64*54*54 matrix" <<endl;

    return 0;
}



