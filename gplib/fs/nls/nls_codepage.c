#include "gplib.h"

INT16U ChangeCodePage(INT16U wCodePage)
{
	INT16S	ret = -1;
	
	gUnicodePage = wCodePage;
	
	switch (wCodePage)
	{
		case UNI_ENGLISH:
			ret = ChangeUnitab((struct nls_table *)&nls_ascii_table);
			break;
		#if 0
		case UNI_ARABIC:
			ret = ChangeUnitab((struct nls_table *)&nls_arabic_table);
			break;
		case UNI_GBK:
			ret = ChangeUnitab((struct nls_table *)&nls_cp936_table);
			break;
		case UNI_BIG5:
			ret = ChangeUnitab((struct nls_table *)&nls_cp950_table);
			break;
		case UNI_SJIS:
			ret = ChangeUnitab((struct nls_table *)&nls_cp932_table);
			break;
		case UNI_CP1252:
			ret = ChangeUnitab((struct nls_table *)&nls_cp1252_table);
			break;
		#endif
		
		case UNI_UNICODE:
			ret = 0;
			break;
			
		default:
			break;
	}
	
	if (ret != 0)
	{	
		gUnicodePage = UNI_ENGLISH;
		ChangeUnitab((struct nls_table *)&nls_ascii_table);
	}
	
	return gUnicodePage;
}

INT16U GetCodePage(void)
{
	return gUnicodePage;
}
