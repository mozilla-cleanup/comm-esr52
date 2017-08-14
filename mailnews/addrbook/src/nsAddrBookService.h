
#include "nsIAddrBookService.h"
#include "nsIMutableArray.h"

class nsAddrBookService : public nsIAddrBookService
{
public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIADDRBOOKSERVICE

    nsAddrBookService();

private:
    virtual ~nsAddrBookService();
};
