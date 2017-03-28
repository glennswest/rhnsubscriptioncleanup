# Redhat RHN Subscription Cleanup
## Overview
Doing lots of testing on a routine basis means my RHN Redhat account gets 
overgrown with systems that have since been deleted. This implements
a cleanup, that deletes all the extra systems. It basis this on time of last
checkin. 

The default value is if a system has not checked into the rhn network
after 5 days, its subscription is removed.

Note that this utility should not be used in rhn redhat accounts that have
production systems, which may go offline for longer periods.

## Usage
Setup the following environment variables:
user = rhn email address 
password = rhn password

Ownerid can be found by using a existing redhat machine:

```
[root@bastion glennswest]# subscription-manager orgs
Username: gwest@redhat.com
Password: 
+-------------------------------------------+
          gwest@redhat.com Organizations
+-------------------------------------------+

Name: 7777777
Key:  7777777


export ownerid=XXXXXXX
export user=XXXXXXX
export password='XXXXXXX'
```

## Notes
This is a nice example of using json data in C, and using curl to do 
rest http calls.
