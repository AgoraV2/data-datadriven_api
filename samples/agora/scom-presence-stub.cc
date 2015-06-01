#include <cstdio>
#include <iostream>
#include <vector>
#include <memory>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#include <stdio.h>     
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <string.h>    

#include <datadriven/datadriven.h>
#include "PresenceInterface.h"
#include "PresenceProxy.h"

using namespace std;
using namespace gen::com_agora_v2;

static char myName[] = "scomPresenceStub";


#define ABSENT    "0"
#define SOON_BACK "1"
#define PRESENT   "2"


class MyPListener :
public datadriven::Observer<PresenceProxy>::Listener {
public:
  void OnUpdate(const std::shared_ptr<PresenceProxy>& pp)
  {
    //const datadriven::ObjectId& id = pp->GetObjectId();
    const PresenceProxy::Properties prop = pp->GetProperties();
    
#if 0
    std::cout << "[Presence listener] Update for presence " << id << ": location = "
    << prop.Source.c_str() << ", state = " << prop.Presence << std::endl;
    std::cout << "> ";
    std::cout.flush();
#endif
    OnPresenceUpdate(prop.Presence, prop.Source.c_str());
  }

  virtual void OnPresenceUpdate(const uint32_t presence, const char* source) = 0;
};


class A_Presence :
public datadriven::ProvidedObject, public PresenceInterface, public MyPListener {
public:
       std::shared_ptr<datadriven::Observer<PresenceProxy> > observer;

       A_Presence(std::shared_ptr<datadriven::ObjectAdvertiser> advertiser,
       uint32_t defaultPresenceState = 0,
       qcc::String path = "") :
       datadriven::ProvidedObject(advertiser, path),   /* If you don't pass a path, it will be constructed for you */
       PresenceInterface(this)
       {
         Presence = defaultPresenceState;
         Source = myName;
         observer = datadriven::Observer<PresenceProxy>::Create(this);
  
         if (nullptr == observer) {
            std::cerr << "Observer not correctly initialized !!!" << std::endl;
         }
       }
       
       ~A_Presence()
       {
       };
       
       const qcc::String& GetSource() const
       {
         return Source;
       };
       
       void Update(void) {
         PresenceInterface::Update();
       };
       
       uint32_t getStatus(void) {
         return Presence;
       };

       void setAbsent(void) {
         Presence = 0;
         Update();
       };

       void setSoonBack(void) {
         Presence = 1;
         Update();
       };

       void setPresent(void) {
         Presence = 2;
         Update();
       };

       void OnPresenceUpdate(const uint32_t presence, const char* source) {
         // Update Presence shared state value with the one updated from another Agora Presence node
         if(strncmp(source, myName, strlen(myName)) != 0) {
           Presence = presence;
           std::cout << "Update shared state value with: ";
           switch(presence){
             case 0:
                cout << "Absent";
                break;
             case 1:
                cout << "Soon";
                break;
             case 2:
                cout << "Present";
                break;
             default:
                cout << "Invalid";
                break;
           }
           std::cout << " coming from " << source <<  std::endl;
           //std::cout << "> ";
           std::cout.flush();
         }
       }
};


int main(int argc, char** argv)
{
    std::shared_ptr<datadriven::ObjectAdvertiser> advertiser = datadriven::ObjectAdvertiser::Create();
    if (nullptr == advertiser) {
      std::cerr << "Object advertiser not correctly initialized !!!" << std::endl;
      return EXIT_FAILURE;
    }
    
    std::string path = "/" + std::string(myName);
    std::unique_ptr<A_Presence> presence = std::unique_ptr<A_Presence>(new A_Presence(advertiser, false, path.c_str()));
  
    // Ugly method (find a way to be sure that we are registered on bus and be sure that all Alljoyn notifications have been received:
    //   wait to be sure that OnUpdateP is called if another Presence node is already present 
    //   on Agora bus to update your Presence state value before putting object on bus
    sleep(2);
      
    if (ER_OK == presence->GetStatus()) {
      if (ER_OK != presence->PutOnBus()) {
        std::cerr << "Failed to announce presence existence !" << std::endl;
      }
    } else {
      std::cerr << "Failed to construct a presence properly" << std::endl;
    }

  while (1) {

      printf("%s: Set Present\n",presence->GetSource().c_str());
      presence->setPresent();
      sleep(5);
      printf("%s: Set Absent\n",presence->GetSource().c_str());
      presence->setAbsent();
      sleep(5);
    }

    return EXIT_SUCCESS;
}


