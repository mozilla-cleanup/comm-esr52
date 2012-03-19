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
 * The Original Code is Mozilla
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corp.
 * Portions created by the Initial Developer are Copyright (C) 2001
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): Krishna Mohan Khandrika (kkhandrika@netscape.com)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
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

#undef UNICODE
#undef _UNICODE

#include "msgMapiFactory.h"
#include "msgMapiImp.h"
#include "msgMapi.h"

CMapiFactory ::CMapiFactory()
: m_cRef(1)
{
}

CMapiFactory::~CMapiFactory()
{
}

STDMETHODIMP CMapiFactory::QueryInterface(const IID& aIid, void** aPpv)
{    
    if ((aIid == IID_IUnknown) || (aIid == IID_IClassFactory))
    {
        *aPpv = static_cast<IClassFactory*>(this); 
    }
    else
    {
        *aPpv = nsnull;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUnknown*>(*aPpv)->AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) CMapiFactory::AddRef()
{
    return (PR_ATOMIC_INCREMENT(&m_cRef));
}

STDMETHODIMP_(ULONG) CMapiFactory::Release() 
{
    PRInt32 temp;
    temp = PR_ATOMIC_DECREMENT(&m_cRef);
    if (m_cRef == 0)
    {
        delete this;
        return 0;
    }

    return temp;
}

STDMETHODIMP CMapiFactory::CreateInstance(IUnknown* aUnknownOuter,
                                           const IID& aIid,
                                           void** aPpv) 
{
    // Cannot aggregate.

    if (aUnknownOuter != nsnull)
    {
        return CLASS_E_NOAGGREGATION ;
    }

    // Create component.

    CMapiImp* pImp = new CMapiImp();
    if (pImp == nsnull)
    {
        return E_OUTOFMEMORY ;
    }

    // Get the requested interface.
    HRESULT hr = pImp->QueryInterface(aIid, aPpv);

    // Release the IUnknown pointer.
    // (If QueryInterface failed, component will delete itself.)

    pImp->Release();
    return hr;
}

STDMETHODIMP CMapiFactory::LockServer(BOOL aLock) 
{
    return S_OK ;
}
