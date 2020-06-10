import matplotlib
import matplotlib.pyplot as plt
import numpy as np
mp=dict()
f=open("out.txt","r")
names=["720.jpg", "1080.jpg","4k.jpg"]
ejex=[1,2,4,8,16]
for k in range(3):
    mp[names[k]]=[[] for i in range(17)]
    for i in range(3,16,2):
        for j in range(5):
            acum = 0.0
            for idx in range(10):
                s=f.readline().strip().split()
                acum += float(s[-1])
            mp[s[0]][i].append(acum/10)
for i in range(3,16,2):
    plt.plot(ejex,mp[names[0]][i],'-o',label=names[0])
    plt.plot(ejex,mp[names[1]][i],'-o',label=names[1])
    plt.plot(ejex,mp[names[2]][i],'-o',label=names[2])
    plt.legend()
    plt.title("kernel = "+str(i))
    plt.ylabel("time sg")
    plt.xlabel("#threads")
    plt.savefig("graficas/"+str(i)+".jpg")
    plt.close()
for i in range(3,16,2):
    arr=[[0 for i in range(5)] for j in range(3)]
    for k in range(3):
        for j in range(5):
            arr[k][j]=(mp[names[k]][i][0]/mp[names[k]][i][j])
    plt.plot(ejex,arr[0],'-o',label=names[0])
    plt.plot(ejex,arr[1],'-o',label=names[1])
    plt.plot(ejex,arr[2],'-o',label=names[2])
    plt.legend()
    plt.title("speedup kernel = "+str(i))
    plt.ylabel("speedup")
    plt.xlabel("#threads")
    plt.savefig("graficas/speedup"+str(i)+".jpg")
    plt.close()
