# -*- coding:utf-8 -*-
from operator import eq
import time
start = time.time()
lineOfText , s , result , tmpString = [''], [''], [] , ''
file_1 = open('11', 'rt', errors='ignore')
file_2 = open('newResult.txt','w', encoding='utf-8')
count=0
for line in file_1:
     lineOfText = line.split('\t')
     i = 0
     for i in range(len(lineOfText)):
          if(lineOfText[i][0] == '/'): continue
          s = lineOfText[i].replace('/', ' ').replace('\t', ' ').split('+')
          strList , iter , c = '' , 0 , 0
          if(len(s) != 1):
               for c in range(len(s)-1):
                    strList = strList + s[c] + ' + '
               strList = strList + s[len(s) - 1]
               strList = strList.split()
               c = (int)(len(strList) / 3)
          else:
               c = 1
               strList = s[0].split()
          checkIfnextValueExist, appendIfLast ,iter = 0 , 0 , 0
          aIndex = 0
          for iter in range(c):
               aIndex = 3 * iter + 2
               if (checkIfnextValueExist == 1): aIndex = aIndex + iter
               # NNG, NNP, NNV, NNA = '일반명사', '고유명사', '동작성명사', '상태성명사'
               # NNBG, NNBU, NPP, NPD = '일반의존명사', '단위성의존명사', '인칭대명사', '지시대명사'
               # NRC, NRO = '양수사', '서수사'
               # VVI, VVT, VVIT = '자동사', '타동사', '자타동사'
               # VA = '형용사'
               # VX = '보조용언'  ## VXV,VXA="보조동사","보조형용사" 이걸로 안 되있음
               # VC = '지정사'
               # MMG, MMQ = '일반관형사', '수관형사'
               # MAG, MAJ = '일반부사', '접속부사'
               list = ['명사', '일반명사', '고유명사', '동작성명사', '상태성명사', '의존명사', '일반의존명사', '단위성의존명사', '대명사' , '인칭대명사', '지시대명사', '수사', '양수사', '서수사', '동사', '자동사', '타동사', '자타동사', '형용사', '보조용언', '보조동사', '보조형용사', '지정사', '관형사', '일반관형사', '수관형사', '부사', '일반부사', '접속부사']
               if(aIndex+2 < len(strList) and strList[aIndex+2]== '+'):  checkIfnextValueExist = 1                   # 뒤에 더하기가 있으면 checkIfnextValueExist = 1
               else:  checkIfnextValueExist = 0                   # 뒤에 더하기가 없으면 checkIfnextValueExist = 0
               for x in range(len(list)):
                    if(aIndex >= len(strList)) : continue
                    if (aIndex < len(strList) and strList[aIndex]==list[x]):
                         if(checkIfnextValueExist != 1):                                                       # +로 더할 때
                              if(aIndex >= 4 and strList[aIndex-4]== '수관형사'): tmpString = tmpString + strList[aIndex - 1]       # 수관형사 일 경우 그냥 appendIfLast
                              else :
                                   if(tmpString == ''): tmpString = tmpString + strList[aIndex - 1]        # 더할게 하나뿐인 경우
                                   else :
                                        result.append(tmpString)                     # 여러 단어 넣어야 할 경우
                                        tmpString = strList[aIndex - 1]
                              result.append(tmpString)
                              tmpString , appendIfLast = '' , 0
                              break
                         else:
                              appendIfLast = 1                                                       # 일단 더할 준비
                              if (tmpString == ''): tmpString = tmpString + strList[aIndex - 1]
                              else :
                                   if (strList[aIndex - 4] == '수관형사'):    tmpString = tmpString + strList[aIndex - 1]
                                   else:
                                        result.append(tmpString)
                                        tmpString = strList[aIndex - 1]

               if(checkIfnextValueExist == 0 and appendIfLast == 1):
                    result.append(tmpString)                           # 마지막 인 경우
                    tmpString = ''
                    appendIfLast=0
               if(checkIfnextValueExist == 0 and appendIfLast == 0):
                    break                                                                   # 중복 들어가는 것 막기
     file_2.write(' '.join(result))
     file_2.write('\n')
     result=[]
file_1.close()
file_2.close()

end = time.time() - start

print (end)
