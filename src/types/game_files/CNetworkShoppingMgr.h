#include "types/netshop/CNetShopTransaction.hpp"


#include <cstdint>
namespace rage
{
	class sysMemAllocator;
}
class CNetworkShoppingMgr {
	public:

	CNetworkShoppingMgr();
	~CNetworkShoppingMgr();

	void InitCatalog( );
	void Init(rage::sysMemAllocator* pAllocator);
	void Shutdown(const uint32_t shutdownMode);
	void Update( );

	//Returns TRUE is the services/categories/transaction types are valid.
	bool        GetCategoryIsValid(const void* category)    const;
	bool GetTransactionTypeIsValid(const void* type) const;
	bool      GetActionTypeIsValid(const void* action)    const;
	bool ActionAllowsNegativeOrZeroValue(const void* action) const;
	/* -------------------------------------- */
	/* BASKET                                 */

	bool                 CreateBasket(void*& id, const void* category, const void* action, const int flags);
	bool                 DeleteBasket( );
	bool                      AddItem(void*& item);
	bool                   RemoveItem(const void* itemId);
	bool                     FindItem(const void* itemId) const;
	bool                  ClearBasket( );
	bool                IsBasketEmpty( ) const;
	bool                 IsBasketFull( ) const;
	uint32_t               GetNumberItems( ) const;


	/* -------------------------------------- */
	/* SERVICES                               */

	bool                 BeginService(void* id, const void* type, const void* category, const void* service, const void* action, const int price, const int flags);
	bool                   EndService( const void* id );


	/* -------------------------------------- */
	/* BASKET + SERVICES                      */

	bool                StartCheckout(const void* id);
	bool                    GetStatus(const void* id, uint32_t code) const;
	bool                GetFailedCode(const void* id, int& code) const;


	//Find our basket for the inventory.
	const void**  FindBasketConst(void*& transId) const { return FindBasket(transId); }

	//Accessor's for services/categories/transaction types.
	const void* GetTransactionTypes( ) const {return m_transactiontypes;};

	//Return TRUE if we should make a NULL transaction.
	bool ShouldDoNullTransaction() const;

	//Return TRUE if a transaction is in Progress.
	bool TransactionInProgress() const;

	//Find any transaction given a transaction id.
	const void**  FindTransaction(const void* id) const;
	void**        FindTransaction(const void* id);

	//Find any transaction given a transaction type and a service id.
	CNetShopTransaction*        FindService( const void* type, const void* id );
	const CNetShopTransaction*  FindService( const void* type, const void* id ) const;

	//Accessor for pending cash reductions
	void*& GetCashReductions() {return m_cashreductions;}

private:

	//Cancel and cleanup in flight transactions.
	void CancelTransactions( );

	//Returns if there are free spaces in the pool or if we have managed to create free spaces.
	bool  CreateFreeSpaces(const bool removeAlsofailed = false);

	//Append a new node.
	bool  AppendNewNode(void** transaction);

	//Find our basket for the inventory.
	void**        FindBasket(void*& transId);
	const void**  FindBasket(void*& transId) const;

public:

	//Valid list of services/categories/transaction types
	void* m_transactiontypes;
	void* m_actiontypes;

	//A list of transactions waiting for processing.
	void* m_TransactionList;

	//Memory allocator
	rage::sysMemAllocator* m_Allocator;

	//Flag init Catalog from cache
	bool  m_LoadCatalogFromCache;

	//Flag transaction in progress.
	bool  m_transactionInProgress;

	//Keep track of cash reductions
	void* m_cashreductions;

};
