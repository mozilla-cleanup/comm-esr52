/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 *
 * Portions created by the Initial Developer are Copyright (C) 2008 
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Nick Kreeger <nick.kreeger@park.edu>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "nsMsgReadStateTxn.h"

#include "nsIMutableArray.h"
#include "nsIMsgHdr.h"
#include "nsComponentManagerUtils.h"


nsMsgReadStateTxn::nsMsgReadStateTxn()
{
}

nsMsgReadStateTxn::~nsMsgReadStateTxn()
{
}

nsresult
nsMsgReadStateTxn::Init(nsIMsgFolder *aParentFolder,
                        PRUint32 aNumKeys,
                        nsMsgKey *aMsgKeyArray)
{
  NS_ENSURE_ARG_POINTER(aParentFolder);

  mParentFolder = aParentFolder;
  mMarkedMessages.AppendElements(aMsgKeyArray, aNumKeys);

  return nsMsgTxn::Init();
}

NS_IMETHODIMP 
nsMsgReadStateTxn::UndoTransaction()
{
  return MarkMessages(PR_FALSE);
}

NS_IMETHODIMP 
nsMsgReadStateTxn::RedoTransaction()
{
  return MarkMessages(PR_TRUE);
}

NS_IMETHODIMP
nsMsgReadStateTxn::MarkMessages(bool aAsRead)
{
  nsresult rv;
  nsCOMPtr<nsIMutableArray> messageArray = 
    do_CreateInstance(NS_ARRAY_CONTRACTID, &rv);
  NS_ENSURE_SUCCESS(rv, rv);

  PRUint32 length = mMarkedMessages.Length();
  for (PRUint32 i = 0; i < length; i++) {
    nsCOMPtr<nsIMsgDBHdr> curMsgHdr;
    rv = mParentFolder->GetMessageHeader(mMarkedMessages[i], 
                                         getter_AddRefs(curMsgHdr));
    if (NS_SUCCEEDED(rv) && curMsgHdr) {
      messageArray->AppendElement(curMsgHdr, PR_FALSE);
    }
  }

  return mParentFolder->MarkMessagesRead(messageArray, aAsRead);
}

