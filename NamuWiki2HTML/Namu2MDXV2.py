# -*- coding: UTF-8 -*-
#---------주의사항----------
#파일 경로가 변환을 위해서만 만들어서 직접 소스에서 수정해야 사용 가능합니다.
#현재 버전의 (11월 2일, 11월 30일 나무위키) 정도만 작동 보증합니다.
#버그가 여러 개 있습니다.
#코드가 엉망입니다.
#최적화를 안 해서 속도가 느리니 Pypy로 실행해 주세요.
#mdxbuilder에서 compactHTML/UTF-8 로 설정해서 변환하세요.

##나무위키에 올라오는 데이터베이스 덤프 파일을 MDICT에 사용할 수 있도록 mdxbuilder에 사용 가능한 파일로 변환하는 프로그램
##Copyright (C) 2015년 <Qewin> 
##이 프로그램은 자유 소프트웨어입니다. 소프트웨어 피이용허락자는 자유 소프트웨어 재단이 공표한 GNU GPL 2판 또는 그 이후 판을 임의로 선택해, 그 규정에 따라 프로그램을 개작하거나 재배포할 수 있습니다. 
##이 프로그램은 유용하게 사용되리라는 희망으로 배포되지만, 특정한 목적에 맞는 적합성 여부나 판매용으로 사용할 수 있다는 묵시적인 보증을 포함한 어떠한 형태의 보증도 제공하지 않습니다. 보다 자세한 사항은 GNU GPL을 참고하시기 바랍니다. 
##GNU GPL은 이 프로그램과 함께 제공됩니다. 만약 이 문서가 누락되어 있다면 자유 소프트웨어 재단으로 문의하시기 바랍니다(자유 소프트웨어 재단: Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA). 



import codecs
import time
#import pdb
infile = codecs.open("E:/programs/programming/NamuV2/namuwiki_20160530.json", 'r', 'utf-8') #JSON 경로
outfile = codecs.open("namu.txt", 'w', 'utf-8') #출력 파일 경로
errfile = codecs.open("err.txt", 'w', 'utf-8') #에러 파일 경로
count = -1 #라인 수(-1인 이유는 \r\n때문. 아래서 0으로 수정됨) 
i = 0
full = 0 #다 읽었는지 여부
read = 0
linecache = [list(),list(),list(),list(),list()] # 문자열 캐시
#linecache[0]:본
#linecache[1]:주석
#linecache[2]:목차
#linecache[3]:temp
#linecache[4]:표
exectime = time.time() # 시작시간
FileCache = list()
x = 0       #표 용이지만 표인지 판별을 위해 글로벌
listree = -1 # 목차 리스트
linecount = 2474
TableOn = False # 테이블 안인지
#2399 인듯
#현재 간노스역이 마지막 항목
#
xcount = 1 #주석은 1부터.
index = [0,0,0,0,0,0,0,0,0,0,0]

#------------------문법 함수-------------------------

