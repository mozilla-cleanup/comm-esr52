/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsMsgSendReport.h"

#include "msgCore.h"
#include "nsIMsgCompose.h"
#include "nsMsgCompCID.h"
#include "nsMsgPrompts.h"
#include "nsError.h"
#include "nsComposeStrings.h"
#include "nsIStringBundle.h"
#include "nsServiceManagerUtils.h"
#include "mozilla/Services.h"

NS_IMPL_ISUPPORTS(nsMsgProcessReport, nsIMsgProcessReport)

nsMsgProcessReport::nsMsgProcessReport()
{
  Reset();
}

nsMsgProcessReport::~nsMsgProcessReport()
{
}

/* attribute boolean proceeded; */
NS_IMETHODIMP nsMsgProcessReport::GetProceeded(bool *aProceeded)
{
  NS_ENSURE_ARG_POINTER(aProceeded);
  *aProceeded = mProceeded;
  return NS_OK;
}
NS_IMETHODIMP nsMsgProcessReport::SetProceeded(bool aProceeded)
{
  mProceeded = aProceeded;
  return NS_OK;
}

/* attribute nsresult error; */
NS_IMETHODIMP nsMsgProcessReport::GetError(nsresult *aError)
{
  NS_ENSURE_ARG_POINTER(aError);
  *aError = mError;
  return NS_OK;
}
NS_IMETHODIMP nsMsgProcessReport::SetError(nsresult aError)
{
  mError = aError;
  return NS_OK;
}

/* attribute wstring message; */
NS_IMETHODIMP nsMsgProcessReport::GetMessage(char16_t * *aMessage)
{
  NS_ENSURE_ARG_POINTER(aMessage);
  *aMessage = ToNewUnicode(mMessage);
  return NS_OK;
}
NS_IMETHODIMP nsMsgProcessReport::SetMessage(const char16_t * aMessage)
{
  mMessage = aMessage;
  return NS_OK;
}

/* void Reset (); */
NS_IMETHODIMP nsMsgProcessReport::Reset()
{
  mProceeded = false;
  mError = NS_OK;
  mMessage.Truncate();

  return NS_OK;
}


NS_IMPL_ISUPPORTS(nsMsgSendReport, nsIMsgSendReport)

nsMsgSendReport::nsMsgSendReport()
{
  uint32_t i;
  for (i = 0; i <= SEND_LAST_PROCESS; i ++)
    mProcessReport[i] = new nsMsgProcessReport();

  Reset(); 
}

nsMsgSendReport::~nsMsgSendReport()
{
  uint32_t i;
  for (i = 0; i <= SEND_LAST_PROCESS; i ++)
    mProcessReport[i] = nullptr;
}

/* attribute long currentProcess; */
NS_IMETHODIMP nsMsgSendReport::GetCurrentProcess(int32_t *aCurrentProcess)
{
  NS_ENSURE_ARG_POINTER(aCurrentProcess);
  *aCurrentProcess = mCurrentProcess;
  return NS_OK;
}
NS_IMETHODIMP nsMsgSendReport::SetCurrentProcess(int32_t aCurrentProcess)
{
  if (aCurrentProcess < 0 || aCurrentProcess > SEND_LAST_PROCESS)
    return NS_ERROR_ILLEGAL_VALUE;

  mCurrentProcess = aCurrentProcess;
  if (mProcessReport[mCurrentProcess])
    mProcessReport[mCurrentProcess]->SetProceeded(true);

  return NS_OK;
}

/* attribute long deliveryMode; */
NS_IMETHODIMP nsMsgSendReport::GetDeliveryMode(int32_t *aDeliveryMode)
{
  NS_ENSURE_ARG_POINTER(aDeliveryMode);
  *aDeliveryMode = mDeliveryMode;
  return NS_OK;
}
NS_IMETHODIMP nsMsgSendReport::SetDeliveryMode(int32_t aDeliveryMode)
{
  mDeliveryMode = aDeliveryMode;
  return NS_OK;
}

/* void Reset (); */
NS_IMETHODIMP nsMsgSendReport::Reset()
{
  uint32_t i;
  for (i = 0; i <= SEND_LAST_PROCESS; i ++)
    if (mProcessReport[i])
      mProcessReport[i]->Reset();

  mCurrentProcess = 0;
  mDeliveryMode = 0;
  mAlreadyDisplayReport = false;

  return NS_OK;
}

/* void setProceeded (in long process, in boolean proceeded); */
NS_IMETHODIMP nsMsgSendReport::SetProceeded(int32_t process, bool proceeded)
{
  if (process < process_Current || process > SEND_LAST_PROCESS)
    return NS_ERROR_ILLEGAL_VALUE;

  if (process == process_Current)
    process = mCurrentProcess;

  if (!mProcessReport[process])
    return NS_ERROR_NOT_INITIALIZED;

  return mProcessReport[process]->SetProceeded(proceeded);
}

