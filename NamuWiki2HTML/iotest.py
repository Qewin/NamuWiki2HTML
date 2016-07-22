# -*- coding: UTF-8 -*-
#---------주의사항----------
#파일 경로가 변환을 위해서만 만들어서 직접 소스에서 수정해야 사용 가능합니다.
#현재 버전의 (11월 2일, 11월 30일 나무위키) 정도만 작동 보증합니다.
#버그가 여러 개 있습니다.
#코드가 엉망입니다.
#최적화를 안 해서 속도가 느리니 Pypy로 실행해 주세요.
#mdxbuilder에서 compactHTML/UTF-8 로 설정해서 변환하세요.

##나무위키에 올라오는 데이터베이스 덤프 파일을 MDICT에 사용할 수 있도록 mdxbuilder에 사용 가능한 파일로 변환하는 프로그램(JSON 버전)
##Copyright (C) 2015~2016 <Qewin> 
##이 프로그램은 자유 소프트웨어입니다. 소프트웨어 피이용허락자는 자유 소프트웨어 재단이 공표한 GNU GPL 2판 또는 그 이후 판을 임의로 선택해, 그 규정에 따라 프로그램을 개작하거나 재배포할 수 있습니다. 
##이 프로그램은 유용하게 사용되리라는 희망으로 배포되지만, 특정한 목적에 맞는 적합성 여부나 판매용으로 사용할 수 있다는 묵시적인 보증을 포함한 어떠한 형태의 보증도 제공하지 않습니다. 보다 자세한 사항은 GNU GPL을 참고하시기 바랍니다. 
##GNU GPL은 이 프로그램과 함께 제공됩니다. 만약 이 문서가 누락되어 있다면 자유 소프트웨어 재단으로 문의하시기 바랍니다(자유 소프트웨어 재단: Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA). 



import codecs
import time
infile = open("E:/programs/programming/NamuV2/namuwiki_20160530.json", 'r') #JSON 경로

print("Reading Cache")
t= time.time()
line = infile.read(150000000)
print("%d"%(t - time.time()))
a = input()


infile.close()
