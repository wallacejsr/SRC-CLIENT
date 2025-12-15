#include "StdAfx.h"
#include "PythonNonPlayer.h"

#include "InstanceBase.h"
#include "PythonCharacterManager.h"

PyObject * nonplayerGetEventType(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualNumber))
		return Py_BuildException();

	BYTE iType = CPythonNonPlayer::Instance().GetEventType(iVirtualNumber);

	return Py_BuildValue("i", iType);
}

PyObject * nonplayerGetEventTypeByVID(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	BYTE iType = CPythonNonPlayer::Instance().GetEventTypeByVID(iVirtualID);

	return Py_BuildValue("i", iType);
}

PyObject * nonplayerGetLevelByVID(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
		return Py_BuildValue("i", -1);

	const CPythonNonPlayer::TMobTable * pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	float fAverageLevel = pMobTable->bLevel;//(float(pMobTable->abLevelRange[0]) + float(pMobTable->abLevelRange[1])) / 2.0f;
	fAverageLevel = floor(fAverageLevel + 0.5f);
	return Py_BuildValue("i", int(fAverageLevel));
}

PyObject * nonplayerGetGradeByVID(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
		return Py_BuildValue("i", -1);

	const CPythonNonPlayer::TMobTable * pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	return Py_BuildValue("i", pMobTable->bRank);
}

PyObject * nonplayerGetMonsterName(PyObject * poSelf, PyObject * poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();
	return Py_BuildValue("s", rkNonPlayer.GetMonsterName(iVNum));
}

#if defined(ENABLE_TELEPORT_SYSTEM) || defined(ENABLE_RESP_SYSTEM)
PyObject * nonplayerGetMonsterLevel(PyObject * poSelf, PyObject * poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();
	return Py_BuildValue("i", rkNonPlayer.GetMonsterLevel(iVNum));
}

PyObject * nonplayerGetMonsterGrade(PyObject * poSelf, PyObject * poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
		return Py_BuildException();

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable(iVNum);

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	return Py_BuildValue("i", pMobTable->bRank);
}

PyObject * nonplayerGetAttElementFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
		return Py_BuildException();

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable(iVNum);

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	return Py_BuildValue("i", pMobTable->dwRaceFlag);
}
#endif

#ifdef ENABLE_SEND_TARGET_INFO
PyObject * nonplayerGetMonsterMaxHP(PyObject * poSelf, PyObject * poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();

	return Py_BuildValue("i", rkNonPlayer.GetMonsterMaxHP(race));
}

PyObject * nonplayerGetRaceNumByVID(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
		return Py_BuildValue("i", -1);

	const CPythonNonPlayer::TMobTable * pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	return Py_BuildValue("i", pMobTable->dwVnum);
}

PyObject * nonplayerGetMonsterRaceFlag(PyObject * poSelf, PyObject * poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();

	return Py_BuildValue("i", rkNonPlayer.GetMonsterRaceFlag(race));
}

PyObject * nonplayerGetMonsterDamage(PyObject * poSelf, PyObject * poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();

	DWORD dmg1 = rkNonPlayer.GetMonsterDamage1(race);
	DWORD dmg2 = rkNonPlayer.GetMonsterDamage2(race);

	return Py_BuildValue("ii", dmg1,dmg2);
}

PyObject * nonplayerGetMonsterExp(PyObject * poSelf, PyObject * poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();

	return Py_BuildValue("i", rkNonPlayer.GetMonsterExp(race));
}

PyObject * nonplayerGetMonsterDamageMultiply(PyObject * poSelf, PyObject * poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();

	return Py_BuildValue("f", rkNonPlayer.GetMonsterDamageMultiply(race));
}

PyObject * nonplayerGetMonsterST(PyObject * poSelf, PyObject * poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();

	return Py_BuildValue("i", rkNonPlayer.GetMonsterST(race));
}

PyObject * nonplayerGetMonsterDX(PyObject * poSelf, PyObject * poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();

	return Py_BuildValue("i", rkNonPlayer.GetMonsterDX(race));
}

PyObject * nonplayerIsMonsterStone(PyObject * poSelf, PyObject * poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();

	return Py_BuildValue("i", rkNonPlayer.IsMonsterStone(race) ? 1 : 0);
}
#endif