/* void setError (in long process, in nsresult error, in boolean overwriteError); */
NS_IMETHODIMP nsMsgSendReport::SetError(int32_t process, nsresult newError, bool overwriteError)
{
  if (process < process_Current || process > SEND_LAST_PROCESS)
    return NS_ERROR_ILLEGAL_VALUE;

  if (process == process_Current)
  {
    if (mCurrentProcess == process_Current)
      // We don't know what we're currently trying to do
      return NS_ERROR_ILLEGAL_VALUE;

    process = mCurrentProcess;
  }

  if (!mProcessReport[process])
    return NS_ERROR_NOT_INITIALIZED;

  nsresult currError = NS_OK;
  mProcessReport[process]->GetError(&currError);
  if (overwriteError || NS_SUCCEEDED(currError))
    return mProcessReport[process]->SetError(newError);
  else
    return NS_OK;
}

/* void setMessage (in long process, in wstring message, in boolean overwriteMessage); */
NS_IMETHODIMP nsMsgSendReport::SetMessage(int32_t process, const char16_t *message, bool overwriteMessage)
{
  if (process < process_Current || process > SEND_LAST_PROCESS)
    return NS_ERROR_ILLEGAL_VALUE;

  if (process == process_Current)
  {
    if (mCurrentProcess == process_Current)
      // We don't know what we're currently trying to do
      return NS_ERROR_ILLEGAL_VALUE;

    process = mCurrentProcess;
  }

  if (!mProcessReport[process])
    return NS_ERROR_NOT_INITIALIZED;

  nsString currMessage;
  mProcessReport[process]->GetMessage(getter_Copies(currMessage));
  if (overwriteMessage || currMessage.IsEmpty())
    return mProcessReport[process]->SetMessage(message);
  else
    return NS_OK;
}

/* nsIMsgProcessReport getProcessReport (in long process); */
NS_IMETHODIMP nsMsgSendReport::GetProcessReport(int32_t process, nsIMsgProcessReport **_retval)
{
  NS_ENSURE_ARG_POINTER(_retval);
  if (process < process_Current || process > SEND_LAST_PROCESS)
    return NS_ERROR_ILLEGAL_VALUE;

  if (process == process_Current)
  {
    if (mCurrentProcess == process_Current)
      // We don't know what we're currently trying to do
      return NS_ERROR_ILLEGAL_VALUE;

    process = mCurrentProcess;
  }

  NS_IF_ADDREF(*_retval = mProcessReport[process]);
  return NS_OK;
}

