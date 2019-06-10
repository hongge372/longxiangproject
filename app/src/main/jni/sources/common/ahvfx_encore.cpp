//
//  ahvfx_encore.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/12.
//  Copyright © 2019 AH. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <vector>
#include <string>
#define min(a, b)    (((a)<(b))?(a):(b))

extern "C" {
    char *readbuf(const char *filename, long &readsize) {
        FILE *fp=fopen(filename, "rb");
        if (!fp) return NULL;
        int ret = 0;
        ret|=fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        ret|=fseek(fp, 0, SEEK_SET);
        if (size == -1 || ret) {
            fclose(fp);
            return NULL;
        }
        char *buf = (char *)malloc(size+4);
        if (!buf) {
            fclose(fp);
            return NULL;
        }
        ret = (int)fread(buf, size, 1, fp);
        if (ret!=1) {
            free(buf);
            buf = NULL;
        } else {
            //检查是否为加密格式
            uint32_t *head=(unsigned int *)buf;
            uint32_t *writer=head;
            long r = (ntohl(*head)^ntohl(*(head+1)));
            if (r == 0x4FD91A4A) {
                head++;
                uint32_t partlen = ntohl((*head)^(*(head+1)));
                head++;
                size = size - partlen - 12;
                for(int i=0;i<size;i+=4) {
                    *writer = (*head)^(*(head+1));
                    writer++;
                    head++;
                }
            }
            else if (r == 0x4A4FD91A)
            {
                head++;
                uint32_t partlen = ntohl((*head)^(*(head+1)));
                head++;
                size = size - partlen - 12;
                for(int i=0;i<size;i+=4) {
                    *writer = (*head)^(*(head+1));
                    writer++;
                    head++;
                }
            }
            buf[size]='\0';
        }
        readsize = size;
        fclose(fp);
        return buf;
    }
    void freeBuf(void *buf) {
        free(buf);
    }
}

