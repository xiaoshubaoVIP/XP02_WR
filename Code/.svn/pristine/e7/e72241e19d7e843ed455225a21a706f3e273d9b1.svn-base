macro MultiLineComment()
{
hwnd = GetCurrentWnd()
selection = GetWndSel(hwnd)
LnFirst =GetWndSelLnFirst(hwnd)
LnLast =GetWndSelLnLast(hwnd)
hbuf = GetCurrentBuf()

if(GetBufLine(hbuf, 0) =="//magic-number:tph85666031"){
stop
}

Ln = Lnfirst
buf = GetBufLine(hbuf, Ln)
len = strlen(buf)

while(Ln <= Lnlast) {
buf = GetBufLine(hbuf, Ln)
if(buf ==""){
Ln = Ln + 1
continue
}

if(StrMid(buf, 0, 1) == "/"){
if(StrMid(buf, 1, 2) == "/"){
PutBufLine(hbuf, Ln, StrMid(buf, 2, Strlen(buf)))
}
}

if(StrMid(buf,0,1) !="/"){
PutBufLine(hbuf, Ln, Cat("//", buf))
}
Ln = Ln + 1
}

SetWndSel(hwnd, selection)
}

macro DefMacroComment()

{

    hwnd=GetCurrentWnd()

    sel=GetWndSel(hwnd)

    lnFirst=GetWndSelLnFirst(hwnd)

    lnLast=GetWndSelLnLast(hwnd)

    hbuf=GetCurrentBuf()

 

    if (LnFirst == 0) {

            szIfStart = ""

    } else {

            szIfStart = GetBufLine(hbuf, LnFirst-1)

    }

    szIfEnd = GetBufLine(hbuf, lnLast+1)

    if (szIfStart == "#if 0" && szIfEnd =="#endif") {

            DelBufLine(hbuf, lnLast+1)

            DelBufLine(hbuf, lnFirst-1)

            sel.lnFirst = sel.lnFirst 每 1

            sel.lnLast = sel.lnLast 每 1

    } else {

            InsBufLine(hbuf, lnFirst, "#if 0")

            InsBufLine(hbuf, lnLast+2, "#endif")

            sel.lnFirst = sel.lnFirst + 1

            sel.lnLast = sel.lnLast + 1

    }

 

    SetWndSel( hwnd, sel )

}

macro stMacroComment()
{

    hwnd=GetCurrentWnd()

    sel=GetWndSel(hwnd)

    lnFirst=GetWndSelLnFirst(hwnd)

    lnLast=GetWndSelLnLast(hwnd)

    hbuf=GetCurrentBuf()

 

    if (LnFirst == 0) {

            szIfStart = ""

    } else {

            szIfStart = GetBufLine(hbuf, LnFirst-1)

    }

    szIfEnd = GetBufLine(hbuf, lnLast+1)

    if (szIfStart == "typedef struct __$name${" && szIfEnd =="}$name$;") {

            DelBufLine(hbuf, lnLast+1)

            DelBufLine(hbuf, lnFirst-1)

            sel.lnFirst = sel.lnFirst 每 1

            sel.lnLast = sel.lnLast 每 1

    } else {

            InsBufLine(hbuf, lnFirst, "typedef struct $name$ $name$Tag;")

            InsBufLine(hbuf, lnFirst+1, "struct $name${")

            InsBufLine(hbuf, lnLast+2, "};")

            sel.lnFirst = sel.lnFirst + 1

            sel.lnLast = sel.lnLast + 2

    }

 

    SetWndSel( hwnd, sel )

}


macro DisplayDate()
{
	szTime = GetSysTime(1)
	Day = szTime.Day
	Month = szTime.Month
	Year = szTime.Year
	Hour = szTime.Hour
	Minute = szTime.Minute
	Second = szTime.Second

	if (Day < 10)
	szDay = "0@Day@"
	else
	szDay = Day

	szMonth = Month
	hbuf=GetCurrentBuf()
	SetBufSelText(hbuf,"@Year@/@szMonth@/@szDay@,@Hour@:@Minute@:@Second@")
}

macro GetFileName(pathName)
{
	nlength = strlen(pathName)
	i = nlength - 1
	name = ""
	while (i + 1)
	{
	ch = pathName[i]
	if ("//" == "@ch@")
	break
	i = i - 1
	}
	i = i + 1
	while (i < nlength)
	{
	name = cat(name, pathName[i])
	i = i + 1
	}
	return name
}

