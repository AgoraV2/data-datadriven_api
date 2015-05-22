#include <cstdio>
#include <iostream>
#include <vector>
#include <memory>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#include <datadriven/datadriven.h>
#include "HomeCinemaInterface.h"
#include "HomeCinemaProxy.h"

using namespace std;
using namespace gen::com_agora_v2;

static char *myName;

class MyHCListener :
public datadriven::Observer<HomeCinemaProxy>::Listener {
public:
  void OnUpdate(const shared_ptr<HomeCinemaProxy>& hc)
  {
    const datadriven::ObjectId& id = hc->GetObjectId();
    const HomeCinemaProxy::Properties prop = hc->GetProperties();
    
    cout << "[HomeCinema listener] Update for hc " << id << ": location = "
    << prop.Source.c_str() << ", state = " << prop.HomeCinema << endl;
    cout << "> ";
    cout.flush();
    OnUpdateHC(prop.HomeCinema, prop.Source.c_str());
  }

  virtual void OnUpdateHC(const bool homeCinema, const char* source) = 0;
};


class A_HomeCinema :
public datadriven::ProvidedObject, public HomeCinemaInterface, public MyHCListener {
public:
       shared_ptr<datadriven::Observer<HomeCinemaProxy> > observer;

       A_HomeCinema(shared_ptr<datadriven::ObjectAdvertiser> advertiser,
       bool defaultHomeCinemaState = false,
       qcc::String path = "") :
       datadriven::ProvidedObject(advertiser, path),   /* If you don't pass a path, it will be constructed for you */
       HomeCinemaInterface(this)
       {
         HomeCinema = defaultHomeCinemaState;
         Source = myName;
         observer = datadriven::Observer<HomeCinemaProxy>::Create(this);
  
         if (nullptr == observer) {
            cerr << "Observer not correctly initialized !!!" << endl;
         }
       }
       
       ~A_HomeCinema()
       {
       };
       
       const qcc::String& GetSource() const
       {
         return Source;
       };
       
       void Update(void) {
         HomeCinemaInterface::Update();
       };
       
       void setOn(void) {
         HomeCinema = true;
         Update();
       };

       void setOff(void) {
         HomeCinema = false;
         Update();
       };

       void OnUpdateHC(const bool homeCinema, const char* source) {
         // Update HomeCinema shared state value with the one updated from another Agora HomeCinema node
         if(strncmp(source, myName, strlen(myName)) != 0) {
           HomeCinema = homeCinema;
           cout << "Update shared state value with: " << homeCinema << " coming from " << source <<  endl;
           cout << "> ";
           cout.flush();
         }
       }
};

int main(int argc, char** argv)
{
  
  if (argc==2)
    myName = strdup(argv[1]);
  else {
    printf("give me a name\n");
    return 0;
  }

  cout << "> ";
  cout.flush();
  

  shared_ptr<datadriven::ObjectAdvertiser> advertiser = datadriven::ObjectAdvertiser::Create();
  if (nullptr == advertiser) {
    cerr << "Object advertiser not correctly initialized !!!" << endl;
    return EXIT_FAILURE;
  }
  
  string path = "/" + string(myName);
  unique_ptr<A_HomeCinema> homeCinema = unique_ptr<A_HomeCinema>(new A_HomeCinema(advertiser, false, path.c_str()));

  // Ugly method (find a way to be sure that we are registered on bus and be sure that all Alljoyn notifications have been received:
  //   wait to be sure that OnUpdateHC is called if another HomeCinema node is already present 
  //   on Agora bus to update your HomeCinema state value before putting object on bus
  sleep(2);
    
  if (ER_OK == homeCinema->GetStatus()) {
    if (ER_OK != homeCinema->PutOnBus()) {
      cerr << "Failed to announce homeCinema existence !" << endl;
    }
  } else {
    cerr << "Failed to construct a homeCinema properly" << endl;
  }

  while (1) {
    string input;
    getline(cin, input);
    if (strncmp(input.c_str(),"exit",4)==0)
      break;
    else if (strncmp(input.c_str(),"on",2)==0) {
      printf("Cinema[%s] ON\n",homeCinema->GetSource().c_str());
      homeCinema->setOn();
    } else if (strncmp(input.c_str(),"off",3)==0) {
      printf("Cinema[%s] OFF\n",homeCinema->GetSource().c_str());
      homeCinema->setOff();      
    }
  }

  if (myName)
    free(myName);

  return EXIT_SUCCESS;
}