PyObject * nonplayerLoadNonPlayerData(PyObject * poSelf, PyObject * poArgs)
{
	char * szFileName;
	if(!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CPythonNonPlayer::Instance().LoadNonPlayerData(szFileName);
	return Py_BuildNone();
}

void initNonPlayer()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetEventType",				nonplayerGetEventType,				METH_VARARGS },
		{ "GetEventTypeByVID",			nonplayerGetEventTypeByVID,			METH_VARARGS },
		{ "GetLevelByVID",				nonplayerGetLevelByVID,				METH_VARARGS },
		{ "GetGradeByVID",				nonplayerGetGradeByVID,				METH_VARARGS },
		{ "GetMonsterName",				nonplayerGetMonsterName,			METH_VARARGS },
#ifdef ENABLE_SEND_TARGET_INFO
		// TARGET_INFO
		{ "GetRaceNumByVID",			nonplayerGetRaceNumByVID,			METH_VARARGS },
		{ "GetMonsterMaxHP",			nonplayerGetMonsterMaxHP,			METH_VARARGS },
		{ "GetMonsterRaceFlag",			nonplayerGetMonsterRaceFlag,		METH_VARARGS },
		{ "GetMonsterLevel",			nonplayerGetMonsterLevel,			METH_VARARGS },
		{ "GetMonsterDamage",			nonplayerGetMonsterDamage,			METH_VARARGS },
		{ "GetMonsterExp",				nonplayerGetMonsterExp,				METH_VARARGS },
		{ "GetMonsterDamageMultiply",	nonplayerGetMonsterDamageMultiply,	METH_VARARGS },
		{ "GetMonsterST",				nonplayerGetMonsterST,				METH_VARARGS },
		{ "GetMonsterDX",				nonplayerGetMonsterDX,				METH_VARARGS },
		{ "IsMonsterStone",				nonplayerIsMonsterStone,			METH_VARARGS },
#endif
#if defined(ENABLE_TELEPORT_SYSTEM) || defined(ENABLE_RESP_SYSTEM)
		{ "GetMonsterLevel",			nonplayerGetMonsterLevel,			METH_VARARGS },
		{ "GetMonsterGrade",			nonplayerGetMonsterGrade,			METH_VARARGS },
		{ "GetAttElementFlag",			nonplayerGetAttElementFlag,			METH_VARARGS },
#endif

		{ "LoadNonPlayerData",			nonplayerLoadNonPlayerData,			METH_VARARGS },

		{ NULL,							NULL,								NULL		 },
	};

	PyObject * poModule = Py_InitModule("nonplayer", s_methods);

	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_NONE",		CPythonNonPlayer::ON_CLICK_EVENT_NONE);
	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_BATTLE",		CPythonNonPlayer::ON_CLICK_EVENT_BATTLE);
	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_SHOP",		CPythonNonPlayer::ON_CLICK_EVENT_SHOP);
	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_TALK",		CPythonNonPlayer::ON_CLICK_EVENT_TALK);
	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_VEHICLE",		CPythonNonPlayer::ON_CLICK_EVENT_VEHICLE);

	PyModule_AddIntConstant(poModule, "PAWN", 0);
	PyModule_AddIntConstant(poModule, "S_PAWN", 1);
	PyModule_AddIntConstant(poModule, "KNIGHT", 2);
	PyModule_AddIntConstant(poModule, "S_KNIGHT", 3);
	PyModule_AddIntConstant(poModule, "BOSS", 4);
	PyModule_AddIntConstant(poModule, "KING", 5);

#if defined (ENABLE_ELEMENT_ADD) || defined(ENABLE_DROP_INFO) || defined(ENABLE_RESP_SYSTEM)
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ANIMAL", CPythonNonPlayer::RACE_FLAG_ANIMAL);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_UNDEAD", CPythonNonPlayer::RACE_FLAG_UNDEAD);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_DEVIL", CPythonNonPlayer::RACE_FLAG_DEVIL);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_HUMAN", CPythonNonPlayer::RACE_FLAG_HUMAN);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ORC", CPythonNonPlayer::RACE_FLAG_ORC);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_MILGYO", CPythonNonPlayer::RACE_FLAG_MILGYO);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_INSECT", CPythonNonPlayer::RACE_FLAG_INSECT);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_FIRE", CPythonNonPlayer::RACE_FLAG_FIRE);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ICE", CPythonNonPlayer::RACE_FLAG_ICE);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_DESERT", CPythonNonPlayer::RACE_FLAG_DESERT);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_TREE", CPythonNonPlayer::RACE_FLAG_TREE);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ATT_ELEC", CPythonNonPlayer::RACE_FLAG_ATT_ELEC);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ATT_FIRE", CPythonNonPlayer::RACE_FLAG_ATT_FIRE);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ATT_ICE", CPythonNonPlayer::RACE_FLAG_ATT_ICE);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ATT_WIND", CPythonNonPlayer::RACE_FLAG_ATT_WIND);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ATT_EARTH", CPythonNonPlayer::RACE_FLAG_ATT_EARTH);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ATT_DARK", CPythonNonPlayer::RACE_FLAG_ATT_DARK);

	PyModule_AddIntConstant(poModule, "IMMUNE_STUN", CPythonNonPlayer::IMMUNE_STUN);
	PyModule_AddIntConstant(poModule, "IMMUNE_SLOW", CPythonNonPlayer::IMMUNE_SLOW);
	PyModule_AddIntConstant(poModule, "IMMUNE_FALL", CPythonNonPlayer::IMMUNE_FALL);
	PyModule_AddIntConstant(poModule, "IMMUNE_CURSE", CPythonNonPlayer::IMMUNE_CURSE);
	PyModule_AddIntConstant(poModule, "IMMUNE_POISON", CPythonNonPlayer::IMMUNE_POISON);
	PyModule_AddIntConstant(poModule, "IMMUNE_TERROR", CPythonNonPlayer::IMMUNE_TERROR);
	PyModule_AddIntConstant(poModule, "IMMUNE_REFLECT", CPythonNonPlayer::IMMUNE_REFLECT);	
	
	PyModule_AddIntConstant(poModule, "MOB_RESIST_SWORD", CPythonNonPlayer::MOB_RESIST_SWORD);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_TWOHAND", CPythonNonPlayer::MOB_RESIST_TWOHAND);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_DAGGER", CPythonNonPlayer::MOB_RESIST_DAGGER);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_BELL", CPythonNonPlayer::MOB_RESIST_BELL);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_FAN", CPythonNonPlayer::MOB_RESIST_FAN);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_BOW", CPythonNonPlayer::MOB_RESIST_BOW);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_FIRE", CPythonNonPlayer::MOB_RESIST_FIRE);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_ELECT", CPythonNonPlayer::MOB_RESIST_ELECT);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_MAGIC", CPythonNonPlayer::MOB_RESIST_MAGIC);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_WIND", CPythonNonPlayer::MOB_RESIST_WIND);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_POISON", CPythonNonPlayer::MOB_RESIST_POISON);
#endif
}