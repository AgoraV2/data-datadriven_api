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
#include <cstdio>

using namespace std;
using namespace datadriven;
using namespace gen::com_agora_v2;

class InComingCall :
    public ProvidedObject, public InComingCallInterface {
  public:
    InComingCall(shared_ptr<ObjectAdvertiser> advertiser,
                uint32_t member0, uint32_t member1, qcc::String member2, qcc::String member3) :
        ProvidedObject(advertiser),
        InComingCallInterface(this)
    {
        CallInfo.member0 = member0;
        CallInfo.member1 = member1;
        CallInfo.member2 = member2;
        CallInfo.member3 = member3;
    }
    void UpdateCallInfo(uint32_t member0, uint32_t member1, qcc::String member2, qcc::String member3){
        CallInfo.member0 = member0;
        CallInfo.member1 = member1;
        CallInfo.member2 = member2;
        CallInfo.member3 = member3;
    }
};


int main(int argc, char** argv)
{
    shared_ptr<ObjectAdvertiser> advertiser = ObjectAdvertiser::Create();
    if (nullptr == advertiser) {
        cerr << "Bus Connection is not correctly initialized !!!" << endl;
        return EXIT_FAILURE;
    }

    /* create one or more participants */
    uint32_t member0 = 2;
    uint32_t member1 = 2;
    qcc::String member2 = qcc::String("");
    qcc::String member3 = qcc::String("");

    InComingCall participant(advertiser, member0, member1, member2, member3);
    if (ER_OK != participant.PutOnBus()) { //This effectively publishes the object
        cerr << "Failed to announce participant !" << endl;
    }

    while (true) {
	    printf("Incoming prioritary call\n");
        member0 = 1;
        member1 = 0;
        member2 = qcc::String("+33156423225");
        member3 = qcc::String("Mamie");
        participant.UpdateCallInfo(member0, member1, member2, member3);
        participant.Update();
        sleep(5);
	    printf("End of prioritary call\n");
        member0 = 2;
        member1 = 2;
        member2 = qcc::String("");
        member3 = qcc::String("");
        participant.UpdateCallInfo(member0, member1, member2, member3);
        participant.Update();
        sleep(5);
    }

    //participant.RemoveFromBus();

    return 0;
}
