#include "mbed.h"
#include "string.h"
#include <cstdio>
#include "DHT.h"
#include "LSM6DS33.h"
#include "GPS.h"

#define N 100
#define SAMPLE 1000
#define GAIN 0.004375 //valeur datasheet LSM6DS33

/*structure de l'acceleromètre + gyro*/
struct Gyro{
  double x=0,  y=0,  z=0, v=0;
};

/*structure de l'acceleromètre + gyro*/
struct struct_GPS{
  char lat[20] = {0}, lon[20] = {0}, alt[20] = {0};
};

/*declaration des pins*/
static UnbufferedSerial bt (PA_9, PA_10, 9600);
static BufferedSerial GPS(PA_2, PB_4,9600); //UART2 utilisé pour l'USB du µc

AnalogIn lum (A2);//pin capteur de luminosité 
AnalogIn son (A0); // pin capteur de son
DHT sensor (A1,SEN11301P); // pin capteur température et humidité
LSM6DS33 acc (PB_7, PB_6); // pin Gyroscope
Timer timer;

/*declaration des fonctions*/
struct_GPS getGPS (void);
Gyro getGyro(double offsetx, double offsety, double offsetz);
float getMaxSon (void);

int main(){

    //buffer d'envoie des valeurs
    char L[50] = "";
    char S[50] = "";
    char T[50] = "";
    char H[50] = "";
    char X[50] = "";
    char Y[50] = "";
    char Z[50] = "";
    char V[50] = "";
    char A[50] = "";
    char G[50] = "";
    char I[50] = "";

    //buffer de reception
    char buffer[3];

    //valeur du gyro + acc
    struct Gyro gyro;

    //valeur de GPS
    struct struct_GPS gps;

    //Variable capteurs analogiques
    double V_lum, V_son, V_temp, V_hum;
    int erreur;

    //variables gyro
    double offsetx=0, offsety=0, offsetz=0;

    /*GPS.set_baud(9600);
    bt.baud(9000);

    GPS.set_blocking(false);
    bt.set_blocking(false);*/

    acc.begin();

    //calcule de l'offset moyen des 3 axes du gyroscope 
    for (int i=0; i<SAMPLE; i++){
        acc.readGyro();
        offsetx += acc.gx;
        offsety += acc.gy;
        offsetz += acc.gz;
        printf("%f\n\r", acc.gx);
    }
    offsetx /= SAMPLE;
    offsety /= SAMPLE;
    offsetz /= SAMPLE;

    printf("offeset x : %f", offsetx);
    printf("offeset y : %f", offsety);
    printf("offeset z : %f", offsetz);
    
    while(1){

        //recupération des valeurs des capteurs
        V_lum = lum.read()*1000;
        V_son = getMaxSon();

        erreur = sensor.readData();
        if (erreur == 0) {
            V_temp = sensor.ReadTemperature(CELCIUS);
            V_hum = sensor.ReadHumidity();
        }

        gyro = getGyro(offsetx, offsety, offsetz);

        //gps = getGPS();


        //envoie les données des capteurs quand il est autorisé par android studio 
        if(bt.readable()){
          bt.read((void *)&buffer, 2);
        }
      
        if (buffer[0] == '1'){
            buffer[0] = 0;

            //mise en forme des valeurs
            sprintf(S, "S %0.2f/", V_son);
            sprintf(L, "L %0.2f/", V_lum);
            sprintf(T, "T %0.2f/", V_temp);
            sprintf(X, "X %0.2f/", gyro.x);
            sprintf(Y, "Y %0.2f/", gyro.y);
            sprintf(Z, "Z %0.2f/", gyro.z);
            sprintf(V, "V %0.2f/", gyro.v);
            strcat(A, gps.alt);
            strcat(G, gps.lon);
            strcat(I, gps.lat);

            printf("%s",A);
            
            //envoi des valeurs 
            bt.write((void *) L, strlen(L));
            wait_us(50000);
            bt.write((void *) S, strlen(S));
            wait_us(50000);
            bt.write((void *) T, strlen(T));
            wait_us(50000);
            bt.write((void *) H, strlen(H));
            wait_us(50000);
            bt.write((void *) X, strlen(X));
            wait_us(50000);
            bt.write((void *) Y, strlen(Y));
            wait_us(50000);
            bt.write((void *) Z, strlen(Z));
            wait_us(50000);
            bt.write((void *) V, strlen(V));
            wait_us(50000);
            bt.write((void *) A, strlen(A));
            wait_us(50000);
            bt.write((void *) G, strlen(G));
            wait_us(50000);
            bt.write((void *) I, strlen(I));
            
            printf("send\n\r");
        }  
     }  
}

