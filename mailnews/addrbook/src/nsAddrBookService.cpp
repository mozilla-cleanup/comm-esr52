
#include "nsAddrBookService.h"
#include "nsIAbManager.h"
#include "nsIAbDirectory.h"
#include "nsISimpleEnumerator.h"
#include "nsCOMPtr.h"
#include "nsAbBaseCID.h"
#include "nsServiceManagerUtils.h"	// do_GetService
#include "nsString.h"


NS_IMPL_ISUPPORTS(nsAddrBookService, nsIAddrBookService)

static nsCOMPtr<nsIAbManager> global_abManager = nullptr;

static nsIAbManager* getAbManager()
{
    // TODO: locking / atomics + refcounting ?
    nsCOMPtr<nsIAbManager> mgr(global_abManager);
    if (mgr == nullptr) {
        mgr = do_GetService(NS_ABMANAGER_CONTRACTID);
        if (mgr == nullptr) {
            printf("nsAddrBookService: failed to get abmanager\n");
        }
        global_abManager = mgr;
    }
    return mgr;
}

nsAddrBookService::nsAddrBookService()
{
}

nsAddrBookService::~nsAddrBookService()
{
}

/**
 * Fill the mailing list members into recipient list
 */
static nsresult fillMailListRecipients(nsCOMPtr<nsIAbDirectory> ml, nsCOMPtr<nsIMutableArray> list, int depth)
{
    /** FIXME: check for potential infinite recursion **/
    if (depth > 10) {
        printf("fillMailListRecipients() recursing too deep\n");
        return NS_ERROR_ABORT;
    }

    nsCString dirURI;
    ml->GetURI(dirURI);

    /** process recursive maillists **/
    nsCOMPtr<nsISimpleEnumerator> sub;
    if (NS_SUCCEEDED(ml->GetChildNodes(getter_AddRefs(sub))) && sub)
    {
        bool hasMore;
        while (NS_SUCCEEDED(sub->HasMoreElements(&hasMore)) && hasMore)
        {
            nsCOMPtr<nsISupports> item;
            if (NS_FAILED(sub->GetNext(getter_AddRefs(item))))
                continue;

            nsCOMPtr<nsIAbDirectory> dirwalk(do_QueryInterface(item));
            if (dirwalk == nullptr)
                continue;

            fillMailListRecipients(dirwalk, list, depth+1);
        }
    }

    /** process cards **/
    nsCOMPtr<nsISimpleEnumerator> cards;
    if (NS_SUCCEEDED(ml->GetChildCards(getter_AddRefs(cards))) && cards)
    {
        bool hasMore;
        while (NS_SUCCEEDED(cards->HasMoreElements(&hasMore)) && hasMore)
        {
            nsCOMPtr<nsISupports> item;
            if (NS_FAILED(cards->GetNext(getter_AddRefs(item))))
                continue;

            nsCOMPtr<nsIAbCard> cardwalk(do_QueryInterface(item));
            if (cardwalk == nullptr)
                continue;

            cardwalk->SetDirectoryURI(dirURI);
            list->AppendElement(cardwalk, false);
        }
    }

    return NS_OK;
}

/*
 * Fill specified contact(s) into array, w/ resolving mailing lists / aliases
 *
 * TODO: do direct lookups instead of list scanning when possible
 * TODO: should have an indexed cache
 */
