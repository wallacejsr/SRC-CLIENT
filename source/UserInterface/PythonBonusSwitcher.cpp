#include "StdAfx.h"
#include "PythonBonusSwitcher.h"
#include "Packet.h"
#include "PythonPlayer.h"
#include "PythonNetworkStream.h"
#include "../gamelib/ItemManager.h"

#ifdef ENABLE_SWITCH_BUY_ITEM_NPC
#include "PythonShop.h"
bool BuyShopPacket = true;
#endif

#ifdef ENABLE_BONUS_SWITCHER
CBonusSwitcher::CBonusSwitcher() : m_pkItem(), m_wSwitchDelay(SPEED_NORMAL), m_apoWnd(nullptr)
{	
	Clear();
}

CBonusSwitcher::~CBonusSwitcher()
{
	Clear();
	m_apoWnd = nullptr;
}

int CBonusSwitcher::AddItem(BYTE byPage, BYTE byCell)
{
	if (byPage > PAGE_MAX_NUM)
		return ERROR_WRONG_PAGE;

	for (int i = 0; i < PAGE_MAX_NUM; i++)
	{
		if (m_pkItem[i].byCell == byCell)
			return ERROR_ITEM_ALREADY_ADDED;
	}

	const auto& pkPlayer = CPythonPlayer::Instance();
	const auto pkItem = pkPlayer.GetItemData(TItemPos(INVENTORY, byCell));

	if (!pkItem)
		return ERROR_ITEM_NOT_FOUND;

	CItemData* pItemData;
	if (!CItemManager::Instance().GetItemDataPointer(pkItem->vnum, &pItemData))
		return ERROR_ITEM_DATA_NOT_FOUND;

	if ((pItemData->GetType() == pItemData->ITEM_TYPE_WEAPON && pItemData->GetSubType() < pItemData->WEAPON_ARROW)
		|| (pItemData->GetType() == pItemData->ITEM_TYPE_ARMOR))
	{
		
		auto& table = m_pkItem[byPage];
		table = {};

		table.byCell = byCell;
		table.dwItemVnum = pkItem->vnum;
		table.byStatus = STATUS_INACTIVE;
		table.byStatusRare = STATUS_INACTIVE;

		return ERROR_OK;
	}

	return ERROR_BAD_ITEM_TYPE;
}

BYTE CBonusSwitcher::GetItemPos(BYTE byPage)
{
	if (byPage > PAGE_MAX_NUM)
		return c_ItemSlot_Count + 1;

	return m_pkItem[byPage].byCell;
}

void CBonusSwitcher::SetDestAttr(BYTE byPage, BYTE byAttrPos, BYTE bAlt, TPlayerItemAttribute aAttr)
{
	if (byPage > PAGE_MAX_NUM)
		return;
		
	m_pkItem[byPage].aDestAttr[bAlt][byAttrPos] = aAttr;
}

TPlayerItemAttribute CBonusSwitcher::GetDestAttr(BYTE byPage, BYTE byAttrPos, BYTE bAlt)
{
	if (byPage > PAGE_MAX_NUM)
	{
		TPlayerItemAttribute defaultValue;
		defaultValue.bType = defaultValue.sValue = 0;
		return defaultValue;
	}

	return m_pkItem[byPage].aDestAttr[bAlt][byAttrPos];
}

void CBonusSwitcher::SetStatus(BYTE byPage, BYTE byStatus)
{
	if (byPage > PAGE_MAX_NUM)
		return;

	m_pkItem[byPage].byStatus = byStatus;

	if (byStatus == STATUS_ACTIVE)
	{
		long lTime = static_cast<long>(time(0));
		SetStartTime(byPage, lTime);
		SetChangesUsed(byPage, 0);
		SetEndTime(byPage, 0);
		__Update(byPage);
	}

	PyCallClassMemberFunc(GetPhaseWnd(), "BINARY_RefreshStatus", Py_BuildValue("()"));
}

BYTE CBonusSwitcher::GetStatus(BYTE byPage)
{
	if (byPage > PAGE_MAX_NUM)
		return STATUS_EMPTY;

	return m_pkItem[byPage].byStatus;
}

