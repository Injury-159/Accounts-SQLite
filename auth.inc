

#include <YSI_Coding\y_hooks>


hook OnPlayerFullyConnected(playerid)
{
	playerID[playerid] = 0;
	playerPass[playerid][0] =
	playerSalt[playerid][0] =	
	playerTmpPass[playerid][0] = EOS;

	playerKicked{playerid} = false;

	ClearPlayerChat(playerid);

	GetPlayerName(playerid, playerName[playerid], sizeof playerName[]);

	defer _FetchPlayer(playerid);
}


hook OnPlayerDisconnect(playerid, reason)
{
	playerID[playerid] = 0;
	playerLevel{playerid} = 0;

	playerName[playerid][0] =
	playerPass[playerid][0] =
	playerSalt[playerid][0] = EOS;

	playerKicked{playerid} = false;

	if(Iter_Contains(Logged, playerid))
	{
		call OnPlayerRequestQuit(playerid);
		Iter_Remove(Logged, playerid);
	}	
}


hook OnPlayerAccountLogin(playerid, bool:register)
{
	Iter_Add(Logged, playerid);

	if(register)
	{
		new string[47];
		format(string, sizeof string, "INSERT INTO players (uid) VALUES (%d)", playerID[playerid]);
		db_query(DB, string);
	}	
	else 
	{
		new string[46];
		new DBResult:result;

		format(string, sizeof string, "SELECT * FROM players WHERE uid = %d", playerID[playerid]);
		if((db_num_rows((result = db_query(DB, string)))))
		{
			SetPlayerLevel(playerid, db_get_field_assoc_int(result, "level"));
		}
		else 
		{
			db_free_result(result);
			SendClientMessage(playerid, -1, "An error occoured in your account.");
			defer _Kick(playerid);
			return;
		}
		db_free_result(result);
	}
	

	SetSpawnInfo(playerid, NO_TEAM, 154, 0.0, 0.0, 3.0, 0.0, 0, 0, 0, 0, 0, 0);
	SpawnPlayer(playerid);
}


stock IsPlayerLogged(playerid)
{
	return Iter_Contains(Logged, playerid);
}


stock GetPlayerAccount(playerid)
{
	return playerID[playerid];
}


stock SetPlayerLevel(playerid, level)
{
	playerLevel{playerid} = clamp(level, 0, 5);
}


stock GetPlayerLevel(playerid)
{
	return playerLevel{playerid};
}


SetPlayerKickStatus(playerid, bool:status)
{
	playerKicked{playerid} = status;
}


static timer _FetchPlayer[200](playerid)
{
	if(!(/*playerBanCheck(playerid) ||*/ playerKicked{playerid}))	
	{

		// Configs
		Streamer_ToggleIdleUpdate(playerid, true);


		new string[223];
		format(string, sizeof string, "SELECT userid, pass, salt, lang FROM auth WHERE name = '%q'", playerName[playerid]);	
		new DBResult:result = db_query(DB, string);

		if(db_num_rows(result))
		{
			playerID[playerid] = db_get_field_assoc_int(result, "userid");
			db_get_field_assoc(result, "pass", playerPass[playerid], 65);
			db_get_field_assoc(result, "salt", playerSalt[playerid], 65);
			Lang_SetPlayerLang(playerid, Lang:db_get_field_assoc_int(result, "lang"));

			Dialog_Show(playerid, Dialog:LOGIN_DIALOG);
		}
		else 
		{
			Dialog_Show(playerid, Dialog:REGISTER_DIALOG_1);
		}

		db_free_result(result);
	}
}
