#pragma once

#include "Packet.h"

enum EBonusSwitcherConfig
{
#ifndef ENABLE_SWITCH_ITEM_SELECT
	SWITCH_VNUM = 71084,
	SWITCH_VNUM_RARE = 71052,
#endif
	PAGE_MAX_NUM = 7,
	ALT_MAX_NUM = 5,
	STATUS_EMPTY = 0,
	STATUS_ACTIVE,
	STATUS_INACTIVE,
	STATUS_DONE,
	IDLE_TIME = 2000,
};

enum ESwitchSpeed
{
	SPEED_SLOW = 1000,
	SPEED_NORMAL = 500,
	SPEED_FAST = 300,
	SPEED_PREMIUM = 150,
};

enum EBonusSwitcherErrorCode
{
	ERROR_WRONG_PAGE,
	ERROR_ITEM_ALREADY_ADDED,
	ERROR_ITEM_NOT_FOUND,
	ERROR_ITEM_DATA_NOT_FOUND,
	ERROR_BAD_ITEM_TYPE,
	ERROR_OK,
};

typedef struct SBonusSwitcherItem
{
	BYTE	byCell;
	TPlayerItemAttribute	aDestAttr[ALT_MAX_NUM][ITEM_ATTRIBUTE_SLOT_MAX_NUM];
	BYTE	byStatus;
	BYTE	byStatusRare;
	DWORD	dwItemVnum;
	long	lStartTime;
	long	lEndTime;
	WORD	wChangesUsed;
	DWORD	dwLastChange;
	BOOL	bUpdate;
#ifdef ENABLE_SWITCH_ITEM_SELECT
	DWORD	dwSwitchItem;
	DWORD	dwSwitchItemRare;
#endif

	void TBonusSwitcherItem()
	{
		byCell = c_ItemSlot_Count + 1;
		byStatus = STATUS_INACTIVE;
		byStatusRare = STATUS_INACTIVE;
		dwItemVnum = 0;
		lStartTime = 0;
		lEndTime = 0;
		wChangesUsed = 0;
		dwLastChange = 0;
		bUpdate = false;
#ifdef ENABLE_SWITCH_ITEM_SELECT
		dwSwitchItem = 0;
		dwSwitchItemRare = 0;
#endif

		memset(aDestAttr, 0, sizeof(aDestAttr));
	}

} TBonusSwitcherItem;

class CBonusSwitcher : public CSingleton<CBonusSwitcher>
{

public:
	CBonusSwitcher();
	virtual ~CBonusSwitcher();

	int AddItem(BYTE byPage, BYTE byCell);

	BYTE GetItemPos(BYTE byPage);

	void SetDestAttr(BYTE byPage, BYTE byAttrPos, BYTE bAlt, TPlayerItemAttribute aAttr);
	TPlayerItemAttribute GetDestAttr(BYTE byPage, BYTE byAttrPos, BYTE bAlt);

	void SetStatus(BYTE byPage, BYTE byStatus);
	BYTE GetStatus(BYTE byPage);
	
	void SetStatusRare(BYTE byPage, BYTE byStatus);
	BYTE GetStatusRare(BYTE byPage);

	DWORD GetItemIndex(BYTE byPage);

	void SetSwitchDelay(WORD wDelay);
	WORD GetSwitchDelay();

	void SetStartTime(BYTE byPage, long lStartTime);
	long GetStartTime(BYTE byPage);

#ifdef ENABLE_SWITCH_BUY_ITEM_NPC
	void BuySwitcherFromShop(DWORD dwVnum);
#endif

	void SetEndTime(BYTE byPage, long lEndTime);
	long GetEndTime(BYTE byPage);

	void SetChangesUsed(BYTE byPage, WORD wChangesUsed);
	WORD GetChangesUsed(BYTE byPage);	
	
#ifdef ENABLE_SWITCH_ITEM_SELECT
	void SetSwitchItem(BYTE byPage, DWORD dwVnum, bool bRare);
	DWORD GetSwitchItem(BYTE byPage, bool bRare);
#endif

	void Update();
	void SetUpdatePos(BYTE byPage, BOOL flag);
	bool GetUpdatePos(BYTE byPage);

	BYTE GetPosByCell(BYTE byPage);

	bool CanUsePremium();

	void Clear(BYTE byPage = PAGE_MAX_NUM + 1);
	void SetPhaseWnd(PyObject* poPhaseWnd) { m_apoWnd = poPhaseWnd; }
	PyObject* GetPhaseWnd() { return m_apoWnd; }

	void LoadData(std::string szFileName);
	void SaveData(std::string szFileName);

protected:
	TBonusSwitcherItem m_pkItem[PAGE_MAX_NUM];
	WORD m_wSwitchDelay;
	void __Update(BYTE byPage);
	PyObject* m_apoWnd;
};