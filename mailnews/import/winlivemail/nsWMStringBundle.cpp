/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "prprf.h"
#include "prmem.h"
#include "nsCOMPtr.h"
#include "nsMsgUtils.h"
#include "nsIStringBundle.h"
#include "nsWMStringBundle.h"
#include "nsIServiceManager.h"
#include "nsIURI.h"
#include "mozilla/Services.h"

#define WM_MSGS_URL       "chrome://messenger/locale/wmImportMsgs.properties"

nsIStringBundle *  nsWMStringBundle::m_pBundle = nullptr;

nsIStringBundle *nsWMStringBundle::GetStringBundle(void)
{
  if (m_pBundle)
    return m_pBundle;

  char*        propertyURL = WM_MSGS_URL;
  nsIStringBundle*  sBundle = nullptr;

  nsCOMPtr<nsIStringBundleService> sBundleService =
    mozilla::services::GetStringBundleService();
  if (sBundleService) {
    sBundleService->CreateBundle(propertyURL, &sBundle);
  }

  m_pBundle = sBundle;

  return sBundle;
}

void nsWMStringBundle::GetStringByID(int32_t stringID, nsString& result)
{
  PRUnichar *ptrv = GetStringByID(stringID);
  result = ptrv;
  FreeString(ptrv);
}

PRUnichar *nsWMStringBundle::GetStringByID(int32_t stringID)
{
  if (!m_pBundle)
    m_pBundle = GetStringBundle();

  if (m_pBundle) {
    PRUnichar *ptrv = nullptr;
    nsresult rv = m_pBundle->GetStringFromID(stringID, &ptrv);

    if (NS_SUCCEEDED(rv) && ptrv)
      return ptrv;
  }

  nsString resultString;
  resultString.AppendLiteral("[StringID ");
  resultString.AppendInt(stringID);
  resultString.AppendLiteral("?]");

  return ToNewUnicode(resultString);
}

void nsWMStringBundle::Cleanup(void)
{
  if (m_pBundle)
    m_pBundle->Release();
  m_pBundle = nullptr;
}
