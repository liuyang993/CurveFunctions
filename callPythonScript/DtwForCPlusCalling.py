import sys
import numpy as np
import datetime as dt
import time
from statistics import mean 


def divide_chunks(l, n): 
      
    # looping till length l 
    for i in range(0, len(l), n):  
        yield l[i:i + n] 


def SplitArrayAndCalAver(arraySrc,arrayDst,n):
    x = list(divide_chunks(arraySrc, n)) 
    for j in range(len(x)):
        arrayDst.append(mean(x[j]))

def normalizeArray(pa):                 # 归一化
    amin,amax = min(pa),max(pa)
    for j in range(len(pa)):   # TODO check if amax == amin to avoid devide by zero   
        pa[j]=(pa[j]-amin)/(amax-amin)


def path_cost(x, y, accumulated_cost, distances):
    path = [[len(x)-1, len(y)-1]]
    cost = 0
    i = len(y)-1
    j = len(x)-1
    while i>0 and j>0:
        if i==0:
            j = j - 1
        elif j==0:
            i = i - 1
        else:
            if accumulated_cost[i-1, j] == min(accumulated_cost[i-1, j-1], accumulated_cost[i-1, j], accumulated_cost[i, j-1]):
                i = i - 1
            elif accumulated_cost[i, j-1] == min(accumulated_cost[i-1, j-1], accumulated_cost[i-1, j], accumulated_cost[i, j-1]):
                j = j-1
            else:
                i = i - 1
                j= j- 1
        path.append([j, i])
    path.append([0,0])
    for [y, x] in path:
        cost = cost +distances[x, y]
    return cost   


def callDTWBtwTwoArray(pa,pb):
    x=[]
    x.append(1)
    x.append(2)
    x.append(3)
    x.append(4)

    #xOneMinAver= []
    #SplitArrayAndCalAver(x,xOneMinAver,60)
    #x= xOneMinAver
    
    
    normalizeArray(x)


    print('orignal array have ' , len(x) , ' elements ')

    print('************************************************')


    y=[]

    y.append(1)
    y.append(5)
    y.append(6)
    y.append(7)



    #yOneMinAver = []
    #SplitArrayAndCalAver(y,yOneMinAver,60)
    #y=  yOneMinAver

    normalizeArray(y)

    distances = np.zeros((len(y), len(x)))    #建立一个 len(y) 行 , len(x) 列 的矩阵 ，并初始化为零
    #print(distances)

    for i in range(len(y)):
        for j in range(len(x)):
            distances[i,j] = (x[j]-y[i])**2    # 计算 y的每个点到 x的距离 

    #print("cal x y distance matrix use time %f seconds" % (time.time() - s))

    #print(distances)        

    #distance_cost_plot(distances)

    # 另一个矩阵
    accumulated_cost = np.zeros((len(y), len(x)))
    accumulated_cost[0,0] = distances[0,0]

    for i in range(1, len(x)):
        accumulated_cost[0,i] = distances[0,i] + accumulated_cost[0, i-1]    

    for i in range(1, len(y)):
        accumulated_cost[i,0] = distances[i, 0] + accumulated_cost[i-1, 0]    

    for i in range(1, len(y)):
        for j in range(1, len(x)):
            accumulated_cost[i, j] = min(accumulated_cost[i-1, j-1], accumulated_cost[i-1, j], accumulated_cost[i, j-1]) + distances[i, j]    #这句是最慢的 

    cost = path_cost(x, y, accumulated_cost, distances)

    return cost 

    #print('DTM value from ' , sys.argv[1]  , ' to ' , loopTableName  , ' is '  , cost)