float getMaxSon (void){
    float max_son = 0;
    double tab_son[N];
    //Récupération du capteur de son + calcule du max
        for (int i=0;i<N;i++){
            tab_son[i]=  son.read();
        }
    
        max_son = tab_son[0];
        for (int j=1;j<N;j++)
        {
            if(tab_son[j]>=max_son) max_son = tab_son[j];
        }

        return max_son; 
}

Gyro getGyro(double offsetx, double offsety, double offsetz)
{
    double rate_gyr_x = 0, rate_gyr_y = 0, rate_gyr_z = 0;
    double gyroXangle=0,  gyroYangle=0,  gyroZangle=0;
    double gxx, gyy, gzz; 
    double time_exec = 0;

    double axx, ayy, azz;
    double vx, vy, V;

    struct Gyro gyro;

    timer.start();
        
        //récupération des valeurs du gyro - offset moyen
        acc.readGyro();
        gxx = acc.gx - offsetx;
        gyy = acc.gy - offsety;
        gzz = acc.gz - offsetz;
        
        time_exec = timer.elapsed_time().count();
        
        //calcule de l'angle de l'axe X
        if(gxx >= 1 | gxx <= -1){
            
            //convertion des valeurs brute en angle 
            rate_gyr_x = gxx * GAIN;
            gyroXangle+=rate_gyr_x*(time_exec/1000);
            
            //conditionnement de l'angle entre 0° et 360°
            if(gyroXangle  > 360) gyroXangle = 0;
            else if (gyroXangle < 0) gyroXangle = 360; 
        }
        
        //calcule de l'angle de l'axe Y
        if(gyy >= 1 | gyy <= -1){
            rate_gyr_y = gyy * GAIN;
            gyroYangle+=rate_gyr_y*(time_exec/1000);
            if(gyroYangle > 360) gyroYangle = 0;
            else if (gyroYangle < 0) gyroYangle = 360;  
        }
        
        //calcule de l'angle de l'axe Z
        if(gzz >= 1 | gzz <= -1){
            rate_gyr_z = gzz * GAIN;
            gyroZangle += rate_gyr_z*(time_exec/1000);
            if(gyroZangle > 360) gyroZangle = 0;
            else if (gyroZangle < 0) gyroZangle = 360;     
        }
        
        timer.reset();
        
        //calcule de l'accéleration
        acc.readAccel();
        axx = acc.ax;             
        ayy = acc.ay; 
        azz = acc.az; 
        vx = vx + axx*9.8*1/104;
        vy = vy + ayy*9.8*1/104;
        V = sqrt(vx*vx+vy*vy);
        
        //attribution des valeurs à la structure
        gyro.v = V;
        gyro.x = gyroXangle;
        gyro.y = gyroYangle;
        gyro.z = gyroZangle;

        return gyro;
        
}

struct_GPS getGPS (void){

    donneGPS myGPS;
    struct struct_GPS gps;

    int i = 0;
    char buffer[256] = {0};
    char envoi[256] = {0};
    int nb_byte = GPS.read((void*)&buffer[0], 1);
    char dollar[] = "$";

    if(GPS.readable())
        {
            //reset du buffer
            for(i=0; i<256;i++)
            {
                buffer[i] = {0};
            }

            //lecture du GPS
            do{
                GPS.read((void*)&buffer[0], 1);
            } while(strcmp(buffer, dollar)!= 0);

            //tri et mise en forme des données 
            for(i=1; i<100 ; i++)
            {
                while(!GPS.readable());
                GPS.read((void*)&buffer[i], 1);
                printf("Buffer[%d] = %s\r\n",i,(char*)&buffer[i]);
                wait_us(1000);
                
                if((strcmp(&buffer[i], "\r") == 0 || strcmp(&buffer[i], "$") == 0) && strlen(buffer) > 60) //buffer[i] == 10 || buffer[i] == 36) 
                {
                    strcpy(envoi, buffer);

                    if(((envoi[3]) == 'G') && ((envoi[4]) == 'G') && ((envoi[5]) == 'A') && (strchr(envoi, '*') != 0) )
                    {
                        myGPS.formatGGA(envoi);
                        printf("Valeur de la longitude : %s\r\n", myGPS.getGGA().LONG);
                        printf("Valeur de l'altitude : %s\r\n", myGPS.getGGA().ALT);
                        printf("Valeur de Latitude : %s\r\n",myGPS.getGGA().LAT);
                    }  
                }        
            } 
        }

        strcpy(gps.lon, myGPS.getGGA().LONG);
        strcpy(gps.lat, myGPS.getGGA().LAT);
        strcpy(gps.alt, myGPS.getGGA().ALT);    

        return gps;
}