void CBonusSwitcher::SetStatusRare(BYTE byPage, BYTE byStatus)
{
	if (byPage > PAGE_MAX_NUM)
		return;

	m_pkItem[byPage].byStatusRare = byStatus;
	if (byStatus == STATUS_ACTIVE)
		__Update(byPage);
}

BYTE CBonusSwitcher::GetStatusRare(BYTE byPage)
{
	if (byPage > PAGE_MAX_NUM)
		return STATUS_EMPTY;

	return m_pkItem[byPage].byStatusRare;
}

DWORD CBonusSwitcher::GetItemIndex(BYTE byPage)
{
	if (byPage > PAGE_MAX_NUM)
		return 0;

	return m_pkItem[byPage].dwItemVnum;
}

void CBonusSwitcher::SetSwitchDelay(WORD wDelay)
{
	m_wSwitchDelay = wDelay;
}

WORD CBonusSwitcher::GetSwitchDelay()
{
	return m_wSwitchDelay;
}

void CBonusSwitcher::SetStartTime(BYTE byPage, long lStartTime)
{
	if (byPage > PAGE_MAX_NUM)
		return;

	m_pkItem[byPage].lStartTime = lStartTime;
}

#ifdef ENABLE_SWITCH_BUY_ITEM_NPC
void CBonusSwitcher::BuySwitcherFromShop(DWORD dwVnum)
{
	if (!BuyShopPacket) { return; }
	for (int i = 0; i < SHOP_HOST_ITEM_MAX_NUM; i++)
	{
		const TShopItemData* c_pItemData;
		if (CPythonShop::Instance().GetItemData(i, &c_pItemData))
		{
			if (c_pItemData->vnum == dwVnum)
			{
				if (c_pItemData->price.dwPrice <= CPythonPlayer::Instance().GetStatus(POINT_GOLD))
				{
					CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
					rkNetStream.SendShopBuyPacket(i);
					BuyShopPacket = false;
					break;
				}
			}
		}
	}
}
#endif

long CBonusSwitcher::GetStartTime(BYTE byPage)
{
	if (byPage > PAGE_MAX_NUM)
		return 0;

	return m_pkItem[byPage].lStartTime;
}

void CBonusSwitcher::SetEndTime(BYTE byPage, long lEndTime)
{
	if (byPage > PAGE_MAX_NUM)
		return;
	
	m_pkItem[byPage].lEndTime = lEndTime;
}

long CBonusSwitcher::GetEndTime(BYTE byPage)
{
	if (byPage > PAGE_MAX_NUM)
		return 0;

	return m_pkItem[byPage].lEndTime;
}

void CBonusSwitcher::SetChangesUsed(BYTE byPage, WORD wChangesUsed)
{
	if (byPage > PAGE_MAX_NUM)
		return;

	m_pkItem[byPage].wChangesUsed = wChangesUsed;
}

WORD CBonusSwitcher::GetChangesUsed(BYTE byPage)
{
	if (byPage > PAGE_MAX_NUM)
		return 0;

	return m_pkItem[byPage].wChangesUsed;
}

#ifdef ENABLE_SWITCH_ITEM_SELECT
void CBonusSwitcher::SetSwitchItem(BYTE byPage, DWORD dwVnum, bool bRare)
{
	if (byPage > PAGE_MAX_NUM)
		return;

	if (bRare)
		m_pkItem[byPage].dwSwitchItemRare = dwVnum;
	else
		m_pkItem[byPage].dwSwitchItem = dwVnum;
}

DWORD CBonusSwitcher::GetSwitchItem(BYTE byPage, bool bRare)
{
	if (byPage > PAGE_MAX_NUM)
		return 0;

	return bRare ? m_pkItem[byPage].dwSwitchItemRare : m_pkItem[byPage].dwSwitchItem;
}
#endif