#삼중중괄호
#---------------------------
#html / 문법 무시
#---------------------------
def TripleBrace(dir,read):
    global line
    global linecache
    if line[read+3:read+9].lower() == "#!html":    #html 스킵 신공
        read +=9
        if titlecache[:2] != "틀:":
            while line [read:read+3] != "}}}":
                read += 1
            read += 3
        else:
            while line [read:read+3] != "}}}":
                if line[read] == "\"":
                    return read #문서의 끝으로 추정
                elif line[read] == "\\":
                    read += 1 # sql 문법 스킵
                linecache[dir].append(line[read])
                read += 1
            read += 3
    elif line [read+3] == '#':              # 색
        read += 4
        linecache[dir].append("<font color=\"")
        while line[read] != ' ':
            linecache[dir].append(line[read])
            read += 1
        linecache[dir].append("\">")
        read += 1                     #빈칸 건너뛰기
        read = WikiParser(dir,read,"}}}")
        linecache[dir].append("</font>")
        read += 3
    elif line[read+3] == "+":
        read += 3
        linecache[dir].append("<font size=\"%s\">"%line[read:read+2])
        read += 2
        read = WikiParser(dir,read,"}}}")
        if line[read] == "}" : read += 3
        linecache[dir].append("</font>")
    else:                                   #문법 무시
        read += 3
        if line[read:read+2] == "\\n":
            read += 2 # \n스킵
            # 여러줄 문법 무시 - {{{ 내부에 {{{이 있을 때(나무위키 실제 예 참고)
            # https://namu.wiki/w/%EB%82%98%EB%AC%B4%EC%9C%84%ED%82%A4/%EC%97%94%EC%A7%84#s-6.4
            linecache[dir].append("<pre><code>") # pre : 상자 안 문법 무시 code : 문법 무
            while line[read:read+5] != "\\n}}}": # \n}}}
                if line[read:read+2] == "\\n":   #\n 처리 (\n}}} 처리가 있지만 그게 우선권을 가져서 상관없음)
                    linecache[dir].append("<br>")
                    read += 2
                #---------------<html 에러 방지>-----------------
                elif line[read] == '<':
                    linecache[dir].append("&lt;")
                    read += 1
                elif line[read] == '>': #\\n> 과는 헤깔릴 일 없음
                    linecache[dir].append("&gt;")
                    read += 1
                elif line[read:read+2] == "  ": #다중 스페이스
                    linecache[dir].append(" &nbsp;")
                    read += 2
                elif line[read] == '&':
                    linecache[dir].append("&quot;")
                    read += 1
                else:
                    if line[read] == "\"":
                        return read #문서의 끝으로 추정
                    elif line[read] == "\\":
                        read += 1 # sql 문법 스킵
                    linecache[dir].append(line[read])
                    read += 1
            read += 5
            linecache[dir].append("</code></pre>")
        else:
            linecache[dir].append("<pre><code>") # html 문법도 무시.
            while line[read:read+3] != "}}}":
                if line[read:read+2] == "\\n":   #\n 처리
                    linecache[dir].append("<br>")
                    read += 2
                #---------------<html 에러 방지>-----------------
                elif line[read] == '<':
                    linecache[dir].append("&lt;")
                    read += 1
                elif line[read] == '>': #\\n> 과는 헤깔릴 일 없음
                    linecache[dir].append("&gt;")
                    read += 1
                elif line[read:read+2] == "  ": #다중 스페이스
                    linecache[dir].append(" &nbsp;")
                    read += 2
                elif line[read] == '&':
                    linecache[dir].append("&quot;")
                    read += 1
                else:
                    linecache[dir].append(line[read])
                    read += 1
            read += 3
            linecache[dir].append("</code></pre>") # html 문법도 무시.
    return read