macro AnnotationMacroComment()
{

    hwnd=GetCurrentWnd()

    sel=GetWndSel(hwnd)

    lnFirst=GetWndSelLnFirst(hwnd)

    lnLast=GetWndSelLnLast(hwnd)

    hbuf=GetCurrentBuf()

 	szTime = GetSysTime(1)
	Day = szTime.Day
	Month = szTime.Month
	Year = szTime.Year
	Hour = szTime.Hour
	Minute = szTime.Minute
	Second = szTime.Second

	if (Day < 10)
	szDay = "0@Day@"
	else
	szDay = Day

	szMonth = Month

	szpathName=GetBufName(hBuf)

	szfileName=GetFileName(szpathName)

	hbuf=GetCurrentBuf()
	
    if (LnFirst == 0) {

            szIfStart = ""

    } else {

            szIfStart = GetBufLine(hbuf, LnFirst-1)

    }

    szIfEnd = GetBufLine(hbuf, lnLast+1)

    if (szIfStart == "/**" && szIfEnd =="**/") {

            DelBufLine(hbuf, lnLast+1)

            DelBufLine(hbuf, lnFirst-1)

            sel.lnFirst = sel.lnFirst 每 1

            sel.lnLast = sel.lnLast 每 1

    } else {

            InsBufLine(hbuf, lnFirst,	"/****************************************************************************/")
            
			InsBufLine(hbuf, lnFirst+1,	"/*	File   :             @szfileName@               */")
	
            InsBufLine(hbuf, lnFirst+2,	"/*	Author :                       songjiang                                */")

            InsBufLine(hbuf, lnFirst+3,	"/*	Version:                          V1.0                                  */")

            InsBufLine(hbuf, lnFirst+4,	"/*	Date   :                    @Year@/@szMonth@/@szDay@,@Hour@:@Minute@:@Second@                         */")


            InsBufLine(hbuf, lnFirst+5,	"/********************************Modify History******************************/")
            
            InsBufLine(hbuf, lnFirst+6,	"/*   yy/mm/dd     F. Lastname    Description of Modification                */")

            InsBufLine(hbuf, lnFirst+7,	"/****************************************************************************/")
            
            InsBufLine(hbuf, lnFirst+8,	"/*	Attention:                                                              */")
            
            InsBufLine(hbuf, lnFirst+9,	"/*	CopyRight:                CopyRight 2017--2025                          */")

            InsBufLine(hbuf, lnFirst+10,"/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */")

            InsBufLine(hbuf, lnFirst+11,"/*	Project :                      SC01                           */")

            InsBufLine(hbuf, lnFirst+12,"/****************************************************************************/")

            sel.lnFirst = sel.lnFirst + 12

            sel.lnLast = sel.lnLast + 12

    }

    SetWndSel( hwnd, sel )

}


macro FunctionAnnotationMacroComment()
{

    hwnd=GetCurrentWnd()

    sel=GetWndSel(hwnd)

    lnFirst=GetWndSelLnFirst(hwnd)

    lnLast=GetWndSelLnLast(hwnd)

    hbuf=GetCurrentBuf()

 

    if (LnFirst == 0) {

            szIfStart = ""

    } else {

            szIfStart = GetBufLine(hbuf, LnFirst-1)

    }

    szIfEnd = GetBufLine(hbuf, lnLast+1)

    if (szIfStart == "/**" && szIfEnd =="**/") {

            DelBufLine(hbuf, lnLast+1)

            DelBufLine(hbuf, lnFirst-1)

            sel.lnFirst = sel.lnFirst 每 1

            sel.lnLast = sel.lnLast 每 1

    } else {

            InsBufLine(hbuf, lnFirst, "/******************************************************************************")

            InsBufLine(hbuf, lnFirst+1, "**Description:																		")

            InsBufLine(hbuf, lnFirst+2, "** Parameters:                        												")
           
            InsBufLine(hbuf, lnFirst+3, "**     Return:   																	")

			InsBufLine(hbuf, lnFirst+4, "*******************************************************************************/")
			
            sel.lnFirst = sel.lnFirst + 4

            sel.lnLast = sel.lnLast + 4

    }

    SetWndSel( hwnd, sel )

}

