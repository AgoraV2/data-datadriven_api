/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include <iostream>
#include <string>

#include <unistd.h>

#include <datadriven/datadriven.h>

#include <InComingCallInterface.h>
#include <InComingCallProxy.h>
#include <PresenceInterface.h>
#include <PresenceProxy.h>
#include <cstdio>

using namespace std;
using namespace datadriven;
using namespace gen::com_agora_v2;

class PresenceListener :
public datadriven::Observer<PresenceProxy>::Listener {
public:
  void OnUpdate(const std::shared_ptr<PresenceProxy>& pp)
  {
    const datadriven::ObjectId& id = pp->GetObjectId();
    const PresenceProxy::Properties prop = pp->GetProperties();
    
    cout << ">>>>>>>>>>>>>>>>>" << endl;
    cout << "Update from: " << id << ": location = " << prop.Source.c_str() << std::endl;
    cout << "Presence: ";
        switch(prop.Presence){
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
    cout << endl;
    cout << "<<<<<<<<<<<<<<<<<" << endl;
    std::cout.flush();
  }
};


class InComingCallListener :
    public Observer<InComingCallProxy>::Listener {
  public:
    virtual void OnUpdate(const shared_ptr<InComingCallProxy>& p)
    {
        const datadriven::ObjectId& id = p->GetObjectId();
        InComingCallProxy::Properties prop = p->GetProperties();
        cout << ">>>>>>>>>>>>>>>>>" << endl;
        cout << "Update from: " << id << endl;
        cout << "Priority: ";
        switch(prop.CallInfo.member0){
          case 0:
             cout << "Normal";
             break;
          case 1:
             cout << "Emergency";
             break;
          case 2:
             cout << "None";
             break;
          default:
             cout << "Invalid";
             break;
        }
        cout << endl;
        cout << "Status: ";
        switch(prop.CallInfo.member1){
          case 0:
             cout << "IncomingCall";
             break;
          case 1:
             cout << "OutgoingCall";
             break;
          case 2:
             cout << "HungUp";
             break;
          default:
             cout << "Invalid";
             break;
        }
        cout << endl;
        cout << "CallerID: " <<  prop.CallInfo.member2 << endl;
        cout << "CallerFriendlyName: " <<  prop.CallInfo.member3 << endl;
        cout << "<<<<<<<<<<<<<<<<<" << endl;
        cout.flush();
    }

    virtual void OnRemove(const shared_ptr<InComingCallProxy>& p)
    {
        InComingCallProxy::Properties prop = p->GetProperties();
        cout << "\"" << prop.CallInfo.member2 << "\" Removed" << endl;
        cout.flush();
    }
};




int main(int argc, char** argv)
{
    shared_ptr<ObjectAdvertiser> advertiser = ObjectAdvertiser::Create();
    if (nullptr == advertiser) {
        cerr << "Bus Connection is not correctly initialized !!!" << endl;
        return EXIT_FAILURE;
    }

    InComingCallListener incomingCallListener;
    shared_ptr<Observer<InComingCallProxy> > observer1 = Observer<InComingCallProxy>::Create(&incomingCallListener);
    if (nullptr == observer1) {
        cerr << "Observer is not correctly initialized !!!" << endl;
        return EXIT_FAILURE;
    }

    PresenceListener presenceListener;
    shared_ptr<Observer<PresenceProxy> > observer2 = Observer<PresenceProxy>::Create(&presenceListener);
    if (nullptr == observer2) {
        cerr << "Observer is not correctly initialized !!!" << endl;
        return EXIT_FAILURE;
    }

    while (1){
    sleep(100);
    }

    //participant.RemoveFromBus();

    return 0;
}