#--------------------------------------------------
#--------------------------------------------------
def newTableFunc(dir,read):
    global titlecache
    #if titlecache == "동방 프로젝트":
    #    import pdb
    #    pdb.set_trace()
    global line
    global linecache
    #re, isFake 지역
    # 광역
    #inWikiTable 광역
    #tableLine, contentLine, xFull, x, y: 문서 내 광역 (문법 해석부 자체를 함수로 만들면 그 때 지역변수)
    re = read                                #얘가 표 맞는지 체크
    isFake = True                                               #표 진위 판별(아래까지.)
    while line[re] == ' ' or line[re] == '	' or line[re] == '*':   #공백이나 개행
        if line[re-2:re] == "\\n" or line[re-2:re] == ",\'":            #,'맞음(8월)
            isFake = False                                              #
        re -= 1                                                         # 
    else:                                                               #
        if line[re-2:re] == "\\n" or line[re-2:re] == ",\'":            #
            isFake = False                                              #
        re -= 1                                                         #
    if isFake == True:  # 표 아니면
        linecache[dir].append("||")     #|| 써주고
        return read+2                   #나가기
    lastscent = 0                           #표가 아닐 때를 대비해 표 html 스크립트가 있는 지점을 알려주는 포인터.
    #??

    linecache[4].append("<table border=\"1\"><tr>")
    x = 0
    xFull = 0
    while True: #표 || 해석부
        l = 0 #|| 개수 세기 위한 변수.
        #print(line[read:read+2])
        while line[read:read+2] == "||": #여기는 || 를 받아야 정상이다.
            l += 1
            read += 2                                                   #read를 건드리지 않기 위한 변수
        isVacant = True
        re = read
        while line[re:re+2] != "\\n" and line[re:re+2] !="\",":
            if line[re] != ' ' and line[re] !='	':
                #print("%s,%s"%(line[read:read+2],line[re-2:re+2]))
                isVacant = False
                break
            re += 1
        if isVacant:
            if l > 1: linecache[4].append("<td colspan=\"%d\"></td>"%(l-1))
            if line[re:re+2] =="\",":
                read = re
                linecache[4].append("</tr></table>")
                break
            elif line[re:re+2] == "\\n":
                if line[re+2:re+4] != "||":#이 자리에는 뒷줄에 ||가 없으면 표 탈출!
                    read = re
                    linecache[4].append("</tr></table>")
                    break
                else:   # 아니면 새 줄
                    read = re + 2
                    linecache[4].append("</tr><tr>") # 새 줄
                    if xFull == 0: xFull = x # 최대 칸 개수 추가
                    x = l #line 약자입니다.숫자 아니에요;;
                    continue # 처음으로 가서 줄 계산 시작해야죠!!
        else:
            x += l # 줄의 칸 개수 세기
            linecache[4].append("<td") #표 끝이 아니니 칸 시작.
            if l != 1: linecache[4].append(" colspan=\"%d\"" %l)
            #표 기교 작성
            while line[read:read+2] != "||":        #다음 칸이 나오기 전에
                if line[read] == '<':               #<찾기
                    if line[read:read+2] == "<-":#<-이면
                        try:
                            read += 2             #<-건너뛰기
                            k = 0                                       #read에 영향을 주지 않기 위한 임시 변수
                            if line[read] == '>':       #비었을 걸 대비해
                                read += 1
                                break                   
                            while line[read+k] != '>' and line[read+k] != '|':
                                                        #>가 나올때까지 (|은 문법 오류 땜)
                                k = k + 1               #임시변수 건너뛰기
                            col = int(line[read:read+k])                #칸 합치기 수
                            if xFull != 0 and col > xFull:#칸 합치기 개수가 칸보다 크면
                                l = xFull - x           #나머지 칸 개수를 다 씀
                            else:                       #아니면
                                l = col                 #그냥 제시된 개수.
                                x += l - 1 #칸 개수 세기 -1 은 이미 1개는 세었으므로
                            linecache[4].append(" colspan=\"%d\"" %l)
                                                        #colspan="l"
                            read += k + 1         #<-??>뒤로
                        except:
                            for w in range(0,6):    #>가 있으면 나가
                                if line[read+w] == '>':
                                    read += w
                                    break
                            read += 1             #> 뒤로
                    elif line[read:read+2] == "<|":#<|이면
                        if line[read:read+3] == "<||":#||<||이면
                            linecache[4].append("<")       #내용 적어주고
                            read += 1             #한칸 전진
                        else:
                            try:
                                read += 2             #<|건너뛰기
                                k = 0                                       #read에 영향을 주지 않기 위한 임시 변수
                                if line[read] == '>':       #비었을 걸 대비해
                                    read += 1
                                    break   
                                re = line.find('>',read)
                                row = int(line[read:re])                #줄 합치기 수
                                linecache[4].append(" rowspan=\"%d\"" %row)
                                read = re + 1          #<|??>뒤로
                            except:
                                while line[read] != '>':    #>가 나올때까지
                                    read += 1         #건너뛰기
                                read += 1             #> 뒤로
                    elif line[read:read+6] == "<align" or line[read:read+6] == "<bgcol" or line[read:read+6] == "<width":
                        read += 1
                        re = line.find('>',read)
                        linecache[4].append(" %s" %line[read:re])
                        read = re + 1
                    elif line[read:read+2] == "<#":
                        read += 1
                        linecache[4].append(" bgcolor=")
                        re = line.find('>',read)
                        linecache[4].append(line[read:re])
                        read = re + 1
                    else:                           #나머지는 일단 나중에
                        while line[read] != '>' and line[read+1:read+3] != "||":    #>가 나올때까지
                            read += 1         #건너뛰기
                        read += 1             #> 뒤로
                elif line[read:read+2] == " <": read += 1 #뛰어쓰기 감지
                else: break                           #<가 더이상 없으면 이제 밖으로.(나중에 ||만나면 새칸으로 다시 들어옴.)
            bwk = len(linecache[4])#append 하기 전에 넣어서 - 1 할 필요 없
            linecache[4].append(">")#123
            read = WikiParser(4,read,"||")
            if line[read] == "\"":#wikiparse->')체크
                linecache[4][bwk] = "></td></tr></table>" #위의 123 부분에 덮어씌움
                break
            else: linecache[4].append("</td>")
    linecache[dir].append("".join(linecache[4]))
    linecache[4] = list()
    return read

