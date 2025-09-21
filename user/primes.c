// Lab Xv6 and Unix utilities
// primes.c

#include "kernel/types.h"
#include "user/user.h"
#include "stddef.h"

// 文件描述符重定向
void mapping(int n, int pd[]) {
  if (close(n) < 0) {
    fprintf(2, "close error\n");
    exit(1);
  }
  if (dup(pd[n]) < 0) {
    fprintf(2, "dup error\n");
    exit(1);
  }
  if (close(pd[0]) < 0) {
    fprintf(2, "close error\n");
    exit(1);
  }
  if (close(pd[1]) < 0) {
    fprintf(2, "close error\n");
    exit(1);
  }
}

/*
 * 素数筛算法：
 * 1. 读取第一个数字，它一定是素数
 * 2. 创建新进程和管道
 * 3. 子进程过滤掉当前素数的倍数
 * 4. 父进程递归处理剩余数字
 * 5. 每个素数对应一个进程
 */
void primes() {
  int previous, next;
  int fd[2];
  
  // 读取第一个数字
  if (read(0, &previous, sizeof(int)) != sizeof(int)) {
    return; // 没有更多数字，直接返回
  }
  
  // 第一个一定是素数，直接打印
  printf("prime %d\n", previous);
  
  // 创建管道
  if (pipe(fd) < 0) {
    fprintf(2, "pipe error\n");
    exit(1);
  }
  
  // 创建子进程
  if (fork() == 0) {
    // 子进程
    // 将管道的写端口映射到描述符 1 上
    mapping(1, fd);
    
    // 循环读取管道中的数据
    while (read(0, &next, sizeof(int)) == sizeof(int)) {
      // 如果该数不是当前素数的倍数
      if (next % previous != 0) {
        // 写入管道
        if (write(1, &next, sizeof(int)) != sizeof(int)) {
          fprintf(2, "write error\n");
          exit(1);
        }
      }
    }
    close(1);
    exit(0); // 子进程完成任务后退出
  } else {
    // 父进程
    // 等待子进程完成
    wait(NULL);
    // 将管道的读端口映射到描述符 0 上
    mapping(0, fd);
    // 递归执行此过程
    primes();
  }  
}

int main(int argc, char *argv[]) {
  int fd[2];
  
  // 创建管道
  if (pipe(fd) < 0) {
    fprintf(2, "pipe error\n");
    exit(1);
  }
  
  // 创建进程
  if (fork() == 0) {
    // 子进程
    // 将管道的写端口映射到描述符 1 上
    mapping(1, fd);
    // 循环获取 2 至 35
    for (int i = 2; i < 270; i++) {

      // 将其写入管道
      if (write(1, &i, sizeof(int)) != sizeof(int)) {
        fprintf(2, "write error\n");
        exit(1);
      }
    }
    exit(0); // 数字生成完成后退出
  } else {
    // 父进程
    // 将管道的读端口映射到描述符 0 上
    mapping(0, fd);
    // 调用 primes() 函数求素数
    primes();
    
    // 等待所有子进程完成
    while (wait(NULL) > 0) {
      // 等待所有子进程
    }
  }
  
  // 正常退出
  exit(0);
}
