#if defined _inc_dialogs_
	#undef _inc_dialogs_
#endif


DialogCreate:LOGIN_DIALOG(playerid)
{
	Dialog_Open(playerid, Dialog:LOGIN_DIALOG, DIALOG_STYLE_PASSWORD, "LOGIN_TITLE", "LOGIN_CAPTION", "LOGIN_YES", "CANCEL");
}


DialogResponse:LOGIN_DIALOG(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Kick(playerid);

	new prefix[161];
	new hash[65];

	strcpy(prefix, PASS_PREFIX);
	strcat(prefix, inputtext);
	SHA256_PassHash(prefix, playerSalt[playerid], hash, sizeof hash);

	if(!strcmp(hash, playerPass[playerid]))
	{
		call OnPlayerAccountLogin(playerid, false);
	} 
	else 
	{
		Lang_SendText(playerid, "INCORRECT_PASSWORD");
		Dialog_Show(playerid, Dialog:LOGIN_DIALOG);		
	}
	return 1;
}



DialogCreate:REGISTER_DIALOG_1(playerid)
{
	Dialog_Open(playerid, Dialog:REGISTER_DIALOG_1, DIALOG_STYLE_INPUT, "REGISTER_TITLE", "REGISTER_CAPTION_1", "REGISTER_YES_1", "CANCEL");
}


DialogResponse:REGISTER_DIALOG_1(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Kick(playerid);

	if(NOT_IN_RANGE(strlen(inputtext), MIN_PLAYER_PASS, MAX_PLAYER_PASS))
	{
		Lang_SendText(playerid, "PASSWORD_MUST_CONTAIN", MIN_PLAYER_PASS, MAX_PLAYER_PASS);
		Dialog_Show(playerid, Dialog:REGISTER_DIALOG_1);	
		return 1;
	}

	for(new i; inputtext[i] != EOS; ++i)
	{
		if(47 < inputtext[i] < 58 || 64 < inputtext[i] < 91 || 96 < inputtext[i] < 123) 
		{
			strcat(playerTmpPass[playerid], inputtext, sizeof playerTmpPass[]);
			Dialog_Show(playerid, Dialog:REGISTER_DIALOG_2);
			return 1;
		}	
	}

	Lang_SendText(playerid, "PASSWORD_MUST_CONTAIN_2");
	Dialog_Show(playerid, Dialog:REGISTER_DIALOG_1);
	return 1;
}	


DialogCreate:REGISTER_DIALOG_2(playerid)
{
	Dialog_Open(playerid, Dialog:REGISTER_DIALOG_2, DIALOG_STYLE_INPUT, "REGISTER_TITLE", "REGISTER_CAPTION_2", "REGISTER_YES_2", "CANCEL");
}


DialogResponse:REGISTER_DIALOG_2(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Kick(playerid);

	if(!strcmp(inputtext, playerTmpPass[playerid], true))
	{
		new string[239];
		new salt[33];
		new hash[65];
		new DBResult:result;

		RandomString(salt, 31);

		strcat(string, PASS_PREFIX);
		strcat(string, inputtext);

		SHA256_PassHash(string, salt, hash, sizeof hash);

		format(string, sizeof string, "INSERT INTO auth (name, pass, salt) VALUES ('%s', '%q', '%q'); SELECT last_insert_rowid()", playerName[playerid], hash, salt);
		result = db_query(DB, string);

		if(db_num_rows(result))
		{
			playerID[playerid] = db_get_field_int(result);
		}
		else 
		{
			db_free_result(result);
			SendClientMessage(playerid, -1, "Bug occurred.");
			defer _Kick(playerid);
			return 1;
		}	

		db_free_result(result);
		call OnPlayerAccountLogin(playerid, true);
	}
	else 
	{
		Lang_SendText(playerid, "EQUAL_PASSWORD");
		Dialog_Show(playerid, Dialog:REGISTER_DIALOG_2);				
	}
	return 1;
}			