#--------------------------------------------------
def indexFunc (dir,read,index):
    #if titlecache == "김지현":
    #    import pdb
    #    pdb.set_trace()
    global line
    global linecache
    k = read
    #= (목차) =
    #^ 에서 시작
    if line[read-2:read] != "\\n" and line[read-2:read] != ",\'" :#and line[read-3:read-1] != "\\n":
        linecache[dir].append("=")
        return read + 1,index
    #목차가 아니면 걍 패스
    where = 0 #목차 번호 찾기.
    read += 1
    while line[read] == "=":
        read += 1
        where = where + 1
    #목차 번호 찾기
    if where > 10 or line[read] != " ":  # 리스트 허용 범위 벗어나면 표 아님.
        linecache[dir].append("="*(where + 1)) 
        return read,index
    for i in range(where+1,7): index[i] = 0
    #아랫번호는 필요없으니 없앰.
    index[where] += 1
    #목차 번호 올림.
    i = 0
    while i <= where:
        if index[i] != 0:
            linecache[3].append("%d."%index[i])
        i = i + 1
    indlist = "".join(linecache[3])
    linecache[3] = list()
    linecache[dir].append("<a name=\"s-%s\"><font color=\"blue\" size=\"5\">" %(indlist[:-1]))
    linecache[2].append("%s<a href=\"entry://#s-%s\">" %(("&nbsp"*where),(indlist[:-1])))#   1.2.3.
    linecache[dir].append(indlist)
    linecache[2].append(indlist)
    linecache[dir].append("</font></a>")
    linecache[2].append("</a>")
    read = WikiParser(3,read," =")
    if line[read:read+2] == " =": 
        read += 1
        while line[read] == "=":
            read += 1
    linecache[dir].append("".join(linecache[3]))
    linecache[2].append("".join(linecache[3]))
    linecache[3] = list()
    linecache[dir].append("<br><hr>")
    linecache[2].append("<br>")
    return read,index
#--------------------------------------------------
# [* sdfsdfsrefe/내부문법[[이렇게 겹치기도.]]/]
#
def SqBracket(dir,read):
    global line
    global linecache
    if line[read:read+4] == "[[[[": #[[[[[ 처리 -> [[ 보다 앞섬
        while line[read] == "[":
            linecache[dir].append("[")
            read += 1
        return read
    temp1 = False
    temp2 = False
    read += 2
    k = 0
    if line[read:read+7] == "http://" or line[read:read+8] == "https://" :
        temp2 = True
    while line[read+k:read+2+k] != "]]" and line[read+k:read+2+k] != "||": #||는 문법 오류 해결용
        if line[read+k] == '|':
            temp1 = True
        #1 : | 들어감 0: | 안들어감 3: | 들어감 http ([외]항목) 2: | 안들어감 http (스킵)
        k = k + 1
    if temp1 == True and temp2 == False: #1
        linecache[dir].append("<a href=\"entry://")
        if line[read] == "/" and line[read+1] != "|": #앞에 제목 안붙
            linecache[dir].append(titlecache)
        while line[read] != '|':
            if line[read] == "\\": read += 1
            linecache[dir].append(line[read])
            read += 1
        read += 1
        linecache[dir].append("\">")
        read = WikiParser(dir,read,"]]")
        linecache[dir].append("</a>")
        if line[read:read+2] == "]]": read += 2
    elif temp1 == False and temp2 == False: # 0
        k = 0
        j = 0
        while line[read+k+1:read+k+3] != "]]":
            if line[read+k+1:read+k+3] == "#s": j = k #표시되는 거에는 #s- 가 표시되면 안됨
            k = k + 1
        if j == 0: j = k # #s- 가 없을 수도.
        if line[read] == "/" and line[read+1] != "]": # 앞에 제목 안붙
            linecache[dir].append("<a href=\"entry://%s%s\">%s</a>" %(titlecache,line[read:read+k+1].replace("\\",""),line[read:read+j+1]))
        else: linecache[dir].append("<a href=\"entry://%s\">%s</a>" %(line[read:read+k+1].replace("\\",""),line[read:read+j+1]))
        #replace는 \'같은 거 처리
        read += k + 1
        while line[read-2:read] != "]]":
            read += 1
    elif temp1 and temp2: #3
        linecache[dir].append("[外]")
        while line[read] != '|':
            read += 1
        read += 1
        read = WikiParser(dir,read,"]]")
        if line[read:read+2] == "]]": read += 2
    else : # 2 는 스킵
        linecache[dir].append("[외]")
        while line[read-2:read] != "]]":
            read += 1
    temp = 0
    return read
