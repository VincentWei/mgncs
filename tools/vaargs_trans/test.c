/*
 ** $Id: test.c 927 2010-07-02 09:05:48Z dongjunjie $
 **
 ** test2.c: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: dongjunjie 
 **  
 ** Create date: 2010-07-02 
 */

if(_M(te, getParaText, (int)GetDlgItemInt(hDlg, IDC_EDIT_PARAIDX, NULL, TRUE),
            (int)GetDlgItemInt(hDlg, IDC_EDIT_PARASTART, NULL, TRUE),
            copy_len < sizeof(buff) ? copy_len : sizeof(buff)-1, buff) > 0)
{
    SetDlgItemText(hDlg, IDC_LABEL_PARATXT, buff);
}

/*_M(pobj, print, "_M(pobj, test,arg1, arg2)", arg2);*/

a = "_M(pobj, test, arg1, arg2)"

_M(pobj, print, "_M(pobj, test,arg1, arg2)", arg2);

_M(pobj, print, _M(pobj2, method2, arg1, "_M(pobj, test, arg1)", arg2));

_M(pobj, print, /* _M(pobj, test, arg1, arg2) */, arg2);
_M(pobj, print, call(/* call test */a,b/* test b */), /*test c*/c, arg3);

_M(pobj, print, //test
        arg1,   //arg1
        arg2,   //arg2
  );

	