void CBonusSwitcher::Update()
{
	DWORD dwTime = ELTimer_GetMSec();
	for (int i = 0; i < PAGE_MAX_NUM; i++)
	{
		if (GetUpdatePos(i))
		{
			__Update(i);
			SetUpdatePos(i, false);
		}
		if ((GetStatus(i) == STATUS_ACTIVE || GetStatusRare(i) == STATUS_ACTIVE) && m_pkItem[i].dwLastChange + IDLE_TIME >= dwTime)
			SetUpdatePos(i, true);
	}

	if (GetSwitchDelay() == SPEED_PREMIUM && !CanUsePremium())
		SetSwitchDelay(SPEED_SLOW);
}

void CBonusSwitcher::SetUpdatePos(BYTE byPage, BOOL flag)
{
	if (byPage > PAGE_MAX_NUM)
		return;

	if (m_pkItem[byPage].bUpdate == flag)
		return;

	m_pkItem[byPage].bUpdate = flag;
}

BYTE CBonusSwitcher::GetPosByCell(BYTE byPage)
{
	for (int i = 0; i < PAGE_MAX_NUM; i++)
		if (GetItemPos(i) == byPage)
			return i;

	return c_ItemSlot_Count + 1;
}

bool CBonusSwitcher::GetUpdatePos(BYTE bPos)
{
	if (!(0 < bPos < PAGE_MAX_NUM))
		return false;

	return m_pkItem[bPos].bUpdate;
}

void CBonusSwitcher::__Update(BYTE byPage)
{
	if (byPage > PAGE_MAX_NUM)
		return;

	if (GetStatus(byPage) == STATUS_ACTIVE || GetStatusRare(byPage) == STATUS_ACTIVE)
	{
		BYTE bItemPos = GetItemPos(byPage);

		if (bItemPos < c_ItemSlot_Count + 1)
		{
			TItemPos Cell = TItemPos(INVENTORY, bItemPos);

			if (GetItemIndex(byPage) == CPythonPlayer::Instance().GetItemIndex(Cell))
			{
				if (GetStatus(byPage) == STATUS_ACTIVE)
				{
					bool need_change = false;

					for (int i = 0; i < ALT_MAX_NUM; i++)
					{
						const auto destAttr = m_pkItem[byPage].aDestAttr[i];
						auto dest = 0;
						auto ok = 0;

						for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM - 2; j++)
						{
							if (destAttr[j].bType)
							{
								dest++;

								for (int k = 0; k < ITEM_ATTRIBUTE_SLOT_MAX_NUM - 2; k++)
								{
									TPlayerItemAttribute curAttr;
									CPythonPlayer::Instance().GetItemAttribute(Cell, k, &curAttr.bType, &curAttr.sValue);

									if (curAttr.bType == destAttr[j].bType && curAttr.sValue >= destAttr[j].sValue)
									{
										ok++;
										break;
									}
								}
							}
						}

						if (dest)
							need_change = true;

						if (dest && dest == ok)
						{
							need_change = false;
							break;
						}
					}

					if (need_change)
					{
#ifdef ENABLE_SWITCH_BUY_ITEM_NPC
						int ItemCount = CPythonPlayer::Instance().GetItemCountByVnum(GetSwitchItem(byPage, false));
						if (ItemCount <= 0)
						{
							BuyShopPacket = true;
							BuySwitcherFromShop(GetSwitchItem(byPage, false));
						}
#endif
						// for (int slot = 0; slot < c_ItemSlot_Count; slot++) // original
						for (int slot = 0; slot < c_Inventory_Count; slot++)
						{
							DWORD dwItemIndex = CPythonPlayer::Instance().GetItemIndex(TItemPos(INVENTORY, slot));
#ifdef ENABLE_SWITCH_ITEM_SELECT
							if (dwItemIndex == GetSwitchItem(byPage, false))
#else
							if (dwItemIndex == SWITCH_VNUM)
#endif
							{
								CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
								rkNetStream.SendItemUseToItemPacket(TItemPos(INVENTORY, slot), Cell);
								SetChangesUsed(byPage, GetChangesUsed(byPage) + 1);
								break;
							}
						}
					}
					else
					{
						SetStatus(byPage, STATUS_DONE);
						SetEndTime(byPage, time(0));
					}
				}
				
				if (GetStatusRare(byPage) == STATUS_ACTIVE)
				{
					bool need_change = false;

					for (int i = 0; i < ALT_MAX_NUM; i++)
					{
						const auto destAttr = m_pkItem[byPage].aDestAttr[i];
						auto dest = 0;
						auto ok = 0;

						for (int j = ITEM_ATTRIBUTE_SLOT_MAX_NUM - 2; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; j++)
						{
							if (destAttr[j].bType)
							{
								dest++;

								for (int k = ITEM_ATTRIBUTE_SLOT_MAX_NUM - 2; k < ITEM_ATTRIBUTE_SLOT_MAX_NUM; k++)
								{
									TPlayerItemAttribute curAttr;
									CPythonPlayer::Instance().GetItemAttribute(Cell, k, &curAttr.bType, &curAttr.sValue);

									if (curAttr.bType == destAttr[j].bType && curAttr.sValue >= destAttr[j].sValue)
									{
										ok++;
										break;
									}
								}
							}
						}

						if (dest)
							need_change = true;

						if (dest && dest == ok)
						{
							need_change = false;
							break;
						}
					}

					if (need_change)
					{
						for (int l = 0; l < c_ItemSlot_Count; l++)
						{
							DWORD dwItemIndex = CPythonPlayer::Instance().GetItemIndex(TItemPos(INVENTORY, l));
#ifdef ENABLE_SWITCH_ITEM_SELECT
							if (dwItemIndex == GetSwitchItem(byPage, true))
#else
							if (dwItemIndex == SWITCH_VNUM_RARE)
#endif							
							{
								CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
								rkNetStream.SendItemUseToItemPacket(TItemPos(INVENTORY, l), Cell);
								SetChangesUsed(byPage, GetChangesUsed(byPage) + 1);
								break;
							}
						}
					}
					else
					{
						SetStatusRare(byPage, STATUS_DONE);
					}
				}
			}
			else
			{
				SetStatus(byPage, STATUS_EMPTY);
				m_pkItem[byPage].dwItemVnum = 0;
			}
		}
	}
	m_pkItem[byPage].dwLastChange = ELTimer_GetMSec();
	PyCallClassMemberFunc(GetPhaseWnd(), "BINARY_Refresh", Py_BuildValue("()"));
}