static nsresult fillRecipients(nsCOMPtr<nsIAbDirectory> directory, const nsAString & addr, nsCOMPtr<nsIMutableArray> list)
{
    nsresult rv;

    nsCString dir_uri;
    directory->GetURI(dir_uri);
    nsString dir_name;
    directory->GetDirName(dir_name);
    bool isML = false;
    directory->GetIsMailList(&isML);

    printf("traversing directory: uri=%s\n", dir_uri.get());
    printf("                      name=%s\n", ToNewUTF8String(dir_name));
    printf("                      %s\n", (isML ? "is maillist" : "not maillist"));
    printf("      looking for: %s\n", ToNewUTF8String(addr));

    /** is this the maillist we're looking for ? **/
    if (isML) {
        if (addr.Equals(dir_name)) {
            /* TODO: add to cache */
            return fillMailListRecipients(directory, list, 0);
        }
        return NS_OK;
    }

    /** process sub directories and maillists **/
    nsCOMPtr<nsISimpleEnumerator> sub;
    if (NS_SUCCEEDED(directory->GetChildNodes(getter_AddRefs(sub))) && sub)
    {
        bool hasMore;
        while (NS_SUCCEEDED(sub->HasMoreElements(&hasMore)) && hasMore)
        {
            nsCOMPtr<nsISupports> item;
            if (NS_FAILED(sub->GetNext(getter_AddRefs(item))))
                continue;

            nsCOMPtr<nsIAbDirectory> dirwalk(do_QueryInterface(item));
            if (dirwalk == nullptr)
                continue;

            rv = fillRecipients(dirwalk, addr, list);
            if (NS_FAILED(rv))
                printf("recursive fillRecipients() failed\n");
        }
    }

    /** process cards **/
    nsCOMPtr<nsISimpleEnumerator> cards;
    if (NS_SUCCEEDED(directory->GetChildCards(getter_AddRefs(cards))) && cards)
    {
        bool hasMore;
        while (NS_SUCCEEDED(cards->HasMoreElements(&hasMore)) && hasMore)
        {
            nsCOMPtr<nsISupports> item;
            if (NS_FAILED(cards->GetNext(getter_AddRefs(item))))
                continue;

            nsCOMPtr<nsIAbCard> cardwalk(do_QueryInterface(item));
            if (cardwalk == nullptr)
                continue;

            nsString email;
            cardwalk->GetPrimaryEmail(email);
            if (email.Equals(addr)) {
                list->AppendElement(cardwalk, false);
                cardwalk->SetDirectoryURI(dir_uri);
                /* TODO: put it into cache */
            }
        }
    }

    return NS_OK;
}

static nsresult fillRecipients(const nsACString &aDirUri, const nsAString & addr, nsCOMPtr<nsIMutableArray> aDirArray)
{
    /* TODO: lookup the cache first */
    nsCOMPtr<nsIAbManager> mgr(getAbManager());
    nsCOMPtr<nsIAbDirectory> directory;
    nsresult rv = mgr->GetDirectory(aDirUri, getter_AddRefs(directory));
    NS_ENSURE_SUCCESS(rv, rv);
    return fillRecipients(directory, addr, aDirArray);
}

/* void FillRecipients (in AString name, in nsIArray list); */
NS_IMETHODIMP nsAddrBookService::FillRecipients(const nsAString & addr, nsIMutableArray *list)
{
    return fillRecipients(NS_LITERAL_CSTRING("moz-abdirectory://"), addr, list);
}

/* nsIArray FindRecipients (in AString name); */
NS_IMETHODIMP nsAddrBookService::FindRecipients(const nsAString & addr, nsIArray * *_retval)
{
    nsresult rv = NS_OK;
    nsCOMPtr<nsIMutableArray> list = do_CreateInstance(NS_ARRAY_CONTRACTID, &rv);
    NS_ENSURE_SUCCESS(rv, rv);
    *_retval = list;
    return FillRecipients(addr, list);
}

/* void FlushCache (); */
NS_IMETHODIMP nsAddrBookService::FlushCache()
{
    global_abManager = nullptr;
    return NS_OK;
}

/* nsIAbDirectory GetCardDirectory (in nsIAbCard card); */
NS_IMETHODIMP nsAddrBookService::GetCardDirectory(nsIAbCard *card, nsIAbDirectory * *_retval)
{
    NS_ENSURE_ARG_POINTER(card);

    nsCString id;
    nsresult rv;

    rv = card->GetDirectoryURI(id);
    NS_ENSURE_SUCCESS(rv, rv);

    nsCOMPtr<nsIAbManager> mgr(getAbManager());

    rv = mgr->GetDirectory(id, _retval);
    if (NS_FAILED(rv)) {
        printf("nsAddrBookService::GetCardDirectory() failed to retrieve directory by uri\n");
    }

    NS_ENSURE_SUCCESS(rv, rv);
    return rv;
}

/* nsIAbDirectory getDirectory (in ACString aURI); */
NS_IMETHODIMP nsAddrBookService::GetDirectory(const nsACString & aURI, nsIAbDirectory * *_retval)
{
    nsCOMPtr<nsIAbManager> mgr(getAbManager());
    return mgr->GetDirectory(aURI, _retval);
}
