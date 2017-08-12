/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsAbMdbCID_h__
#define nsAbMdbCID_h__

#include "nsAbBaseCID.h"

//
// mdb directory factory
//
#define NS_ABMDBDIRECTORY "moz-abmdbdirectory"

#define NS_ABMDBDIRFACTORY_CONTRACTID \
    NS_AB_DIRECTORY_FACTORY_CONTRACTID_PREFIX NS_ABMDBDIRECTORY

#define NS_ABMDBDIRFACTORY_CID \
{ /* {E1CB9C8A-722D-43E4-9D7B-7CCAE4B0338A}*/ \
    0xE1CB9C8A, 0x722D, 0x43E4, \
    {0x9D, 0x7B, 0x7C, 0xCA, 0xE4, 0xB0, 0x33, 0x8A} \
}

//
// nsAbMDBDirectory
//
#define NS_ABMDBDIRECTORY_CONTRACTID \
    NS_AB_DIRECTORY_TYPE_CONTRACTID_PREFIX NS_ABMDBDIRECTORY

#define NS_ABMDBDIRECTORY_CID \
{ /* {e618f894-1dd1-11b2-889c-9aaefaa90dde}*/ \
    0xe618f894, 0x1dd1, 0x11b2, \
    {0x88, 0x9c, 0x9a, 0xae, 0xfa, 0xa9, 0x0d, 0xde} \
}

//
// nsAbMDBCard
//
#define NS_ABMDBCARD_CONTRACTID \
    "@mozilla.org/addressbook/moz-abmdbcard;1"

#define NS_ABMDBCARD_CID \
{ /* {f578a5d2-1dd1-11b2-8841-f45cc5e765f8} */ \
    0xf578a5d2, 0x1dd1, 0x11b2, \
    {0x88, 0x41, 0xf4, 0x5c, 0xc5, 0xe7, 0x65, 0xf8} \
}

#endif // nsAbMdbCID_h__