bool CBonusSwitcher::CanUsePremium()
{
	// TODO: make real check for that
	// TODO: it can be point
	return true;
}

void CBonusSwitcher::Clear(BYTE byPage)
{
	if (byPage <= PAGE_MAX_NUM)
	{
		memset(&m_pkItem[byPage], 0, sizeof(TBonusSwitcherItem));

		SetStatus(byPage, STATUS_EMPTY);
		SetStatusRare(byPage, STATUS_EMPTY);

		m_pkItem[byPage].byCell = c_ItemSlot_Count + 1;

		PyCallClassMemberFunc(GetPhaseWnd(), "BINARY_Refresh", Py_BuildValue("()"));
		PyCallClassMemberFunc(GetPhaseWnd(), "BINARY_RefreshAttribute", Py_BuildValue("()"));
		return;
	}

	for (int i = 0; i < PAGE_MAX_NUM; i++)
	{
		memset(&m_pkItem[i], 0, sizeof(TBonusSwitcherItem));

		SetStatus(i, STATUS_EMPTY);
		SetStatusRare(i, STATUS_EMPTY);

		m_pkItem[i].byCell = c_ItemSlot_Count + 1;
	}
	SetPhaseWnd(nullptr);
}

void CBonusSwitcher::LoadData(std::string szFileName)
{
	FILE* infile;

	infile = fopen(szFileName.c_str(), "r");
	if (infile == nullptr)
		return;

	int i = 0;
	while (fread(&m_pkItem[i], sizeof(TBonusSwitcherItem), 1, infile))
	{
		i++;
	}

	for (int i = 0; i < PAGE_MAX_NUM; i++)
	{
		auto& item = m_pkItem[i];

		if (item.byStatus == STATUS_INACTIVE && item.byStatusRare == STATUS_INACTIVE)
			memset(&item.aDestAttr, 0, sizeof(item.aDestAttr));
	}

	PyCallClassMemberFunc(GetPhaseWnd(), "BINARY_Refresh", Py_BuildValue("()"));

	fclose(infile);
}

