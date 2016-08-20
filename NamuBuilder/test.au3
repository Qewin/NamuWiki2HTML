#include <ButtonConstants.au3>
#include <ComboConstants.au3>
#include <EditConstants.au3>
#include <GUIConstantsEx.au3>
#include <StaticConstants.au3>
#include <WindowsConstants.au3>
#Region ### START Koda GUI section ### Form=C:\Users\용하\Desktop\koda_1.7.3.0\Forms\Form1.kxf
$Form1 = GUICreate("나무위키 MDict 빌더 by 퀘윈", 615, 286, 192, 124)
$Input1 = GUICtrlCreateInput("", 136, 40, 361, 21)
$Label1 = GUICtrlCreateLabel("나무위키 JSON 파일", 8, 40, 104, 17)
$Label2 = GUICtrlCreateLabel("HTML 파일 저장경로", 8, 80, 106, 17)
$Input2 = GUICtrlCreateInput("", 136, 80, 361, 21)
$Label3 = GUICtrlCreateLabel("MDict 파일 저장경로", 8, 120, 104, 17)
$Input3 = GUICtrlCreateInput("", 136, 120, 361, 21)
$Combo1 = GUICtrlCreateCombo("설정하세요", 136, 176, 145, 25, BitOR($CBS_DROPDOWN,$CBS_AUTOHSCROLL))
GUICtrlSetData(-1, "1(권장)|2|4|8|16")
$Label4 = GUICtrlCreateLabel("스레드 개수", 8, 176, 62, 17)
$start = GUICtrlCreateButton("시작", 136, 240, 75, 25)
$help = GUICtrlCreateButton("도움말", 408, 240, 75, 25)
$JsonDir = GUICtrlCreateButton("찾기", 528, 40, 75, 25)
$HtmlDir = GUICtrlCreateButton("찾기", 528, 80, 75, 25)
$MdxDir = GUICtrlCreateButton("찾기", 528, 120, 75, 25)
GUISetState(@SW_SHOW)
#EndRegion ### END Koda GUI section ###

While 1
	$nMsg = GUIGetMsg()
	Switch $nMsg
		 Case $JsonDir
			GUICtrlSetData ($Input1 , FileOpenDialog ("찾기", ".\", "JSON (*.json)") )
		 Case $HtmlDir
			GUICtrlSetData ($Input2 , FileOpenDialog ("찾기", ".\", "Mdict HTML (*.txt)") )
		 Case $MdxDir
			GUICtrlSetData ($Input3 , FileOpenDialog ("찾기", ".\", "Mdict File (*.mdx)") )
		 Case $start
			Local $Json = GUICtrlRead ($Input1)
			Local $Html = GUICtrlRead ($Input2)
			Local $Mdx = GUICtrlRead ($Input3)
			Run (".\namuwiki.exe", )
		 Case $GUI_EVENT_CLOSE
			Exit

	EndSwitch
WEnd
