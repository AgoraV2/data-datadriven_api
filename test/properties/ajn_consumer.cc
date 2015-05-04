/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
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

#include <vector>

#include <qcc/String.h>

#include <alljoyn/BusAttachment.h>
#include <alljoyn/ProxyBusObject.h>

#include <alljoyn/AboutListener.h>
#include <alljoyn/AboutObjectDescription.h>
#include <alljoyn/Init.h>

#include "ConsumerBase.h"
#include "data.h"

using namespace std;
using namespace ajn;

namespace test_system_properties {
class AJNPropertiesConsumer :
    public ConsumerBase,
    private ajn::AboutListener,
    private BusAttachment::JoinSessionAsyncCB,
    private ProxyBusObject::PropertiesChangedListener {
  public:
    AJNPropertiesConsumer() :
        bus("AJNPropertiesConsumer", true), pbo(NULL)
    {
        assert(ER_OK == bus.Start());
        assert(ER_OK == bus.Connect());

        bus.RegisterAboutListener(*this);
        RegisterInterface();
        bus.WhoImplements(NULL);
    }

    ~AJNPropertiesConsumer()
    {
        delete pbo;
    }

    // TODO need ASACORE_47 fix for signal testing
    // TODO check updated properties in signal
    // TODO check invalidated properties in signal

    virtual void WaitForPeer()
    {
        // TODO wait for properties signal
        assert(ER_OK == sync.TimedWait(30 * 1000));
    }

    virtual int32_t GetProperty(const char* name)
    {
        MsgArg val;

        cout << "Consumer get property " << name << endl;
        assert(ER_OK == pbo->GetProperty(IFACE, name, val));

        return MsgArgDereference(val);
    }

    virtual void SetProperty(const char* name, int32_t val)
    {
        MsgArg msgarg_val("i", val);

        cout << "Consumer set property " << name << endl;

        assert(ER_OK == pbo->SetProperty(IFACE, name, msgarg_val));
    }

    virtual void PropertiesChanged(ProxyBusObject& obj,
                                   const char* ifaceName,
                                   const MsgArg& changed,
                                   const MsgArg& invalidated,
                                   void* context)
    {
        updated++;
        cout << "AJN consumer got changed property for " << obj.GetPath() << endl;
        assert(ER_OK == sync.Post());
    }

  private:
    BusAttachment bus;
    ProxyBusObject* pbo;
    qcc::String remoteBusName;
    qcc::String objectPath;
    const InterfaceDescription* iface;

    virtual void Announced(const char* busName, uint16_t version,
                           SessionPort port, const MsgArg& objectDescriptionArg,
                           const MsgArg& aboutDataArg)
    {
        QCC_UNUSED(version);
        QCC_UNUSED(aboutDataArg);

        ajn::AboutObjectDescription objDesc;
        QStatus status = objDesc.CreateFromMsgArg(objectDescriptionArg);
        assert(status == ER_OK);

        // ensure at least one object
        size_t numPathsOd = objDesc.GetPaths(NULL, 0);
        const char** pathsOd = new const char*[numPathsOd];
        objDesc.GetPaths(pathsOd, numPathsOd);

        assert(numPathsOd > 0);

        // get path of object matching our interface
        size_t numPaths = objDesc.GetInterfacePaths(IFACE, NULL, 0);
        const char** paths = new const char*[numPaths];
        objDesc.GetInterfacePaths(IFACE, paths, numPaths);

        assert(numPaths > 0);

        objectPath = paths[0];

        // join session with peer
        if (remoteBusName != busName) {
            SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, false,
                             SessionOpts::PROXIMITY_PHYSICAL, TRANSPORT_ANY);
            assert(
                ER_OK == bus.JoinSessionAsync(busName, port, NULL, opts, this, NULL));
            remoteBusName = busName;
        }
    }

    void JoinSessionCB(QStatus status,
                       ajn::SessionId id,
                       const ajn::SessionOpts& opts,
                       void* context)
    {
        assert(ER_OK == status);
        pbo = new ProxyBusObject(bus, remoteBusName.c_str(), objectPath.c_str(), id);
        assert(NULL != pbo);

        cout << "Consumer add interface for session " << id << endl;

        /* Add interface and register the properties changed handler */
        assert(ER_OK == pbo->AddInterface(*iface));
        bus.EnableConcurrentCallbacks();
        assert(ER_OK == pbo->RegisterPropertiesChangedListener(IFACE, NULL, 0, *this, NULL));

        updated++;
        assert(ER_OK == sync.Post());
    };

    int32_t MsgArgDereference(const MsgArg& value)
    {
        assert(ALLJOYN_VARIANT == value.typeId);
        assert(ALLJOYN_INT32 == value.v_variant.val->typeId);
        return value.v_variant.val->v_int32;
    }

    void RegisterInterface()
    {
        InterfaceDescription* tmp;

        assert(ER_OK == bus.CreateInterface(IFACE, tmp));
        assert(ER_OK == tmp->AddProperty(PROP_RO, "i", PROP_ACCESS_READ));
        assert(ER_OK == tmp->AddProperty(PROP_WO, "i", PROP_ACCESS_WRITE));
        assert(ER_OK == tmp->AddProperty(PROP_RW, "i", PROP_ACCESS_RW));
        assert(ER_OK == tmp->AddProperty(PROP_ET, "i", PROP_ACCESS_RW));
        assert(ER_OK == tmp->AddPropertyAnnotation(PROP_ET, org::freedesktop::DBus::AnnotateEmitsChanged, "true"));
        assert(ER_OK == tmp->AddProperty(PROP_EI, "i", PROP_ACCESS_RW));
        assert(ER_OK ==
               tmp->AddPropertyAnnotation(PROP_EI, org::freedesktop::DBus::AnnotateEmitsChanged, "invalidates"));
        assert(ER_OK == tmp->AddProperty(PROP_EF, "i", PROP_ACCESS_RW));
        assert(ER_OK == tmp->AddPropertyAnnotation(PROP_EF, org::freedesktop::DBus::AnnotateEmitsChanged, "false"));
        tmp->Activate();
        iface = tmp;
    }
};
} /* namespace test_system_properties */

using namespace test_system_properties;

int main(int argc, char** argv)
{
    if (AllJoynInit() != ER_OK) {
        return EXIT_FAILURE;
    }
#ifdef ROUTER
    if (AllJoynRouterInit() != ER_OK) {
        AllJoynShutdown();
        return EXIT_FAILURE;
    }
#endif
    {
        AJNPropertiesConsumer cons;

        cons.Test();
    }
#ifdef ROUTER
    AllJoynRouterShutdown();
#endif
    AllJoynShutdown();

    return 0;
}
