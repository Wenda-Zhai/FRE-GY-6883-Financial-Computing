//
//  gnuplot_linux.cpp
//

#include <stdio.h>   // 标准输入/输出头文件
#include <stdlib.h>  // 用于内存分配函数，如 malloc()
#include <math.h>    // 用于数学函数，如 sin() 和 cos()
void plotResults(double* xData, double* yData, double *yData2, int dataSize);
//画图的函数
int main() {
    int i = 0;
    int nIntervals = 100; //数据间隔数量
    double intervalSize = 1.0; //区间长度
    double stepSize = intervalSize/nIntervals;
    double* xData = (double*) malloc((nIntervals+1)*sizeof(double)); //为x分配内存
    double* yData = (double*) malloc((nIntervals+1)*sizeof(double)); //为y1分配内存
    double* yData2 = (double*) malloc((nIntervals+1)*sizeof(double));//为y2分配内存
    //生成x坐标
    xData[0] = 0.0;
    double x0 = 0.0;
    for (i = 0; i < nIntervals; i++) {
        x0 = xData[i];
        xData[i+1] = x0 + stepSize;
    }
    for (i = 0; i <= nIntervals; i++) {
        x0 = xData[i];
        yData[i] = sin(x0)*cos(10*x0);
    }
    for (i = 0; i <= nIntervals; i++) {
        x0 = xData[i];
        yData2[i] = sin(x0)*cos(5*x0);
    }
    plotResults(xData, yData, yData2, nIntervals);
    return 0;
}
void plotResults(double* xData, double* yData, double* yData2, int dataSize) {
    FILE *gnuplotPipe,*tempDataFile; // 用于 gnuplot 管道和临时数据文件的文件指针
    const char *tempDataFileName1 = "tempData1";
    const char *tempDataFileName2 = "tempData2";
    double x,y, x2, y2;
    int i;
    const char *title = "Try it";
    const char *yLabel = "Y";
    gnuplotPipe = popen("gnuplot -persist", "w"); //gnuplotPipe True/False  // 打开带有持续窗口的 gnuplot
    fprintf(gnuplotPipe, "set grid\n"); // 添加网格到绘图
    fprintf(gnuplotPipe, "set title '%s'\n", title); // 设置绘图标题
    fprintf(gnuplotPipe, "set arrow from 0.5,graph(0,0) to 0.5,graph(1,1) nohead lc rgb 'red'\n"); // 在 x=0 处添加红色垂直线
    fprintf(gnuplotPipe, "set xlabel 'Announcement Date'\nset ylabel '%s'\n", yLabel); // 设置轴标签
    if (gnuplotPipe) {
        // 两个数据文件的绘图命令
        fprintf(gnuplotPipe,"plot \"%s\" with lines, \"%s\" with lines\n",tempDataFileName1, tempDataFileName2);
        fflush(gnuplotPipe); // 确保命令立即发送

        // 将第一个数据集写入临时文件
        tempDataFile = fopen(tempDataFileName1,"w");
        for (i=0; i <= dataSize; i++){
            x = xData[i];
            y = yData[i];
            fprintf(tempDataFile,"%lf %lf\n",x,y);
        }
        fclose(tempDataFile);

        // 将第二个数据集写入临时文件
        tempDataFile = fopen(tempDataFileName2,"w");
        for (i=0; i <= dataSize; i++) {
            x2 = xData[i];
            y2 = yData2[i];
            fprintf(tempDataFile,"%lf %lf\n",x2,y2);
        }
        fclose(tempDataFile);

        // 等待用户输入后再关闭
        printf("press enter to continue...");
        getchar();
        //清理临时文件并推出gnuplot
        remove(tempDataFileName1);
        remove(tempDataFileName2);
        fprintf(gnuplotPipe,"exit \n");
    }
    else{
        printf("gnuplot not found...");
    }
} 