/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsAbOsxCID_h__
#define nsAbOsxCID_h__

#include "nsISupports.h"
#include "nsIFactory.h"
#include "nsIComponentManager.h"
#include "nsAbBaseCID.h"

//
// nsAbOSXDirectory
//
#define NS_ABOSXDIRECTORY_PREFIX "moz-abosxdirectory"
#define NS_ABOSXCARD_PREFIX "moz-abosxcard"

#define NS_ABOSXDIRECTORY_CONTRACTID \
    NS_AB_DIRECTORY_TYPE_CONTRACTID_PREFIX NS_ABOSXDIRECTORY_PREFIX

#define NS_ABOSXDIRECTORY_CID \
{ /* {83781cc6-c682-11d6-bdeb-0005024967b8} */ \
    0x83781cc6, 0xc682, 0x11d6, \
    {0xbd, 0xeb, 0x00, 0x05, 0x02, 0x49, 0x67, 0xb8} \
}

//
// nsAbOSXCard
//
#define NS_ABOSXCARD_CONTRACTID \
  NS_AB_DIRECTORY_TYPE_CONTRACTID_PREFIX NS_ABOSXCARD_PREFIX

#define NS_ABOSXCARD_CID \
{ /* {89bbf582-c682-11d6-bc9d-0005024967b8} */ \
    0x89bbf582, 0xc682, 0x11d6, \
    {0xbc, 0x9d, 0x00, 0x05, 0x02, 0x49, 0x67, 0xb8} \
}

//
// OS X directory factory
//
#define NS_ABOSXDIRFACTORY_CONTRACTID \
  NS_AB_DIRECTORY_FACTORY_CONTRACTID_PREFIX NS_ABOSXDIRECTORY_PREFIX

#define NS_ABOSXDIRFACTORY_CID \
{ /* {90efe2fe-c682-11d6-9c83-0005024967b8} */ \
    0x90efe2fe, 0xc682, 0x11d6, \
    {0x9c, 0x83, 0x00, 0x05, 0x02, 0x49, 0x67, 0xb8} \
}

#endif // nsAbOsxCID_h__
