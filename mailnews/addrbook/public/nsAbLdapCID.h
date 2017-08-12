/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsAbLDAPCID_h__
#define nsAbLDAPCID_h__

#include "nsISupports.h"
#include "nsIFactory.h"
#include "nsIComponentManager.h"
#include "nsAbBaseCID.h"

//
// nsAbLDAPDirectory
//
#define NS_ABLDAPDIRECTORY_CONTRACTID \
    NS_AB_DIRECTORY_TYPE_CONTRACTID_PREFIX "moz-abldapdirectory"

#define NS_ABLDAPDIRECTORY_CID \
{ /* {783E2777-66D7-4826-9E4B-8AB58C228A52} */ \
    0x783E2777, 0x66D7, 0x4826, \
    {0x9E, 0x4B, 0x8A, 0xB5, 0x8C, 0x22, 0x8A, 0x52} \
}

//
// nsAbLDAPDirectoryQuery
//
#define NS_ABLDAPDIRECTORYQUERY_CONTRACTID \
    "@mozilla.org/addressbook/ldap-directory-query;1"

#define NS_ABLDAPDIRECTORYQUERY_CID \
{ /* {783E2777-66D7-4826-9E4B-8AB58C228A53}*/ \
    0x783E2777, 0x66D7, 0x4826, \
    {0x9E, 0x4B, 0x8A, 0xB5, 0x8C, 0x22, 0x8A, 0x53} \
}

//
// nsAbLDAPCard
//
#define NS_ABLDAPCARD_CONTRACTID \
    "@mozilla.org/addressbook/moz-abldapcard"

#define NS_ABLDAPCARD_CID \
{ /* {10307B01-EBD6-465F-B972-1630410F70E6} */ \
    0x10307B01, 0xEBD6, 0x465F, \
    {0xB9, 0x72, 0x16, 0x30, 0x41, 0x0F, 0x70, 0xE6} \
}

//
// LDAP directory factory
//
#define NS_ABLDAPDIRFACTORY_CONTRACTID \
    NS_AB_DIRECTORY_FACTORY_CONTRACTID_PREFIX "moz-abldapdirectory"

#define NS_ABLDAPDIRFACTORY_CID \
{ /* {8e3701af-8828-426c-84ac-124825c778f8} */ \
    0x8e3701af, 0x8828, 0x426c, \
    {0x84, 0xac, 0x12, 0x48, 0x25, 0xc7, 0x78, 0xf8} \
}

//
// LDAP autocomplete directory factory
//
#define NS_ABLDAPACDIRFACTORY_CONTRACTID \
    NS_AB_DIRECTORY_FACTORY_CONTRACTID_PREFIX "ldap"
#define NS_ABLDAPSACDIRFACTORY_CONTRACTID \
    NS_AB_DIRECTORY_FACTORY_CONTRACTID_PREFIX "ldaps"

//
// nsAbLDAPAutoCompFormatter
//
#define NS_ABLDAPAUTOCOMPFORMATTER_CID \
{ /* {4e276d6d-9981-46b4-9070-92f344ac5f5a} */\
    0x4e276d6d, 0x9981, 0x46b4, \
    {0x90, 0x70, 0x92, 0xf3, 0x44, 0xac, 0x5f, 0x5a} \
}

#define NS_ABLDAPAUTOCOMPFORMATTER_CONTRACTID \
    "@mozilla.org/ldap-autocomplete-formatter;1?type=addrbook"

//
// nsAbLDAPReplicationService
//
#define NS_ABLDAP_REPLICATIONSERVICE_CID \
{ /* {ece81280-2639-11d6-b791-00b0d06e5f27} */ \
    0xece81280, 0x2639, 0x11d6, \
    {0xb7, 0x91, 0x00, 0xb0, 0xd0, 0x6e, 0x5f, 0x27} \
}

#define NS_ABLDAP_REPLICATIONSERVICE_CONTRACTID \
    "@mozilla.org/addressbook/ldap-replication-service;1"

//
// nsAbLDAPReplicationQuery
//
#define NS_ABLDAP_REPLICATIONQUERY_CID \
{ /* {5414fff0-263b-11d6-b791-00b0d06e5f27} */ \
    0x5414fff0, 0x263b, 0x11d6, \
    {0xb7, 0x91, 0x00, 0xb0, 0xd0, 0x6e, 0x5f, 0x27} \
}

#define NS_ABLDAP_REPLICATIONQUERY_CONTRACTID \
    "@mozilla.org/addressbook/ldap-replication-query;1"

//
// nsAbLDAPChangeLogQuery
//
#define NS_ABLDAP_CHANGELOGQUERY_CID \
{ /* {63E11D51-3C9B-11d6-B7B9-00B0D06E5F27} */ \
    0x63e11d51, 0x3c9b, 0x11d6, \
    {0xb7, 0xb9, 0x00, 0xb0, 0xd0, 0x6e, 0x5f, 0x27} \
}

#define NS_ABLDAP_CHANGELOGQUERY_CONTRACTID \
    "@mozilla.org/addressbook/ldap-changelog-query;1"

//
// nsAbLDAPProcessReplicationData
//
#define NS_ABLDAP_PROCESSREPLICATIONDATA_CID \
{ /* {5414fff1-263b-11d6-b791-00b0d06e5f27} */ \
	0x5414fff1, 0x263b, 0x11d6, \
	{0xb7, 0x91, 0x00, 0xb0, 0xd0, 0x6e, 0x5f, 0x27} \
}

#define NS_ABLDAP_PROCESSREPLICATIONDATA_CONTRACTID \
	"@mozilla.org/addressbook/ldap-process-replication-data;1"

//
// nsAbLDAPProcessChangeLogData
//
#define NS_ABLDAP_PROCESSCHANGELOGDATA_CID \
{ /* {63E11D52-3C9B-11d6-B7B9-00B0D06E5F27} */ \
    0x63e11d52, 0x3c9b, 0x11d6, \
    {0xb7, 0xb9, 0x00, 0xb0, 0xd0, 0x6e, 0x5f, 0x27} \
}

#define NS_ABLDAP_PROCESSCHANGELOGDATA_CONTRACTID \
    "@mozilla.org/addressbook/ldap-process-changelog-data;1"

#define NS_ABLDIFSERVICE_CID \
{ \
    0xdb6f46da, 0x8de3, 0x478d, \
    {0xb5, 0x39, 0x80, 0x13, 0x98, 0x65, 0x6c, 0xf6} \
}

#define NS_ABLDIFSERVICE_CONTRACTID \
    "@mozilla.org/addressbook/abldifservice;1"

#endif // nsAbLDAPCID_h__