/* nsresult displayReport (in nsIPrompt prompt, in boolean showErrorOnly, in boolean dontShowReportTwice); */
NS_IMETHODIMP nsMsgSendReport::DisplayReport(nsIPrompt *prompt, bool showErrorOnly, bool dontShowReportTwice, nsresult *_retval)
{
  NS_ENSURE_ARG_POINTER(_retval);

  NS_ENSURE_TRUE(mCurrentProcess >= 0 && mCurrentProcess <= SEND_LAST_PROCESS,
                 NS_ERROR_NOT_INITIALIZED);

  nsresult currError = NS_OK;
  mProcessReport[mCurrentProcess]->GetError(&currError);
  *_retval = currError;

  if (dontShowReportTwice && mAlreadyDisplayReport)
    return NS_OK;

  if (showErrorOnly && NS_SUCCEEDED(currError))
    return NS_OK;

  nsString currMessage;
  mProcessReport[mCurrentProcess]->GetMessage(getter_Copies(currMessage));

  nsresult rv; // don't step on currError.
  nsCOMPtr<nsIStringBundleService> bundleService =
    mozilla::services::GetStringBundleService();
  NS_ENSURE_TRUE(bundleService, NS_ERROR_UNEXPECTED);
  nsCOMPtr<nsIStringBundle> bundle;
  rv = bundleService->CreateBundle("chrome://messenger/locale/messengercompose/composeMsgs.properties", getter_AddRefs(bundle));
  if (NS_FAILED(rv))
  {
    //TODO need to display a generic hardcoded message
    mAlreadyDisplayReport = true;
    return NS_OK;  
  }

  nsString dialogTitle;
  nsString dialogMessage;

  if (NS_SUCCEEDED(currError))
  {
    //TODO display a success error message
    return NS_OK;
  }

  //Do we have an explanation of the error? if no, try to build one...
  if (currMessage.IsEmpty())
  {
    switch (currError)
    {
      case NS_BINDING_ABORTED:
      case NS_ERROR_SEND_FAILED:
      case NS_ERROR_SEND_FAILED_BUT_NNTP_OK:
      case NS_MSG_FAILED_COPY_OPERATION:
      case NS_MSG_UNABLE_TO_SEND_LATER:
      case NS_MSG_UNABLE_TO_SAVE_DRAFT:
      case NS_MSG_UNABLE_TO_SAVE_TEMPLATE:
        //Ignore, don't need to repeat ourself.
        break;
      default:
        const char16_t* errorString = errorStringNameForErrorCode(currError);
        nsMsgGetMessageByName(errorString, currMessage);
        break;
    }
  }

  if (mDeliveryMode == nsIMsgCompDeliverMode::Now || mDeliveryMode == nsIMsgCompDeliverMode::SendUnsent)
  {
    // SMTP is taking care of it's own error message and will return NS_ERROR_BUT_DONT_SHOW_ALERT as error code.
    // In that case, we must not show an alert ourself.
    if (currError == NS_ERROR_BUT_DONT_SHOW_ALERT)
    {
      mAlreadyDisplayReport = true;
      return NS_OK;
    }

    bundle->GetStringFromName(u"sendMessageErrorTitle",
                              getter_Copies(dialogTitle));

    const char16_t* preStrName = u"sendFailed";
    bool askToGoBackToCompose = false;
    switch (mCurrentProcess)
    {
      case process_BuildMessage :
        preStrName = u"sendFailed";
        askToGoBackToCompose = false;
        break;
      case process_NNTP :
        preStrName = u"sendFailed";
        askToGoBackToCompose = false;
        break;
      case process_SMTP :
        bool nntpProceeded;
        mProcessReport[process_NNTP]->GetProceeded(&nntpProceeded);
        if (nntpProceeded)
          preStrName = u"sendFailedButNntpOk";
        else
          preStrName = u"sendFailed";
        askToGoBackToCompose = false;
        break;
      case process_Copy:
        preStrName = u"failedCopyOperation";
        askToGoBackToCompose = (mDeliveryMode == nsIMsgCompDeliverMode::Now);
        break;
      case process_FCC:
        preStrName = u"failedCopyOperation";
        askToGoBackToCompose = (mDeliveryMode == nsIMsgCompDeliverMode::Now);
        break;
    }
    bundle->GetStringFromName(preStrName, getter_Copies(dialogMessage));

    //Do we already have an error message?
    if (!askToGoBackToCompose && currMessage.IsEmpty())
    {
      //we don't have an error description but we can put a generic explanation
      bundle->GetStringFromName(u"genericFailureExplanation",
                                getter_Copies(currMessage));
    }

    if (!currMessage.IsEmpty())
    {
      //Don't need to repeat ourself!
      if (!currMessage.Equals(dialogMessage))
      {
        if (!dialogMessage.IsEmpty())
          dialogMessage.Append(char16_t('\n'));
        dialogMessage.Append(currMessage);
      }
    }

    if (askToGoBackToCompose)
    {
      bool oopsGiveMeBackTheComposeWindow = true;
      nsString text1;
      bundle->GetStringFromName(u"returnToComposeWindowQuestion",
                                getter_Copies(text1));
      if (!dialogMessage.IsEmpty())
        dialogMessage.AppendLiteral("\n");
      dialogMessage.Append(text1);
      nsMsgAskBooleanQuestionByString(prompt, dialogMessage.get(), &oopsGiveMeBackTheComposeWindow, dialogTitle.get());
      if (!oopsGiveMeBackTheComposeWindow)
        *_retval = NS_OK;
    }
    else
      nsMsgDisplayMessageByString(prompt, dialogMessage.get(), dialogTitle.get());
  }
  else
  {
    const char16_t* title;
    const char16_t* messageName;

    switch (mDeliveryMode)
    {
      case nsIMsgCompDeliverMode::Later:
        title = u"sendLaterErrorTitle";
        messageName = u"unableToSendLater";
        break;

      case nsIMsgCompDeliverMode::AutoSaveAsDraft:
      case nsIMsgCompDeliverMode::SaveAsDraft:
        title = u"saveDraftErrorTitle";
        messageName = u"unableToSaveDraft";
        break;

      case nsIMsgCompDeliverMode::SaveAsTemplate:
        title = u"saveTemplateErrorTitle";
        messageName = u"unableToSaveTemplate";
        break;

      default:
        /* This should never happen! */
        title = u"sendMessageErrorTitle";
        messageName = u"sendFailed";
        break;
    }

    bundle->GetStringFromName(title, getter_Copies(dialogTitle));
    bundle->GetStringFromName(messageName,
                              getter_Copies(dialogMessage));

    //Do we have an error message...
    if (currMessage.IsEmpty())
    {
      //we don't have an error description but we can put a generic explanation
      bundle->GetStringFromName(u"genericFailureExplanation",
                                getter_Copies(currMessage));
    }

    if (!currMessage.IsEmpty())
    {
      if (!dialogMessage.IsEmpty())
        dialogMessage.Append(char16_t('\n'));
      dialogMessage.Append(currMessage);
    }
    nsMsgDisplayMessageByString(prompt, dialogMessage.get(), dialogTitle.get());
  }

  mAlreadyDisplayReport = true;
  return NS_OK;
}

