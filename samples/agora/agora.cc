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

class A_HomeCinema :
public datadriven::ProvidedObject, public HomeCinemaInterface {
public:
  A_HomeCinema(shared_ptr<datadriven::ObjectAdvertiser> advertiser,
       bool on = false,
       qcc::String path = "") :
       datadriven::ProvidedObject(advertiser, path),   /* If you don't pass a path, it will be constructed for you */
       HomeCinemaInterface(this)
       {
         HomeCinema = on;
         Source = myName;
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
};

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

  shared_ptr<datadriven::ObjectAdvertiser> advertiser = datadriven::ObjectAdvertiser::Create();
  if (nullptr == advertiser) {
    cerr << "Object advertiser not correctly initialized !!!" << endl;
    return EXIT_FAILURE;
  }
  
  string path = "/" + string(myName);
  unique_ptr<A_HomeCinema> homeCinema = unique_ptr<A_HomeCinema>(new A_HomeCinema(advertiser, false, path.c_str()));
    
  if (ER_OK == homeCinema->GetStatus()) {
    if (ER_OK != homeCinema->PutOnBus()) {
      cerr << "Failed to announce homeCinema existence !" << endl;
    }
  } else {
    cerr << "Failed to construct a homeCinema properly" << endl;
  }

  MyHCListener dl = MyHCListener();
  shared_ptr<datadriven::Observer<HomeCinemaProxy> > observer = datadriven::Observer<HomeCinemaProxy>::Create(&dl);
  
  if (nullptr == observer) {
    cerr << "Observer not correctly initialized !!!" << endl;
    return EXIT_FAILURE;
  }  
  
  while (1) {
    string input;
    cout << "> ";
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