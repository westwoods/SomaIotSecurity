#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <signal.h>
#include "gateway_main.h"
#include "write_conf.h"
#include "lcd.h"
#include "otp.h"
#include "init.h"

const int LCD_POW_PIN = 2; //wiringPi pin 2

void init_struct(){

    //read struct from backup file(backup.txt)
    FILE *fr;
    fr=fopen("backup.txt","r");
    fscanf(fr,"%s %s %s %s",&inner_data.local_SSID, &inner_data.local_PW, &inner_data.guest_SSID, &inner_data.guest_PW);
    fclose(fr);

}

void init_service(){
    //run hostapd
    init_hostapd();

}

void init_hostapd(){

    hostapd_local_PID = fork();
    if (hostapd_local_PID > 0){
        printf("부모 프로세스 %d : %d\n", getpid(), hostapd_local_PID); //for debug

    }
    else if (hostapd_local_PID == 0){
        printf("자식 프로세스 %d\n", getpid());//for debug

        char *argv[]   = { "hostapd", "/etc/hostapd/hostapd.conf", "-f", "/var/log/hostapd_local.log", NULL};
        execv( "/usr/sbin/hostapd", argv);

        pause();
    }
    else if (hostapd_local_PID == -1){
        perror("fork error : ");
        exit(0);
    }

    hostapd_guest_PID = fork();
    if (hostapd_guest_PID > 0){
        printf("부모 프로세스 %d : %d\n", getpid(), hostapd_guest_PID);//for debug

    }
    else if (hostapd_guest_PID == 0){
        printf("자식 프로세스 %d\n", getpid());//for debug

        char *argv[]   = { "hostapd", "/etc/hostapd/hostapd_1.conf", "-f", "/var/log/hostapd_guest.log", NULL};
        execv( "/usr/sbin/hostapd", argv);

        pause();
    }
    else if (hostapd_guest_PID == -1){
        perror("fork error : ");
        exit(0);
    }


}
void restart_hostapd_local(){
    kill(hostapd_local_PID, SIGTERM);

    hostapd_local_PID = fork();
    if (hostapd_local_PID > 0){
        printf("부모 프로세스 %d : %d\n", getpid(), hostapd_local_PID); //for debug

    }
    else if (hostapd_local_PID == 0){
        printf("자식 프로세스 %d\n", getpid());//for debug

        char *argv[]   = { "hostapd", "/etc/hostapd/hostapd.conf", "-f", "/var/log/hostapd_local.log", NULL};
        execv( "/usr/sbin/hostapd", argv);

        pause();
    }
    else if (hostapd_local_PID == -1){
        perror("fork error : ");
        exit(0);
    }


}

void restart_hostapd_guest(){
    kill(hostapd_guest_PID, SIGTERM);

    hostapd_guest_PID = fork();
    if (hostapd_guest_PID > 0){
        printf("부모 프로세스 %d : %d\n", getpid(), hostapd_guest_PID);//for debug

    }
    else if (hostapd_guest_PID == 0){
        printf("자식 프로세스 %d\n", getpid());//for debug

        char *argv[]   = { "hostapd", "/etc/hostapd/hostapd_1.conf", "-f", "/var/log/hostapd_guest.log", NULL};
        execv( "/usr/sbin/hostapd", argv);

        pause();
    }
    else if (hostapd_guest_PID == -1){
        perror("fork error : ");
        exit(0);
    }


}
void backup_struct(){
    //save current struct
    FILE *fw;
    fw=fopen("backup.txt","w");
    fprintf(fw,"%s %s %s %s\n",inner_data.local_SSID, inner_data.local_PW, inner_data.guest_SSID, inner_data.guest_PW);
    fclose(fw);
}

void restart_lcd(){

    pullUpDnControl(LCD_POW_PIN, PUD_DOWN); //pull-down lcd power relay pin
    pinMode(LCD_POW_PIN, OUTPUT);
    digitalWrite(LCD_POW_PIN, HIGH);
    sleep(1);
    digitalWrite(LCD_POW_PIN, LOW);

    sprintf(lcd_data.row[0], "SSID: %s", inner_data.guest_SSID);
    sprintf(lcd_data.row[1], "PW: %s", inner_data.guest_PW);
    update_flag.lcd = 1;

}
