/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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
 * Mozilla Japan.
 * Portions created by the Initial Developer are Copyright (C) 2009
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Makoto Kato <m_kato@ga2.so-net.ne.jp>
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

#include "mozilla/ModuleUtils.h"
#include "nsISupports.h"
#include "nsCOMPtr.h"

#include "nsIFactory.h"
#include "nsIServiceManager.h"
#include "nsIModule.h"

#include "nsFts3TokenizerCID.h"

#include "nsFts3Tokenizer.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(nsFts3Tokenizer)

NS_DEFINE_NAMED_CID(NS_FTS3TOKENIZER_CID);

const mozilla::Module::CIDEntry kFts3TokenizerCIDs[] = {
  { &kNS_FTS3TOKENIZER_CID, false, NULL, nsFts3TokenizerConstructor },
  { NULL }
};

const mozilla::Module::ContractIDEntry kFts3TokenizerContracts[] = {
  { NS_FTS3TOKENIZER_CONTRACTID, &kNS_FTS3TOKENIZER_CID },
  { NULL }
};

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

static const mozilla::Module kFts3TokenizerModule = {
    mozilla::Module::kVersion,
    kFts3TokenizerCIDs,
    kFts3TokenizerContracts
};

NSMODULE_DEFN(fts3tokenizer) = &kFts3TokenizerModule;
