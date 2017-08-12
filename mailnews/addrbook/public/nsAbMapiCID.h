/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsAbOutlookCID_h__
#define nsAbOutlookCID_h__

#include "nsISupports.h"
#include "nsIFactory.h"
#include "nsIComponentManager.h"
#include "nsAbBaseCID.h"

//
// nsAbOutlookDirectory
//
#define NS_ABOUTLOOKDIRECTORY_CONTRACTID \
    NS_AB_DIRECTORY_TYPE_CONTRACTID_PREFIX "moz-aboutlookdirectory"

#define NS_ABOUTLOOKDIRECTORY_CID \
{ /* {9cc57822-0599-4c47-a399-1c6fa185a05c} */ \
    0x9cc57822, 0x0599, 0x4c47, \
    {0xa3, 0x99, 0x1c, 0x6f, 0xa1, 0x85, 0xa0, 0x5c} \
}

//
// Outlook directory factory
//
#define NS_ABOUTLOOKDIRFACTORY_CONTRACTID \
    NS_AB_DIRECTORY_FACTORY_CONTRACTID_PREFIX "moz-aboutlookdirectory"

#define NS_ABOUTLOOKDIRFACTORY_CID \
{ /* {558ccc0f-2681-4dac-a066-debd8d26faf6} */ \
    0x558ccc0f, 0x2681, 0x4dac, \
    {0xa0, 0x66, 0xde, 0xbd, 0x8d, 0x26, 0xfa, 0xf6} \
}

#endif // nsAbOutlookCID_h__
