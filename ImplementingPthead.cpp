#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<iostream>
using namespace std;

static int BridgeLoad;
static int waitTime;
static int Actual_BridgeLoad;


//Characteristic of each vehicle crossing the bridge
 struct Vehicle_info{
  string Vehicle_ID;
  int Arrival_timeDelay;
  int Vehicle_Weight;
  int Vehicle_CrossTime;

};

//declaration of the pthread variable
static pthread_mutex_t Lock;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


//function that handle the vehicle Entering  the bridge
void EnterBridge(int weight, string VechicleName){

     pthread_mutex_lock(&Lock);
     while((weight + Actual_BridgeLoad) > BridgeLoad)
     {
     pthread_cond_wait(&cond,&Lock);
     }
     Actual_BridgeLoad  = Actual_BridgeLoad + weight;
     cout<<"Vehicle "<<VechicleName<<" goes on bridge. "<<endl;
     cout<<"The Current Bridge load is "<<Actual_BridgeLoad<<" tons"<<endl;
     pthread_cond_signal(&cond);
     pthread_mutex_unlock(&Lock);

  }

//function that handle the vehicle leaving the bridge
void LeaveBridge(int weight,string VehicleName)
{

      pthread_mutex_lock(&Lock);
      cout<<"Vehicle "<<VehicleName<<" leave the bridge. "<<endl;
      Actual_BridgeLoad  = Actual_BridgeLoad - weight;
      cout<<"The Current Bridge load is "<<Actual_BridgeLoad<<" tons"<<endl;
      pthread_cond_signal(&cond);
      pthread_mutex_unlock(&Lock);
}

//function that handle the vehicle while on the bridge
void *CrossingBridge(void* PassVehicle){

      Vehicle_info *Vehicle = (Vehicle_info*)PassVehicle;
      waitTime = waitTime + Vehicle->Arrival_timeDelay;
      sleep(waitTime);

      //checking weither the bridge is capable of containing an extra vehicle
      if(Vehicle->Vehicle_Weight > BridgeLoad){
         cout<<"Vehicle "<<Vehicle->Vehicle_ID<<" exceeds maximum bridge laod"<<endl;
         cout<<"The Current Bridge load is "<<Actual_BridgeLoad<<" tons"<<endl;
         pthread_exit(NULL);

      }else{
         cout<<"Vehicle "<<Vehicle->Vehicle_ID<<" Arrives at the bridge"<<endl;
         cout<<"The Current Bridge load is "<<Actual_BridgeLoad<<" tons"<<endl;
         EnterBridge(Vehicle->Vehicle_Weight,Vehicle->Vehicle_ID);
      }
         sleep(Vehicle->Vehicle_CrossTime);



         LeaveBridge(Vehicle->Vehicle_Weight, Vehicle->Vehicle_ID);

         pthread_exit(NULL);
}

int main(int argc,char *argv[])
{
    BridgeLoad = atoi(argv[1]);

    //array of possible vehicles
    Vehicle_info *Vehicle = new Vehicle_info [120];

    //declaration of variable read from the file
    string TempVehicle_ID = " ";
    int TempArrival_timeDelay = 0;
    int TempVehicle_Weight = 0;
    int TempVehicle_CrossTine = 0;
    int NumberVehicles = 0;

    //reading from the file using a while loop
while(cin>>TempVehicle_ID>>TempArrival_timeDelay>>TempVehicle_Weight>>TempVehicle_CrossTine){

    Vehicle[NumberVehicles].Vehicle_ID = TempVehicle_ID;
    Vehicle[NumberVehicles].Arrival_timeDelay = TempArrival_timeDelay;
    Vehicle[NumberVehicles].Vehicle_Weight = TempVehicle_Weight;
    Vehicle[NumberVehicles].Vehicle_CrossTime = TempVehicle_CrossTine;

    NumberVehicles++;
}
    pthread_t Thread[NumberVehicles];

    //ask user the Maximum weight that the bridge can take on anytime
    cout<<"Maximum bridge load is "<<BridgeLoad<<" tons"<<endl;


    if(pthread_mutex_init(&Lock,NULL) != 0){

    cout<<"Mutex failed"<<endl;

    return 1;
}
    if(pthread_cond_init(&cond,NULL) != 0){

    cout<<"Condition Variable Failed"<<endl;

    return 1;
}
   //creating pthread for each vehicle base on the total number of vehicles
for(int index = 0; index < NumberVehicles; index++){

    pthread_create(&Thread[index],NULL,CrossingBridge,(void *)&Vehicle[index]);

}
  
for(int index = 0; index < NumberVehicles; index++){

    pthread_join(Thread[index], NULL);

}
    cout<<"\n";
    cout<<"Total Number of Vehicle "<<NumberVehicles<<endl;

    cout<<"\n";
 }