#----------------------------------------------
def extra(dir,xcount,read): #[*
    global linecache
    global line
    read += 2
    #뒤에가 blank인지 스포일러 등의 글자인지 판단 필요
    if line[read] == " ":
        read += 1
        linecache[dir].append("<a name=\"r%d\"><a href=\"entry://#%d\">[%d]</a></a>"%(xcount,xcount,xcount))
        linecache[1].append("<br><a name=\"%d\"><a href=\"entry://#r%d\">[%d]</a></a>"%(xcount,xcount,xcount))
        xcount = xcount + 1
    else:
        k = read
        while line[k] != " " and line[k] != "]": k = k + 1
        linecache[dir].append("<a name=\"wr%s\"><a href=\"entry://#w%s\">[%s]</a></a>"%(line[read:k],line[read:k],line[read:k]))
        linecache[1].append("<br><a name=\"w%s\"><a href=\"entry://#wr%s\">[%s]</a></a>"%(line[read:k],line[read:k],line[read:k]))
        read = k
    read = WikiParser(1,read,"]")
    if line[read] == "]": read += 1 #",대비
    #linecache[0].append
    #WikiParser(1,read,"]")
    return xcount,read
#----------------------------------------------
#본 문법 함수
#----------------------------------------------
#----------------------------------------------
def WikiParser(dir,read,end): #linecache 위치 / read / 종결 문자열(Null 이면 원래대로)
    #표
    global xcount
    global index
    global TableOn
    if end == "":
        xcount = 1 #주석은 1부터.
        index = [0,0,0,0,0,0,0,0,0,0,0]
        TableOn = False
    inTable = False #삭제 필요
    tableLine = 0 #삭제 필요
    #표
    contentLine = 0
    strong = True
    italic = True
    cancelline = True #취소
    underline = True # 밑줄
    up = True #지수용
    down = True #밑
    inWikiTable = False
    readtm = -1
    global listree
    while True:
        if read == readtm: #무한루프 에러 기록.
            errfile.writelines("무한루프 에러:%s?%s?%s\r\n" %(line[read-10:read], line[read], line[read+1:read+10]))
            read += 1
        readtm = read
        #-------------------------------
        #기본 마크업
        #-------------------------------
        #
        #elif line[read:read+4] == "\\\'\\\'": # \'\' 이탤릭 처리/아직 구현 안됨.
        #    if italic :
        #        linecache[dir].append("<i>")
        #        italic = False
        #    else:
        #        linecache[dir].append("</i>")
        #        italic = True
        #    read += 4
        #
        if line[read:read+4] == "[목차]":
            linecache[0].append("TestTest") #목차로 덮어씨워질 부분
            listree = len(linecache[0]) - 1 # 0부터 시작
            if listree == -1:
                #print("Found Ya")
                listree = 0
            read += 4
        elif line[read:read+2] == ",,":
            if down :
                linecache[dir].append("<sub>")
                down = False
            else:
                linecache[dir].append("</sub>")
                down = True
            read += 2
        elif line[read:read+2] == "^^":
            if up :
                linecache[dir].append("<sup>")
                up = False
            else:
                linecache[dir].append("</sup>")
                up = True
            read += 2
        elif line[read:read+2] == "~~" or line[read:read+2] == "--":# ~~
            if cancelline :
                if line.find(line[read:read+2],read+2) < line.find("\\n",read+2):
                    linecache[dir].append("<font color=\"grey\"><s>")
                    cancelline = False
                else: linecache[dir].append(line[read:read+2])
            else:
                linecache[dir].append("</s></font>")
                cancelline = True
            read += 2
        elif line[read:read+2] == "__":# 밑줄
            if underline :
                if line.find("__",read+2) < line.find("\\n",read+2):
                    linecache[dir].append("<u>")
                    underline = False
                else: linecache[dir].append("__")
            else:
                linecache[dir].append("</u>")
                underline = True
            read += 2
        elif line[read:read+4].lower() == "[br]":
            if not strong:
                linecache[dir].append("</b>")
                strong = True
            if not cancelline:
                linecache[dir].append("</s></font>")
                cancelline = True
            if not underline:
                linecache[dir].append("</u>")
                underline = True
            linecache[dir].append("<br>")
            read += 4
        elif line[read] == "{":
            if line[read:read+3] == "{{{": read = TripleBrace(dir,read)#기타{{{
            elif line[read:read+3] == "{{|"  and line[read+3] != '|':#표랑 안엇갈리게
                linecache[dir].append("<table border=\"1\">")
                read += 3
            elif line[read:read+2] == "{|" and line[read+2] != '|':#표랑 안엇갈리게
                linecache[dir].append("<table border=\"1\">")
                inWikiTable = True
                read += 2
            else:
                linecache[dir].append("{")
                read += 1
        elif line[read] == "|":
            if line[read:read+3] == "|}}":
                linecache[dir].append("</table>")
                read += 3
            elif line[read:read+2] == "|}":
                linecache[dir].append("</table>")
                inWikiTable = False
                read += 2
            elif line[read:read+2] == "||":
                if inWikiTable:
                    linecache[dir].append("||")
                    read += 2
                elif end == "||" or TableOn == True: break # 표 안
                elif end == "]": 
                    errfile.write("주석 에러:%s(%s)\r\n" %(titlecache,line[read-10:read+1]))
                    break #주석에 표가 없다는 가정 하에.
                elif end == "]]": 
                    errfile.write("링크 에러(링크 안의 표):%s(%s)\r\n" %(titlecache,line[read-10:read+1]))
                    break #링크에 표가 없으니까.
                elif end == "||":
                    break # 표 문법 안.
                else:
                    TableOn = True
                    read = newTableFunc(dir,read)
                    TableOn = False
            else:
                linecache[dir].append("|")
                read += 1
                
        elif line[read:read+7] == "http://" or line[read:read+8] == "https://": #http 스킵
            read += 7
            while line[read] != ' ' and line [read:read+2] != "||" and line [read:read+2] != "\\n" and line[read:read+4] != "[br]" and line[read] != "]":
                read += 1
                if line[read-4:read] == ".jpg" or line[read-5:read] == ".jpeg" or line[read-4:read] == ".png" or line[read-4:read] == ".bmp":
                    break
        elif line[read:read+8].lower() == "youtube(": # 유튜브 구문 스킵
            while line[read] != ')':
                read += 1
        elif end == "]" and line[read:read+4] == "\\n\\n": #주석
            break
        elif line[read:read+11] == "attachment:":
            read += 11
            while line[read-4:read] == ".jpg" or line[read-5:read] == ".jpeg" or line[read-4:read] == ".png" or line[read-4:read] == ".bmp" or line[read] == " ":
                read += 1
        elif line[read-2:read+1] == "\\n>": #인용문 표
            linecache[dir].append("<table border=\"1\"><td bgcolor=\"#eeeeee\">")
            while line[read] == '>':
                read += 1
                read = WikiParser(dir,read,"\\n")
            linecache[dir].append("</td></table>") # 표끝마치기
        #--------------------------
        # 문법 스킵 시에도 들어가야 하는 sql문법(?)
        #--------------------------
        #\ 관련
        elif line[read] == '\\':
            if line[read:read+6] == "\\\'\\\'\\\'": # \'\'\' -> \'에 앞서게.
                if strong :
                    linecache[dir].append("<b>")
                    strong = False
                else:
                    linecache[dir].append("</b>")
                    strong = True
                read += 4#뒤에서 +2
            elif line[read+1] == 'n':
                linecache[dir].append("<br>") # \n
                contentLine = contentLine + 1 # 표를 위해 카운트
                if not strong:
                    linecache[dir].append("</b>")
                    strong = True
                if not cancelline:
                    linecache[dir].append("</s></font>")
                    cancelline = True
                if not underline:
                    linecache[dir].append("</u>")
                    underline = True
                if not down:
                    linecache[dir].append("</sub>")
                    down = True
                if not up:
                    linecache[dir].append("</sup>")
                    up = True
                if len(end)>=2 and end[1] == "=": break  #목차 처리
                elif end == "||" and line[read+2:read+4] == "||":
                    read += 2
                    break #
                elif end == "\\n":
                    read += 2
                    break # ">" 형 상자 처리
                #여기에 다중줄과 일반 다음 표 구분 알고리즘
                #
                #
            elif line[read+1] == '\\':   # \\
                linecache[dir].append("\\")
            elif line[read+1] == '\'':   # \'
                linecache[dir].append("\'")
            elif line[read+1] == '\"':   # \"
                linecache[dir].append("\"")
            elif line[read:read+4] == "\\n##": #주석 (\n##)
                read += 2
                while line[read:read+2] != "\\n" and line[read:read+2] != "\",":
                    read += 1
                #\n까지 옴
                read -= 2
            read += 2
        ###############################3
        elif line[read] == "/": #이미지 스킵.
            a = True
            for i in range(50):
                if line[read+i:read+i+3] == "jpg" or line[read+i:read+i+3] == "png" or line[read+i:read+i+3] == "gif":
                    read += i + 3
                    a = False
                    break
                elif line[read+i] == " " or line[read+i:read+i+2] == "\\n":
                    break
            if a:
                linecache[dir].append("/")
                read += 1
        elif line[read] == "[":
            if line[read:read+2] == "[[":
                if end == "]]":
                    errfile.write("링크 에러(중복 링크):%s(%s)\r\n" %(titlecache,line[read-10:read+1]))
                    break
                else: read = SqBracket(dir,read)#링크?
            #elif line[read] == '[':
            elif line[read:read+9].lower() == "[include(": #틀
                read += 9
                k = read
                while line[read:read+2] != ")]":
                    read += 1#slice 는 마지막 숫자를 잘라먹어서 괜춘.
                linecache[dir].append("<a href=\"entry://%s\">[%s]</a>"%(line[k:read],line[k:read]))
                read += 2
            elif line[read:read+2] == "[*": xcount,read = extra(dir,xcount,read)
            else:
                linecache[dir].append("[")
                read += 1
        elif line[read] == "=" and inTable == False:
            if end == " =":
                read = read - 1
                break
            read,index = indexFunc(dir,read,index)#목차
        #---------------<html 에러 방지>-----------------
        elif line[read] == '<':
            linecache[dir].append("&lt;")
            read += 1
        elif line[read] == '>': #\\n> 과는 헤깔릴 일 없음
            linecache[dir].append("&gt;")
            read += 1
        elif line[read:read+2] == "  ": #다중 스페이스
            linecache[dir].append(" &nbsp;")
            read += 2
        elif line[read] == '&':
            linecache[dir].append("&quot;")
            read += 1
        #-----------------------------------------------------------
        elif line[read] == '\"' and line[read+1] == ',': #and line[read-1] != '\\':
            break                #양식 밖의 ')
        elif end != "" and line[read:read+len(end)] == end: #커스텀 탈출 문자열 (""이면 말고)
            if not strong:
                linecache[dir].append("</b>")
                strong = True
            if not cancelline:
                linecache[dir].append("</s></font>")
                cancelline = True
            if not underline:
                linecache[dir].append("</u>")
                underline = True
            break
        else:
            linecache[dir].append(line[read])
            read += 1
    if end == "":
        if not strong:
            errfile.write("강조 에러:%s\r\n" %titlecache)
        if inTable == True:
            errfile.write("테이블 에러:%s\r\n" %titlecache)
    return read
