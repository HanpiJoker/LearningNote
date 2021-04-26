static PT_FontOpr g_ptFontOprHead;


int RegisterFontOpr(PT_FontOpr ptFontOpr)
{
	PT_FontOpr ptTmp;

	if (!g_ptFontOprHead)            // when the list is NULL
	{
		g_ptFontOprHead = ptFontOpr;
		ptFontOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptFontOprHead;
		while(ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext = ptFontOpr;
		ptFontOpr->ptNext = NULL;
	}
	return 0;
}

int FontsInit(void)
{
	int iError;

	iError = FreeTypeInit ();
	if (iError)
	{
		DBG_PRINT("FreeTypeInit error!\n");
		return -1;
	}
}