void CBonusSwitcher::SaveData(std::string szFileName)
{
	FILE* outfile;

	outfile = fopen(szFileName.c_str(), "w");
	if (outfile == nullptr)
		return;

	for (int i = 0; i < PAGE_MAX_NUM; i++)
		fwrite(&m_pkItem[i], sizeof(TBonusSwitcherItem), 1, outfile);

	fclose(outfile);
}

PyObject* bsAddItem(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byPage;
	if (!PyTuple_GetByte(poArgs, 0, &byPage))
		return Py_BadArgument();

	BYTE byCell;
	if (!PyTuple_GetByte(poArgs, 1, &byCell))
		return Py_BadArgument();

	BYTE byResult = CBonusSwitcher::Instance().AddItem(byPage, byCell);
	return Py_BuildValue("i", byResult);
}

PyObject* bsGetDestAttr(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byPage;
	if (!PyTuple_GetByte(poArgs, 0, &byPage))
		return Py_BadArgument();

	BYTE byPos;
	if (!PyTuple_GetByte(poArgs, 1, &byPos))
		return Py_BadArgument();	
	
	BYTE byAlt;
	if (!PyTuple_GetByte(poArgs, 2, &byAlt))
		return Py_BadArgument();

	TPlayerItemAttribute aAttr = CBonusSwitcher::Instance().GetDestAttr(byPage, byPos, byAlt);
	return Py_BuildValue("ii", aAttr.bType, aAttr.sValue);
}

PyObject* bsSetDestAttr(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byPage;
	if (!PyTuple_GetByte(poArgs, 0, &byPage))
		return Py_BadArgument();

	BYTE byPos;
	if (!PyTuple_GetByte(poArgs, 1, &byPos))
		return Py_BadArgument();

	BYTE byAlt;
	if (!PyTuple_GetByte(poArgs, 2, &byAlt))
		return Py_BadArgument();

	BYTE byType;
	if (!PyTuple_GetByte(poArgs, 3, &byType))
		return Py_BadArgument();	
		
	int iValue;
	if (!PyTuple_GetInteger(poArgs, 4, &iValue))
		return Py_BadArgument();

	TPlayerItemAttribute aAttr;
	aAttr.bType = byType;
	aAttr.sValue = iValue;

	CBonusSwitcher::Instance().SetDestAttr(byPage, byPos, byAlt, aAttr);
	return Py_BuildNone();
}

PyObject* bsGetStatus(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byPage;
	if (!PyTuple_GetByte(poArgs, 0, &byPage))
		return Py_BadArgument();

	BYTE byStatus = CBonusSwitcher::Instance().GetStatus(byPage);
	return Py_BuildValue("i", byStatus);
}

PyObject* bsSetStatus(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byPage;
	if (!PyTuple_GetByte(poArgs, 0, &byPage))
		return Py_BadArgument();

	BYTE byStatus;
	if (!PyTuple_GetByte(poArgs, 1, &byStatus))
		return Py_BadArgument();

	CBonusSwitcher::Instance().SetStatus(byPage, byStatus);
	return Py_BuildNone();
}

PyObject* bsGetStatusRare(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byPage;
	if (!PyTuple_GetByte(poArgs, 0, &byPage))
		return Py_BadArgument();

	BYTE byStatus = CBonusSwitcher::Instance().GetStatusRare(byPage);
	return Py_BuildValue("i", byStatus);
}

PyObject* bsSetStatusRare(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byPage;
	if (!PyTuple_GetByte(poArgs, 0, &byPage))
		return Py_BadArgument();

	BYTE byStatus;
	if (!PyTuple_GetByte(poArgs, 1, &byStatus))
		return Py_BadArgument();

	CBonusSwitcher::Instance().SetStatusRare(byPage, byStatus);
	return Py_BuildNone();
}