#--------------------------------------------------
#
#
#
#
#--------------------------------------------------
print("Reading Cache")
for i in range(0,25):  #50):
    FileCache.append(infile.readline())
line = "".join(FileCache).decode('unicode-escape')
#outfile.writelines(FileCache)
#raise sdfS
print("Converting...")
while True:
    for i in range(0,100):
        index = [0,0,0,0,0,0,0,0,0,0,0] # 목차 초기화
        titlecache = list()
        isTemplate = False  # 필요없는 템플레이트 문서 스킵
        while line[read:read+13] != "\"namespace\":\"": #read 는 개수보다 1개 작음
            read += 1
        if line[read+13] == "1":
            titlecache.append("틀:")
        elif line[read+13] == "2" or line[read-1] == "3": # 사실 3은 이미지. 그러나 날리기 위해 그냥.
            titlecache.append("분류:")
        elif line[read+13] == "6":
            titlecache.append("나무위키:")
        while line[read:read+9] != "\"title\":\"" :  #제목 "title":"
            read += 1
        read += 9
        while line[read:read+3] != "\",\"" : ## title 끝으로
            titlecache.append(line[read])
            read += 1
        while line[read:read+8] != "\"text\":\"" :   ## "text":"
            read += 1
        read += 8 #본문으로


        titlecache = "".join(titlecache) #titlecahce 를 문자열
        if titlecache[:4] != "템플릿:" and titlecache[:3] != "분류:":#(Template 아님)
            if count != -1 : linecache[0].append("\r\n")
            else: count = 0
            linecache[0].append("%s\r\n<b><font size=\"5\">%s</font></b><br><hr>" %(titlecache, titlecache))
            #제목
            #(항목 내 제목)
            #------------(<hr>)
        else:       #Template이면
            isTemplate = True
        #----------------------------------------
        #|문법 해석부 (func화 해서 제목에도/재귀)     |
        #----------------------------------------
        #아래 : \', 는 계속, (\아님)', 이면 탈출 (\ 쪽이 확률이 높음.)
        
        if isTemplate :
            while True:
                if line[read:read+14] == "\"contributors\"": #"contributors"
                    break
        elif line[read:read+9] == "#redirect": #리다이렉트
            linecache[0].append("<a href=\"entry://")
            read += 10 # 공백 땜에 9 + 1
            k = 0
            while line[read+k+1:read+k+3] != "\\n" and line[read+k+1:read+k+3] != "')":
                k = k + 1
            linecache[0].append(line[read:read+k+1]+"\">리다이렉트:"+line[read:read+k+1]+"</a>")
            read += k + 1
        else : read = WikiParser(0,read,"") #위키 문법
        #------------------------------------------
        #line = line[read:]
        #read = 0
        #------------------------------------------
        if len(linecache[1]) != 0:
            linecache[0].append("<br><hr>")
            linecache[0].append("".join(linecache[1]))
            linecache[0].append("<br><br>")
            linecache[1] = list()
        if listree != -1:
            linecache[0][listree] = "<table border=\"1\"><td>%s</td></table>" %("".join(linecache[2]))
            linecache[2] = list()
            listree = -1
        elif len(linecache[2]) != 0:
            linecache[2] = list()
        if not isTemplate : linecache[0].append("\r\n</>")
        #내용
        #</>
        #제목
    count = count + 1
    if count % 100 == 0 :
        print(count)
        print("진행 시간: %.02f 초" % (time.time() - exectime))
        print("예상 시간: %.02f 분" % ((time.time() - exectime) / count * linecount /60))
        print("남은 시간: %.02f 분" % ((time.time() - exectime) / count * (linecount - count) /60))
    # ");\nINPUT
    #   ^
    # ");\nINPUT
    #      ^
    read += 2
    outfile.writelines(linecache[0])
    linecache = [list(),list(),list(),list(),list()] # 문자열 캐시 초기화
    try:
        if len(line) - read < 1500000:
            line = line[read:]
            read = 0
            if full == 0:
                FileCache = list() # 초기화
                FileCache.append(line)
                for i in range(0,40):
                    FileCache.append(infile.readline())
                if FileCache[40] == "":
                    infile.close()
                    full = -1
                line = "".join(FileCache).decode('unicode-escape')
            elif line.count(';') == 0: raise out # 리스트도 비고 세미콜론 없으면 끝.
    except:
        break
print("Done!")
print(count)
print("빌드 시간: %.02f분" % ((time.time() - exectime) / 60))
outfile.close()

