// SNM - Sleptsov Net Machine in Arduino IDE, debugged on Raspberry Pi Pico
// SN in sn.h
// pin map in pin.h
// just to run SN, pin map can be omitted
static int dbg=2; // debug mode if dbg>0
volatile static int go=1;

#include "sn.h"

#include "pin.h"

// if no pin map included, please uncomment thefollowing plugs
// void Init_pins_IR(){}
// void ReadIn(){}
// void WriteOut(){}

void setup() {

  // init pins
  // init interrupts
  Init_pins_IR();
  if(dbg>0) {
    Serial.begin(115200);
    delay(1000);
    Serial.print("SNM time in ms "); 
    Serial.println(millis());
  } 

}

#define MOFF(i,j,d1,d2) ((d2)*(i)+(j))
#define MELT(x,i,j,d1,d2) (*((x)+MOFF(i,j,d1,d2)))

static int y[n]; 

void print_matr(int *x,int d1,int d2)
{
  int i,j;
  for(i=0;i<d1;i++)
  {
    for(j=0;j<d2;j++)
    {
      Serial.print(MELT(x,i,j,d1,d2),2);
      Serial.print(" ");
    }
    Serial.println("");
  }
}

void print_vect(int *x,int d1)
{
  int i;
  for(i=0;i<d1;i++)
  {
    Serial.print(x[i]);
    Serial.print(" ");
  }
  Serial.println("");

}

#define zmin(x,y) (((x)<(y))?(x):(y))

static int i,j,yy,jj,yij,k=0;  

void loop() {
  
    while(go) 
    {
      ReadIn();
if(dbg>0) {Serial.print("sn-vm step "); Serial.print(k); Serial.print(" time in ms "); Serial.println(millis()); }
if(dbg>0) print_vect(mu,m); 
      // y[j] = firing multiplicity = INT_MAX !!!

      for(j=0;j<n;j++) y[j]=INT_MAX;
      
      for(i=0;i<m;i++)
        for(j=0;j<n;j++)
        {
          yij = (b[i][j]>0)? mu[i] / b[i][j] : (b[i][j]<0)? ((mu[i]>0)? 0: INT_MAX): INT_MAX;
          y[j]=zmin(y[j],yij);
        }                  
      
if(dbg>1){
Serial.print("y#1: ");          
print_vect(y,n);}       
      
      // remove low priority transitions
      
      for(j=0;j<n;j++)
        for(i=0;i<n;i++)
        {
          if(i==j) continue;
          if(r[i][j] > 0 && y[i] != 0) 
            y[j]=0;
        }
if(dbg>1){
Serial.print("y#2: ");              
print_vect(y,n);}            
 
      // count firable and choose firing
      jj=-1;	 
      for(j=0;j<n;j++)
	      if(y[j]>0){jj=j; break;}

      if(jj==-1) {go=0; break;}
       
      yy=y[jj];
if(dbg>0) { Serial.print("fires "); Serial.print(jj); Serial.print(" in "); Serial.print(yy); Serial.println(" copies"); }      

      // mu[i] = mu[i] + yy*c[i][jj];  next marking
      
      for(i=0;i<m;i++)
        mu[i] = mu[i] - ((b[i][jj]>0)?yy*b[i][jj]:0) + yy*d[i][jj];
         
      k++;   
      WriteOut();
    } 
    
}

// @ Dmitry Zaitsev, May 25, 2024, daze@acm.org