PyObject* bsGetItemPos(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byPage;
	if (!PyTuple_GetByte(poArgs, 0, &byPage))
		return Py_BadArgument();

	int iItemCell = CBonusSwitcher::Instance().GetItemPos(byPage);
	return Py_BuildValue("i", iItemCell);
}

PyObject* bsGetItemIndex(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byPage;
	if (!PyTuple_GetByte(poArgs, 0, &byPage))
		return Py_BadArgument();

	int iItemVnum = CBonusSwitcher::Instance().GetItemIndex(byPage);
	return Py_BuildValue("i", iItemVnum);
}

PyObject* bsSetSwitchDelay(PyObject* poSelf, PyObject* poArgs)
{
	int iSpeed;
	if (!PyTuple_GetInteger(poArgs, 0, &iSpeed))
		return Py_BadArgument();

	if (iSpeed == SPEED_SLOW || iSpeed == SPEED_NORMAL || iSpeed == SPEED_FAST || (iSpeed == SPEED_PREMIUM && CBonusSwitcher::Instance().CanUsePremium()))
		CBonusSwitcher::Instance().SetSwitchDelay(iSpeed);
	
	return Py_BuildNone();

}

PyObject* bsGetSwitchDelay(PyObject* poSelf, PyObject* poArgs)
{
	int iSwitchDelay = CBonusSwitcher::Instance().GetSwitchDelay();
	return Py_BuildValue("i", iSwitchDelay);
}

PyObject* bsGetChangesUsed(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byPage;
	if (!PyTuple_GetByte(poArgs, 0, &byPage))
		return Py_BadArgument();

	int iChangesUsed = CBonusSwitcher::Instance().GetChangesUsed(byPage);
	return Py_BuildValue("i", iChangesUsed);
}

#ifdef ENABLE_SWITCH_ITEM_SELECT
PyObject* bsSetSwitchItem(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byPage;
	if (!PyTuple_GetByte(poArgs, 0, &byPage))
		return Py_BadArgument();	
	
	DWORD dwVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 1, &dwVnum))
		return Py_BadArgument();	
	
	bool bRare;
	if (!PyTuple_GetBoolean(poArgs, 2, &bRare))
		return Py_BadArgument();

	CBonusSwitcher::Instance().SetSwitchItem(byPage, dwVnum, bRare);
	return Py_BuildNone();
}

PyObject* bsGetSwitchItem(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byPage;
	if (!PyTuple_GetByte(poArgs, 0, &byPage))
		return Py_BadArgument();

	bool bRare;
	if (!PyTuple_GetBoolean(poArgs, 1, &bRare))
		return Py_BadArgument();

	DWORD dwVnum = CBonusSwitcher::Instance().GetSwitchItem(byPage, bRare);
	return Py_BuildValue("i", dwVnum);
}
#endif

PyObject* bsGetStartTime(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byPage;
	if (!PyTuple_GetByte(poArgs, 0, &byPage))
		return Py_BadArgument();

	long szTime = CBonusSwitcher::Instance().GetStartTime(byPage);
	return Py_BuildValue("l", szTime);
}

PyObject* bsGetEndTime(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byPage;
	if (!PyTuple_GetByte(poArgs, 0, &byPage))
		return Py_BadArgument();

	long szTime = CBonusSwitcher::Instance().GetEndTime(byPage);
	return Py_BuildValue("l", szTime);
}

PyObject* bsClear(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byPage;
	if (!PyTuple_GetByte(poArgs, 0, &byPage))
		byPage = PAGE_MAX_NUM + 1;

	CBonusSwitcher::Instance().Clear(byPage);
	return Py_BuildNone();
}

PyObject* bsSetWnd(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poPhaseWnd;
	if (!PyTuple_GetObject(poArgs, 0, &poPhaseWnd))
		return Py_BuildException();
	
	CBonusSwitcher::Instance().SetPhaseWnd(poPhaseWnd);
	return Py_BuildNone();
}

PyObject* bsLoadData(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();
	
	CBonusSwitcher::Instance().LoadData(szFileName);
	return Py_BuildNone();
}

