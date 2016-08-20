#include <ButtonConstants.au3>
#include <ComboConstants.au3>
#include <EditConstants.au3>
#include <GUIConstantsEx.au3>
#include <StaticConstants.au3>
#include <WindowsConstants.au3>
#Region ### START Koda GUI section ### Form=C:\Users\����\Desktop\koda_1.7.3.0\Forms\Form1.kxf
$Form1 = GUICreate("������Ű MDict ���� by ����", 615, 286, 192, 124)
$Input1 = GUICtrlCreateInput("", 136, 40, 361, 21)
$Label1 = GUICtrlCreateLabel("������Ű JSON ����", 8, 40, 104, 17)
$Label2 = GUICtrlCreateLabel("HTML ���� ������", 8, 80, 106, 17)
$Input2 = GUICtrlCreateInput("", 136, 80, 361, 21)
$Label3 = GUICtrlCreateLabel("MDict ���� ������", 8, 120, 104, 17)
$Input3 = GUICtrlCreateInput("", 136, 120, 361, 21)
$Combo1 = GUICtrlCreateCombo("�����ϼ���", 136, 176, 145, 25, BitOR($CBS_DROPDOWN,$CBS_AUTOHSCROLL))
GUICtrlSetData(-1, "1(����)|2|4|8|16")
$Label4 = GUICtrlCreateLabel("������ ����", 8, 176, 62, 17)
$start = GUICtrlCreateButton("����", 136, 240, 75, 25)
$help = GUICtrlCreateButton("����", 408, 240, 75, 25)
$JsonDir = GUICtrlCreateButton("ã��", 528, 40, 75, 25)
$HtmlDir = GUICtrlCreateButton("ã��", 528, 80, 75, 25)
$MdxDir = GUICtrlCreateButton("ã��", 528, 120, 75, 25)
GUISetState(@SW_SHOW)
#EndRegion ### END Koda GUI section ###

While 1
	$nMsg = GUIGetMsg()
	Switch $nMsg
		 Case $JsonDir
			GUICtrlSetData ($Input1 , FileOpenDialog ("ã��", ".\", "JSON (*.json)") )
		 Case $HtmlDir
			GUICtrlSetData ($Input2 , FileOpenDialog ("ã��", ".\", "Mdict HTML (*.txt)") )
		 Case $MdxDir
			GUICtrlSetData ($Input3 , FileOpenDialog ("ã��", ".\", "Mdict File (*.mdx)") )
		 Case $start
			Local $Json = GUICtrlRead ($Input1)
			Local $Html = GUICtrlRead ($Input2)
			Local $Mdx = GUICtrlRead ($Input3)
			Run (".\namuwiki.exe", )
		 Case $GUI_EVENT_CLOSE
			Exit

	EndSwitch
WEnd
