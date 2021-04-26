
static PT_DispOpr g_ptDispOprHead;


int RegisterDispOpr (PT_DispOpr ptDispOpr)
{
	PT_DispOpr ptTmp;

	if (!g_ptDispOprHead)            // when the list is NULL
	{
		g_ptDispOprHead = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptDispOprHead;
		while(ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}
	return 0;
}