PyObject* bsSaveData(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();
	
	CBonusSwitcher::Instance().SaveData(szFileName);
	return Py_BuildNone();
}

void initBonusSwitcher()
{
	static PyMethodDef s_methods[] =
	{
		{ "AddItem", bsAddItem, METH_VARARGS },

		{ "GetDestAttr", bsGetDestAttr, METH_VARARGS },
		{ "SetDestAttr", bsSetDestAttr, METH_VARARGS },		

		{ "SetStatus", bsSetStatus, METH_VARARGS },
		{ "GetStatus", bsGetStatus, METH_VARARGS },		
	
		{ "SetStatusRare", bsSetStatusRare, METH_VARARGS },
		{ "GetStatusRare", bsGetStatusRare, METH_VARARGS },
	
		{ "GetItemIndex", bsGetItemIndex, METH_VARARGS },
		{ "GetItemPos", bsGetItemPos, METH_VARARGS },

		{ "SetSwitchDelay", bsSetSwitchDelay, METH_VARARGS },
		{ "GetSwitchDelay", bsGetSwitchDelay, METH_VARARGS },

		{ "GetChangesUsed", bsGetChangesUsed, METH_VARARGS },

#ifdef ENABLE_SWITCH_ITEM_SELECT
		{ "SetSwitchItem", bsSetSwitchItem, METH_VARARGS },
		{ "GetSwitchItem", bsGetSwitchItem, METH_VARARGS },
#endif

		{ "GetStartTime", bsGetStartTime, METH_VARARGS },		
	
		{ "GetEndTime", bsGetEndTime, METH_VARARGS },

		{ "Clear", bsClear, METH_VARARGS },
		{ "SetWnd", bsSetWnd, METH_VARARGS },

		{ "LoadData", bsLoadData, METH_VARARGS },
		{ "SaveData", bsSaveData, METH_VARARGS },

		{ nullptr, nullptr, 0 },
	};

	PyObject * poModule = Py_InitModule("bs", s_methods);


#ifdef ENABLE_SWITCH_ITEM_SELECT
	PyModule_AddIntConstant(poModule, "ENABLE_SWITCH_ITEM_SELECT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SWITCH_ITEM_SELECT", 0);
#endif

	PyModule_AddIntConstant(poModule, "PAGE_MAX_NUM", PAGE_MAX_NUM);
	PyModule_AddIntConstant(poModule, "ALT_MAX_NUM", ALT_MAX_NUM);

	PyModule_AddIntConstant(poModule, "STATUS_EMPTY", STATUS_EMPTY);
	PyModule_AddIntConstant(poModule, "STATUS_ACTIVE", STATUS_ACTIVE);
	PyModule_AddIntConstant(poModule, "STATUS_INACTIVE", STATUS_INACTIVE);
	PyModule_AddIntConstant(poModule, "STATUS_DONE", STATUS_DONE);

	PyModule_AddIntConstant(poModule, "SPEED_SLOW", SPEED_SLOW);
	PyModule_AddIntConstant(poModule, "SPEED_NORMAL", SPEED_NORMAL);
	PyModule_AddIntConstant(poModule, "SPEED_FAST", SPEED_FAST);
	PyModule_AddIntConstant(poModule, "SPEED_PREMIUM", SPEED_PREMIUM);	
	
	PyModule_AddIntConstant(poModule, "ERROR_WRONG_PAGE", ERROR_WRONG_PAGE);
	PyModule_AddIntConstant(poModule, "ERROR_ITEM_ALREADY_ADDED", ERROR_ITEM_ALREADY_ADDED);
	PyModule_AddIntConstant(poModule, "ERROR_ITEM_NOT_FOUND", ERROR_ITEM_NOT_FOUND);
	PyModule_AddIntConstant(poModule, "ERROR_ITEM_DATA_NOT_FOUND", ERROR_ITEM_DATA_NOT_FOUND);
	PyModule_AddIntConstant(poModule, "ERROR_BAD_ITEM_TYPE", ERROR_BAD_ITEM_TYPE);
	PyModule_AddIntConstant(poModule, "ERROR_OK", ERROR_OK);
}

#endif