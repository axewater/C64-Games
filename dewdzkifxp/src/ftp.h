#ifndef FTP_H
#define FTP_H

#include <stdint.h>

#define MAX_FTP_SERVERS 8

/* FTP Server structure */
typedef struct {
    uint8_t active;
    char name[20];
    uint8_t bandwidth;    /* 10-100 MB/s */
    uint8_t status;       /* 0=closed, 1=open */
} FTPServer;

/* Global FTP pool */
extern FTPServer ftps[MAX_FTP_SERVERS];

/* Initialize FTP system */
void ftp_init(void);

/* Scan for new public FTP (80% success rate) */
uint8_t ftp_scan(void);

/* Get FTP server by index */
FTPServer* ftp_get(uint8_t index);

/* Add FTP to pool */
uint8_t ftp_add(FTPServer* ftp);

#endif /* FTP_H */
