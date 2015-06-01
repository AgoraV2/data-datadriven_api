$ ./scom-logger
>>>>>>>>>>>>>>>>>
Update from: ObjectId(bus::lIAvNH-I.2,obj:/O04c48eaace4f4a76ab949ef5fa648242)
Priority: None
Status: HungUp
CallerID: 
CallerFriendlyName: 
<<<<<<<<<<<<<<<<<
>>>>>>>>>>>>>>>>>
Update from: ObjectId(bus::lIAvNH-I.3,obj:/sagemcom): location = sagemcom
Presence: Soon
<<<<<<<<<<<<<<<<<
>>>>>>>>>>>>>>>>>
Update from: ObjectId(bus::99Q9B2eB.2,obj:/O311ae44f30f04d78a14b7f6306764075)
Priority: Emergency
Status: IncomingCall
CallerID: +33156423225
CallerFriendlyName: Mamie
<<<<<<<<<<<<<<<<<
>>>>>>>>>>>>>>>>>
Update from: ObjectId(bus::99Q9B2eB.2,obj:/O311ae44f30f04d78a14b7f6306764075)
Priority: None
Status: HungUp
CallerID: 
CallerFriendlyName: 
<<<<<<<<<<<<<<<<<
>>>>>>>>>>>>>>>>>
Update from: ObjectId(bus::6AFqvaW7.2,obj:/scomPresenceStub): location = scomPresenceStub
Presence: Present
<<<<<<<<<<<<<<<<<
>>>>>>>>>>>>>>>>>
Update from: ObjectId(bus::99Q9B2eB.2,obj:/O311ae44f30f04d78a14b7f6306764075)
Priority: Emergency
Status: IncomingCall
CallerID: +33156423225
CallerFriendlyName: Mamie
<<<<<<<<<<<<<<<<<
>>>>>>>>>>>>>>>>>
Update from: ObjectId(bus::6AFqvaW7.2,obj:/scomPresenceStub): location = scomPresenceStub
Presence: Absent
<<<<<<<<<<<<<<<<<
>>>>>>>>>>>>>>>>>
Update from: ObjectId(bus::99Q9B2eB.2,obj:/O311ae44f30f04d78a14b7f6306764075)
Priority: None
Status: HungUp
CallerID: 
CallerFriendlyName: 
<<<<<<<<<<<<<<<<<
>>>>>>>>>>>>>>>>>
Update from: ObjectId(bus::6AFqvaW7.2,obj:/scomPresenceStub): location = scomPresenceStub
Presence: Present
<<<<<<<<<<<<<<<<<
>>>>>>>>>>>>>>>>>
Update from: ObjectId(bus::99Q9B2eB.2,obj:/O311ae44f30f04d78a14b7f6306764075)
Priority: Emergency
Status: IncomingCall
CallerID: +33156423225
CallerFriendlyName: Mamie
<<<<<<<<<<<<<<<<<
>>>>>>>>>>>>>>>>>
Update from: ObjectId(bus::lIAvNH-I.3,obj:/sagemcom): location = sagemcom
Presence: Soon
<<<<<<<<<<<<<<<<<
>>>>>>>>>>>>>>>>>
Update from: ObjectId(bus::6AFqvaW7.2,obj:/scomPresenceStub): location = scomPresenceStub
Presence: Absent
<<<<<<<<<<<<<<<<<


$ ./scom-call-notifier-stub
Incoming prioritary call
End of prioritary call
Incoming prioritary call
End of prioritary call
Incoming prioritary call
End of prioritary call

$ ./scom-presence-stub 
Update shared state value with: Soon coming from sagemcom
scomPresenceStub: Set Present
scomPresenceStub: Set Absent
scomPresenceStub: Set Present
scomPresenceStub: Set Absent
scomPresenceStub: Set Present

